/*
 * Copyright 2011 IPOL Image Processing On Line http://www.ipol.im/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file canny.c
 * @brief Canny filter
 *
 * @author Vincent Maioli <vincent.maioli@crans.org>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "io_png.h"
#include "length_dsf.h"
#include "canny.h"

// Gestion des bords par diffusion
int value(int x, int y, size_t nx, size_t ny) {
	//Prolongement miroir
	int xt, yt;
	if (x < 0) 
		xt = -x;
	else 
		if (x > (int)nx -1) 
			xt = 2*nx - 2 - x;
		else 
			xt = x;
	if (y < 0) 
		yt = -y;
	else 
		if (y > (int)ny - 1) 
			yt = 2*ny - 2 - y;
		else 
			yt = y;
	return xt + nx*yt;

}
// Interpolation bilinéaire spécifique.
// 
double bilin(double* grad, double t, size_t x, size_t y, size_t nx, size_t ny, int dir) {

	double x1,x2,xt,y1,y2,yt;

	xt = dir * cos(t);
	yt = dir * sin(t);	

	x1 = floor(xt), x2 = x1 + 1;
	y1 = floor(yt), y2 = y1 + 1;


	double temp = grad[value(x+x1,y+y1,nx,ny)]*(x2-xt)*(y2-yt)
		- grad[value(x+x2,y+y1,nx,ny)]*(x1-xt)*(y2-yt)
		- grad[value(x+x1,y+y2,nx,ny)]*(x2-xt)*(y1-yt)
		+ grad[value(x+x2,y+y2,nx,ny)]*(x1-xt)*(y1-yt);

	return temp;

}

static void maxima(double* grad, double *theta, unsigned char *output, 
		   size_t nx, size_t ny, int seuil_bas,int seuil_haut) {
	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			double t = theta[y*nx + x];

			double prev = bilin(grad,t,x,y,nx,ny,-1);			
			double next = bilin(grad,t,x,y,nx,ny,1);			
			double now = grad[y*nx+x];

			if ((now <= prev) || (now <= next) || (now <= seuil_bas))
				output[y*nx + x] = 0;
			else if (now >= seuil_haut)
				output[y*nx + x] = 2;
			else 
				output[y*nx + x] = 1;
			//grad[y*nx+x]);
		}
	}
}

static const char *help =
	"lost_train usage:\n"
	"\t-h | --help          Display this help message\n"
	"\t-v | --version	Display the version of this program\n"
	"\t-s | --sigma   DBLE  Gaussian filter's variance\n"
	"\t-lt	          DBLE  Low threshold\n"
	"\t-ht	          DBLE 	High threshold\n"
	"\t-i | --input   FILE  Input file\n"
	"\t-o | --output  FILE  Output file\n"
	;

/**
 * @brief main function call
 *
 */
int main(int argc, char *const *argv)
{
	size_t nx, ny, nc;              /* data size */
	char *input_file = '\0', *output_file = '\0'; //Name of the files
	unsigned char *input = NULL, *output = NULL;        /* input/output data */
	//TODO : Pour le moment : ne travaille que sur le premier canal
//	int channel = 1;
	//s : sigma, variance du filtre
	int s = 10;
	double seuil_bas=4, seuil_haut = 5;


	// First step: parse command line argument and check that parameter
	// syntax is valid, no check for parameter value vailidity here, just
	// basic parsing.
	argc--, argv++;
	while (argc != 0) {
		const char *arg = argv[0];
		if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			fprintf(stderr, "%s", help);
			exit(EXIT_SUCCESS);
		}

 		if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			fprintf(stdout, "%s version " __DATE__ "\n", argv[0]);
			exit(EXIT_SUCCESS);
		}
	
		if (!strcmp(arg, "-i") || !strcmp(arg, "--input")) {
			if (argc == 1)
				error("missing filename for %s", arg);
			input_file = argv[1];
			argc -= 2, argv += 2;
			continue;
		}
		if (!strcmp(arg, "-o") || !strcmp(arg, "--output")) {
			if (argc == 1)
				error("missing filename for %s", arg);
			output_file = argv[1];
			argc -= 2, argv += 2;
			continue;
		}
		if (!strcmp(arg, "-s") || !strcmp(arg, "--sigma")) {
			if (argc == 1)
				error("missing value for %s", arg);
			s = atol(argv[1]);
			argc -= 2, argv += 2;
			continue;
		}
		if (!strcmp(arg, "-lt")) {
			if (argc == 1)
				error("missing value for %s", arg);
			seuil_bas = atol(argv[1]);
			argc -= 2, argv += 2;
			continue;
		}
		if (!strcmp(arg, "-ht")) {
			if (argc == 1)
				error("missing value for %s", arg);
			seuil_haut = atol(argv[1]);
			argc -= 2, argv += 2;
			continue;
		}
	}

	// Second step: validate the set of parameter to be used in training, we
	// check here that all needed parameters are sets or have a default and
	// that the combination of all these make a valid set.
	if (input_file[0] == '\0')
		error("Missing input file");
	if (output_file[0] == '\0')
		error("Missing output file");
		
	/* read the PNG image */
//	input = read_png_u8_rgb(input_file, &nx, &ny);
	// TODO : use f32 read/write functions
//	input = read_png_u8(input_file, &nx, &ny, &nc);

	input = read_png_u8_gray(input_file, &nx, &ny);
nc = 1;
//	input = read_png_f32(input_file, &nx, &ny, &nc);
	if (input == NULL)
		error("the image could not be properly read");
printf("%d\n",nc);
	int N = nx*ny;
	double* in = xmalloc(sizeof(double) * nx * ny * nc);
	
	for(size_t d = 0 ; d < nx*ny*nc; d++) 
			in[d] = (double) input[d];
		
	free(input);
	double* data = xmalloc(sizeof(double) * nx * ny * nc);

	//filtrage gaussien	
	gblur(data, in, nx, ny, nc, s);

	free(in); 

	double* grad = xmalloc(sizeof(double) * nx * ny);
	double* theta = xmalloc(sizeof(double) * nx * ny);


	output = xmalloc(sizeof(unsigned char) * nx * ny);



for(int c = 0 ; c < nc ; c++) {
	//Valeur du gradient :

	double hgrad,vgrad;

	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			//Gradient horizontal
			hgrad = data[value(x+1,y,nx,ny) + c*N] - data[value(x-1,y,nx,ny) + c*N];
			//Gradient vertical
			vgrad = data[value(x,y+1,nx,ny) + c*N] - data[value(x,y-1,nx,ny) + c*N];
			//Norme (L1) du gradient
			grad[x + y*nx] = fabs(hgrad) + fabs(vgrad); 
			//Direction du gradient
			// si vgrad est à zero :
			// hgrad neg : -pi/2 sinon pi/2
			// automatiquement fait par atan <3
			//Mais on veut pas (sinon on a pas -pi/4)
			if(vgrad==0)
				theta[x + y*nx] = M_PI_2; 
			else
				theta[x + y*nx] = atan(hgrad / vgrad);

		}
	}

	//Suppression des non-maxima
	// seuil bas, on construit l'arbre, seuil haut 
	// Pour chaque canal on ajoute à ce qui avait été fait pour les
	// canaux précédents
	maxima(grad,theta,output,nx,ny,seuil_bas,seuil_haut);
}

	int *t = xmalloc(sizeof(int) * N);
	int *size = xmalloc(sizeof(int) * N);
	adsf_begin(t,N,size);

	// On construit les arbres
	for(size_t x = 0 ; x < nx ; x++) 
		for(size_t y = 0 ; y < ny ; y++) {
			int d = x + nx * y;
			if(output[d]) {
				for(int ex = -1 ; ex < 2 ; ex++)
				for(int ey = -1 ; ey < 2 ; ey++) {
					int ed = value(x+ex,y+ey,nx,ny);
					if(output[ed])	                                       
						adsf_union(t,N,d, ed, size);
				}
			}
		}

	// On marque tous les arbres dont un noeuds est plus grand que le seuil_haut
	for(int d = 0 ; d < N ; d++)
		if(output[d] == 2)
			output[adsf_find(t,N,d)] = 2;

	adsf_assert_consistency(t,N,size);
	
	// On supprime tous les arbres dont la racine n'est pas marquée
	for(int d = 0 ; d < N ; d++) 
		if(output[adsf_find(t,N,d)]  < 2) 
			output[d] = 0;
		else
			output[d] = (char) -1;
	

	free(grad);
	free(theta);
	free(data);

	/* write the mask as a PNG image */
	int write  = write_png_u8(output_file, output, nx, ny, 1);
	if (write == -1)
		error("the image could not be properly written");
	free(output);
	return EXIT_SUCCESS;

}

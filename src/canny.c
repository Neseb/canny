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
		if (x > nx -1) 
			xt = 2*nx - 2 - x;
		else 
			xt = x;
	if (y < 0) 
		yt = -y;
	else 
		if (y > ny - 1) 
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

static void maxima(double* grad, double *theta, unsigned char *output, size_t nx, size_t ny, int channel, int seuil_bas,int seuil_haut) {
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

/**
 * @brief main function call
 *
 */
int main(int argc, char *const *argv)
{
	size_t nx, ny;              /* data size */
	unsigned char *input, *output;        /* input/output data */
	//TODO : Pour le moment : ne travaille que sur le premier canal
	int channel = 1;
	//	int gausSize = 5;
	//s : sigma, variance du filtre
	int s = 1;

	/* "-v" option : version info */
	if (2 <= argc && 0 == strcmp("-v", argv[1]))
	{
		fprintf(stdout, "%s version " __DATE__ "\n", argv[0]);
		return EXIT_SUCCESS;
	}
	/* wrong number of parameters : simple help info */
	if (3 != argc)
	{
		fprintf(stderr, "usage : %s in.png out.png\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* read the PNG image */
	input = read_png_u8_rgb(argv[1], &nx, &ny);
	if (input == NULL)
	{
		fprintf(stderr, "the image could not be properly read\n");
		return EXIT_FAILURE;
	}

	double* in = xmalloc(sizeof(double) * nx * ny * channel);

	for(size_t x = 0 ; x < nx ; x++)
		for(size_t y = 0 ; y < ny ; y++)
			in[y*nx+x] = input[y*nx+x];

	free(input);
	double* data = xmalloc(sizeof(double) * nx * ny * channel);

	//filtrage gaussien	
	gblur(data, in, nx, ny, channel, s);

	free(in); 

	double* grad = xmalloc(sizeof(double) * nx * ny);
	double* theta = xmalloc(sizeof(double) * nx * ny);

	//Valeur du gradient :

	double hgrad,vgrad;

	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			//Gradient horizontal
			hgrad = data[value(x+1,y,nx,ny)] - data[value(x-1,y,nx,ny)];
			//Gradient vertical
			vgrad = data[value(x,y+1,nx,ny)] - data[value(x,y-1,nx,ny)];
			//Norme (L1) du gradient
			grad[y*nx+x] = fabs(hgrad) + fabs(vgrad); 
			//Direction du gradient
			// si vgrad est à zero :
			// hgrad neg : -pi/2 sinon pi/2
			// automatiquement fait par atan <3
			//Mais on veut pas (sinon on a pas -pi/4)
			if(vgrad==0)
				theta[y*nx+x] = M_PI_2; 
			else
				theta[y*nx+x] = atan(hgrad / vgrad);

		}
	}

	//Suppression des non-maxima

	output = xmalloc(sizeof(unsigned char) * nx * ny * channel);

// seuil bas, on construit l'arbre, seuil haut
// seuil haut puis seuil bas
// les deux mélangés
	double seuil_bas=4, seuil_haut = 5;

	maxima(grad,theta,output,nx,ny,channel,seuil_bas,seuil_haut);
	
	int N = nx*ny;

	int t[N];
	int size[N];
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
			output[d] = HUGE;
	

	free(grad);
	free(theta);
	free(data);

	/* write the mask as a PNG image */
	write_png_u8(argv[2], output, nx, ny, 1);
	free(output);

	return EXIT_SUCCESS;

}

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
double value(double* array, int x, int y, size_t nx, size_t ny) {
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
	return array[xt + nx*yt];

}
// Interpolation bilinéaire spécifique.
// 
double bilin(double* grad, double t, size_t x, size_t y, size_t nx, size_t ny, int dir) {

	double x1,x2,xt,y1,y2,yt;

	xt = dir * cos(t);
	yt = dir * sin(t);	

	x1 = floor(xt), x2 = x1 + 1;
	y1 = floor(yt), y2 = y1 + 1;


	double temp = value(grad,x+x1,y+y1,nx,ny)*(x2-xt)*(y2-yt)
		- value(grad,x+x2,y+y1,nx,ny)*(x1-xt)*(y2-yt)
		-  value(grad,x+x1,y+y2,nx,ny)*(x2-xt)*(y1-yt)
		+  value(grad,x+x2,y+y2,nx,ny)*(x1-xt)*(y1-yt);

	return temp;

}

static void maxima(double* grad, double *theta, unsigned char *output, size_t nx, size_t ny, int channel) {
	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			double t = theta[y*nx + x];

			double past = bilin(grad,t,x,y,nx,ny,-1);			
			double future = bilin(grad,t,x,y,nx,ny,1);			
			double present = grad[y*nx+x];

			output[y*nx + x] = (present <= past) ? 0 : ((present <= future) ? 0 : grad[y*nx+x]);

		}
	}
}

/*static void maxima_dumb(double* grad, int *theta, unsigned char *mask, size_t nx, size_t ny, int channel) {
	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			int t = floor((M_PI_2 + theta[y*nx+x])/M_PI_4);
			int ex,ey;
			switch(t) {
				case 0:
					ex = 0;
					if((y==0)||(y==ny-1))
						ey = 0;
					else
						ey = 1;
					break;

				case 1:
					if((x==0)||(y==0)||(x==nx-1)||(y==ny-1))
						ex = ey = 0;
					else
						ex = -1, ey = 1;
					break;
				case 2:	
					ey = 0;
					if((x==0)||(x==nx-1))
						ex = 0;
					else
						ex = 1;
					break;
				case 3:
					if((x==0)||(y==0)||(x==nx-1)||(y==ny-1))
						ex = ey = 0;
					else
						ex = 1, ey = -1;
					break;
				default: 
					error(sprintf("Erreur : \ndirection= %d\ngrad= %g",t,grad[y*nx+x]));
			}	
			assert((x-ex)+nx*(y-ey) < nx*ny);

			assert((x+ex)+nx*(y+ey) < nx*ny);

			double past = grad[(x-ex)+nx*(y-ey)];
			double future = grad[(x+ex)+nx*(y+ey)];
			double present = grad[y*nx+x];

			// Ajouter un moyen de détecter que les variations de gradient sont minimale
			// Hey dude, isn't that hysteresis filtering ?!
			mask[y*nx + x] = (present <= past) ? 0 : ((present <= future) ? 0 : grad[y*nx+x]);
			//s_HUGE_data[y*nx+x]_
			//s_HUGE_grad[y*nx+x]_

		}
	}
}*/

/**
 * @brief unsigned char comparison

 static int cmp_u8(const void *a, const void *b)
 {
 return (int) (*(const unsigned char *) a - *(const unsigned char *) b);
 }
 */
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
			hgrad = value(data,x+1,y,nx,ny) - value(data,x-1,y,nx,ny);
			//Gradient vertical
			vgrad = value(data,x,y+1,nx,ny) - value(data,x,y-1,nx,ny);
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

	maxima(grad,theta,output,nx,ny,channel);
	

// on applique le seuil bas, on construit l'arbre, on applique le seuil haut
// seuil haut puis seuil bas
// les deux mélangés

	int N = nx*ny;
	int t[N];
	int size[N];
	asdf_begin(t,N,size);
	for(int d = 0 ; d < N ; d++) {
	
		asdf_union(t,N,d, .., size);
		ajouter d à ses voisins
	}
	asdf_assert_consistency(t,N,size);
	for(int d = 0 ; d < N ; d++) {
	
	si d < seuil bas
			effacer
		if(size[asdf_find(t,N,d)])  < seuil haut
			output[d] = 0;
			pour c < N 
				si père(c) = d pere(c) = pere(d);
			père(d) = d; 
			
		sinon garder
	}

	//TODO : rajouter hysteresis

	free(grad);
	free(theta);
	free(data);

	/* write the mask as a PNG image */
	write_png_u8(argv[2], output, nx, ny, 1);
	free(output);

	return EXIT_SUCCESS;

}

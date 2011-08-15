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
#include "canny.h"

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
	unsigned char *input, *mask;        /* input/output data */
	double *data, *in;
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

	in = xmalloc(sizeof(double) * nx * ny * channel);

	for(size_t x = 0 ; x < nx ; x++)
		for(size_t y = 0 ; y < ny ; y++)
			in[y*nx+x] = input[y*nx+x];

	free(input);
	data = xmalloc(sizeof(double) * nx * ny * channel);

	//filtrage gaussien	
	gblur(data, in, nx, ny, channel, s);

	free(in); 

	double* grad = xmalloc(sizeof(double) * nx * ny);
	int* theta = xmalloc(sizeof(int) * nx * ny);

	//Valeur du gradient :

	double hgrad,vgrad;

	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			//Gradient horizontal
			hgrad = (x==0) ? (data[y*nx+1] - data[y*nx]) : ((x==nx-1) ? (data[y*nx+(nx-1)] - data[y*nx+(nx-2)]) : (data[y*nx+(x+1)] - data[y*nx+(x-1)]));
			//Gradient vertical
			vgrad = (y==0) ? (data[x+nx] - data[x]) : ((y==ny-1) ? (data[x+nx*(ny-1)] - data[x + nx*(ny-2)]) : (data[y*nx+x+1] - data[y*nx+x-1]));
			//Norme (L1) du gradient
			grad[y*nx+x] = fabs(hgrad) + fabs(vgrad); 
			//Direction du gradient
			// si vgrad est à zero :
			// hgrad neg : -pi/2 sinon pi/2
			// automatiquement fait par atan <3
			//Mais on veut pas (sinon on a pas -pi/4
			if(vgrad==0 ) 
				theta[y*nx+x] = 0; 
			else
				theta[y*nx+x] = floor((M_PI_2 + atan(hgrad / vgrad))/M_PI_4);
			
		}
	}

	//Suppression des non-maxima

	mask = xmalloc(sizeof(unsigned char) * nx * ny * channel);
	for(size_t x = 0 ; x < nx ; x++) {
		for(size_t y = 0 ; y < ny ; y++) {
			int t = theta[y*nx+x];
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
			printf("Erreur : \ndirection= %d\ngrad= %g",t,grad[y*nx+x]);
					 return EXIT_FAILURE;
			}	
			assert((x-ex)+nx*(y-ey) < nx*ny);

			assert((x+ex)+nx*(y+ey) < nx*ny);
		
			double past = grad[(x-ex)+nx*(y-ey)];
			double future = grad[(x+ex)+nx*(y+ey)];
			double present = grad[y*nx+x];

		// Ajouter un moyen de détecter que les variations de gradient sont minimale
		// Hey dude, isn't that hysteresis filtering ?!
		mask[y*nx + x] = (present <= past) ? 0 : ((present <= future) ? 0 : HUGE);
		//s_HUGE_data[x*y*nx+x]_
		}
	}
	
	//TODO : rajouter hysteresis
	free(grad);
	free(theta);
	free(data);
	/* write the mask as a PNG image */
	write_png_u8(argv[2], mask, nx, ny, 1);
	free(mask);
	
	return EXIT_SUCCESS;

}

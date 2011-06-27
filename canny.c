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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "io_png.h"

/**
 * @brief unsigned char comparison
 */
static int cmp_u8(const void *a, const void *b)
{
    return (int) (*(const unsigned char *) a - *(const unsigned char *) b);
}

/**
 * @brief main function call
 *
 * The saturation is processed half at both ends of the histogram.
 */
int main(int argc, char *const *argv)
{
    float s;                    /* saturated percentage */
    size_t nx, ny;              /* data size */
    unsigned char *data;        /* input/output data */
    unsigned char *tmpdata;     /* temporary sort data */
    int channel;

    /* "-v" option : version info */
    if (2 <= argc && 0 == strcmp("-v", argv[1]))
    {
        fprintf(stdout, "%s version " __DATE__ "\n", argv[0]);
        return EXIT_SUCCESS;
    }
    /* wrong number of parameters : simple help info */
    if (4 != argc)
    {
        fprintf(stderr, "usage : %s S in.png out.png\n", argv[0]);
        fprintf(stderr, "        S saturated pixels percentage [0...100[\n");
        return EXIT_FAILURE;
    }

    /* flattening percentage */
    s = atof(argv[1]);
    if (0. > s || 100. <= s)
    {
        fprintf(stderr, "the saturation percentage must be in [0..100[\n");
        return EXIT_FAILURE;
    }

    /* read the TIFF image */
    if (NULL == (data = read_png_u8_rgb(argv[2], &nx, &ny)))
    {
        fprintf(stderr, "the image could not be properly read\n");
        return EXIT_FAILURE;
    }











unsigned char* mask = malloc(sizeof(unsigned char) * nx * ny);

//Valeur du gradient :

double hgrad,vgrad;

for(x = 0 ; x < nx ; x++) {
	for(y = 0 ; y < ny ; y++) {
		//Gradient horizontal
		hgrad = hgrad[x*ny + y] = (x==0) ? (data[ny + y] - data[[nx-1]*ny + y]) : ((x==nx-1) ? (data[y] - data[[nx-2]*ny + y]) : (data[[x+1]*ny + y] - data[[x-1]*ny + y]));
		//Gradient vertical
		vgrad = (y==0) ? (data[x*ny + 1] - data[x*ny + ny-1]) : ((y==ny-1) ? (data[x*ny] - data[x*ny + ny-2]) : (data[x*ny + y+1 ] - data[x*ny + y-1]));
		//Norme (L1) du gradient
		grad[x*ny + y] = fabs(hgrad) + fabs(vgrad); 
		//Direction du gradient
		// si vgrad est à zero :
		// hgrad neg : -pi/2 sinon pi/2
		// automatiquement fait par atan <3
		//Mais on veut pas (sinon on a pas -pi/4
		theta[x*ny + y] = floor((M_PI_2 + atan(hgrad / vgrad))/M_PI_4); 
	}
}

//Suppression des non-maximums

for(x = 0 ; x < nx ; x++) {
	for(y = 0 ; y < ny ; y++) {
		double past, future;
		int t = theta[x*ny + y];
		switch(t) {
			case 0:
			case 4: 
				past = data[x*ny + y-1];
				future = data[x*ny + y+1];
				break;
			case 1:
				future = data[[x+1]*ny + y+1];
				past = data[[x-1]*ny + y-1];
				break;
			case 2:	
				future = data[[x+1]*ny + y];
				past = data[[x-1]*ny + y];
				break;
			case 3:
				fx = x+1;
				fy = y-1;
				px = x-1;
				py = 
				future = data[x+1,y-1];
				past = data[x-1,y+1]
				break;
			default: printf("Une couille dans le potage");
				return;
				break;
		}	
		double future = data[fx,fy];
		double past = data[px,py]
		double present = data[x*ny + y];
		mask[x * ny + y] = (present < past) ? 0 : ((present < future) ? 0 : 1);
	}
}

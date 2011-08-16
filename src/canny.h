#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

void maxima(double* grad,int *theta,unsigned char *mask,size_t nx,size_t ny, int channel);
void error(const char *fmt, ...);
void *xmalloc(size_t size);
void gblur(double *y, double *x, int w, int h, int pd, double s);

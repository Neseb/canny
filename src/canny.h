#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

#include "length_dsf.h"

void error(const char *fmt, ...);
void *xmalloc(size_t size);
void gblur(double *y, double *x, int w, int h, int pd, double s);

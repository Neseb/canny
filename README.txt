% Color balance with saturation

# ABOUT

* Author    : Vincent Maioli <maioli@crans.org>
* Copyright : (C) 2011 IPOL Image Processing On Line http://www.ipol.im/
* Licence   : GPL v3+, see GPLv3.txt

# OVERVIEW


This source code provides an implementation of the Canny filter, as described 
in IPOL in
    http://www.ipol.im/pub/algo/lmps_simplest_color_balance/

* normalize_sort uses a simple sorting method
* normalize_histo uses an histogram method, more efficient and
  requiring less memory

This program reads PNG image, then scale the pixel values by an
affine function such that a user-defined number of pixels are scaled
over the [0..255] limits. These pixels are replaced by the 0 or 255
value, and these values are written as a PNG image.

Only 8bit RGB PNG images are handled. Other PNG files are implicitly
converted to 8bit color RGB.

# REQUIREMENTS

The code is written in ISO C99, and should compile on any system with
an ANSI C compiler.

The libpng and header and the libpng and fftw3 libraries are required
on the system for compilation and execution.

# COMPILATION

Simply use the provided makefile, with the command `make`.

# USAGE

'canny' takes up to 6 parameters: 

`canny -s sigma -lt low_t -ht high_t [-a] -i in -o out`

* `sigma`   : gaussian blur standard deviation
* `low_t`   : low threshold
* `high_t`  : high threshold
* `-a` 	    : (optional) triggers the higher-order gradient computation 
* `in`      : input image
* `out`     : output image

See also `canny -h`.

# ABOUT THIS FILE

Copyright 2011 IPOL Image Processing On Line http://www.ipol.im/
Author: Vincent Maioli <maioli@crans.org>

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.


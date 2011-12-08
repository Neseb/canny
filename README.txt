% Canny edge detector

# ABOUT

* Author    : Vincent Maioli <maioli@crans.org>
* Copyright : (C) 2011 IPOL Image Processing On Line http://www.ipol.im/
* Licence   : GPL v3+, see GPLv3.txt

# OVERVIEW


This source code provides an implementation of the Canny filter, as presented for publication 
in IPOL.

This program reads PNG image, then output a map of all edge points of the image.

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


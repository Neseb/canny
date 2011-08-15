# Copyright 2009, 2010 IPOL Image Processing On Line http://www.ipol.im/
# Author: Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty provided
# the copyright notice and this notice are preserved.  This file is
# offered as-is, without any warranty.

CC	= cc

CSRC	= src/*.c 

SRC	= $(CSRC)
HDR	= src/*.h
BIN	= canny


COPT	= -O3 -funroll-loops -fomit-frame-pointer
CFLAGS	=  -std=c99 -W -Wall $(COPT)
LIBS	= -lpng -lfftw3 -lm

default: $(BIN)

canny	: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o canny $(LIBS) $(SRC)
	$(RM) src/*.o	
	
.PHONY	: clean distclean
clean	:
	$(RM) *.o
distclean	: clean
	$(RM) $(BIN)

# extra tasks
.PHONY  : lint beautify release
lint    : $(CSRC)
	splint -weak $^;
beautify        : $(CSRC)
	for FILE in $^; do \
		expand $$FILE | sed 's/[ \t]*$$//' > $$FILE.$$$$ \
		&& indent -kr -bl -bli0 -i4 -l78 -nut -nce -sob -sc \
		$$FILE.$$$$ -o $$FILE \
		&& rm $$FILE.$$$$; \
	done
release :
	git archive --format=tar --prefix=canny/ HEAD \
		| gzip > ../canny.tar.gz

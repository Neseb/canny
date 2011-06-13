# Copyright 2009, 2010 IPOL Image Processing On Line http://www.ipol.im/
# Author: Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty provided
# the copyright notice and this notice are preserved.  This file is
# offered as-is, without any warranty.

CSRC	= io_png.c canny.c

SRC	= $(CSRC)
OBJ	= $(CSRC:.c=.o)
BIN	= canny

COPT	= -O3 -funroll-loops -fomit-frame-pointer
CFLAGS	= -ansi -pedantic -Wall -Wextra -Werror $(COPT)

default: $(BIN)

%.o	: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

canny	: io_png.o canny.o
	$(CC) $(CFLAGS) -o $@ -lpng $^

.PHONY	: clean distclean
clean	:
	$(RM) $(OBJ)
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

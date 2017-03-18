# Linux makefile for Neural Net adaline program
#
# Usage:
# make				incrementally makes a debug version 
# make clean		cleans the source tree of *.o
#

OPTS = -g -D_DEBUG
CC = c++
AR = ar
RANLIB = ranlib
LDFLAGS = -Wl,-rpath,/usr/X11R6/lib -L/usr/X11R6/lib
LIBS = -lX11 
DEFS = 
CFLAGS = -I../bw/include -Wall -Werror $(OPTS) $(DEFS)
CPPFLAGS = 

%.o: %.cc
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

SOURCES = adaline.cc
OBJECTS = adaline.o

# targets
all: adaline

adaline: $(OBJECTS) ../bw/libbw.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o adaline $(OBJECTS) ../bw/libbw.a $(LIBS)

clean:
	rm -f *.o adaline


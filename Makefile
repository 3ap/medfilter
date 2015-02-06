CC=gcc
CFLAGS=-I.

all: medfilter gensig
medfilter: medfilter.c
	$(CC) $(CFLAGS) medfilter.c -o medfilter

gensig: gensig.c
	$(CC) $(CFLAGS) gensig.c -o gensig

clean:
	rm -rf *.o medfilter gensig

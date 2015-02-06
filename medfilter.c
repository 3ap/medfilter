#include "medfilter.h" /* median, filter, usage */

void usage(void) {
	fprintf(stderr, "Usage: medfilter WINDOW\n"); /* argv[0] may be better */
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	if(argc != 2) 
		usage();

	uint32_t wndsz;
	sscanf(argv[1], "%"SCNu32, &wndsz);
	if(wndsz % 2 == 0 || wndsz == 0) {
		perror("Window must be odd and positive");
		exit(2);
	}

	filter(wndsz, stdin, stdout);
	return 0;
}

void filter(uint32_t wndsz, FILE* in, FILE* out) {
	/* (wndsz-1)/2 */
	uint32_t halfwndsz = wndsz >> 1;

	/* buffers for signal and window */
	int32_t* sigbuf;
	int32_t* wndbuf;

/*	wndbuf = (int32_t*)mmap(NULL, sizeof(int32_t) * wndsz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (wndbuf == MAP_FAILED) { 
		perror("Decrease window");
		exit(EXIT_FAILURE);	
	}
*/
	// TODO: calloc killed
	wndbuf = (int32_t*)calloc(sizeof(int32_t), wndsz);
	size_t sigbufsz = (wndsz > MAX_BUFFER ? wndsz+halfwndsz : MAX_BUFFER);
	sigbuf = (int32_t*)calloc(sizeof(int32_t), sigbufsz);

	/*
	   sigbuf 	=	[ x x x x x x  n n n n n n n n n ]
	   			-------------  -------------------
	   			prev halfwnd   new part of signal
	*/

	size_t n;
	int c;

	/* pointer to first element of part of signal */
	int32_t* first_sigel = sigbuf+halfwndsz; 
	/* pointer to last element of part of signal */
	int32_t* last_sigel;

	/* iterators */
	int32_t* i;
	int32_t* j;
	int32_t cc = 0;

	/* median of window */
	int32_t m;

	/* fill ``prev halfwnd'' by first element of signal at first iteration */
	char first_iteration = 0;
	while(n = fread(first_sigel, sizeof(int32_t), sigbufsz-halfwndsz, in)) {
		if(n < wndsz && !first_iteration) {
			fprintf(stderr, "Window size more than input length (%"SCNu32" > %d)\n", wndsz, n);
			exit(2);
		}
		#ifdef DEBUG
			uint32_t l1;
			fprintf(stderr, "sigbuf in = {");
			for(l1 = 0; l1 < sigbufsz; l1++)
				fprintf(stderr, "%"SCNi32",", sigbuf[l1]); 
			fprintf(stderr, "}\n");
		#endif

		last_sigel = first_sigel + n - 1;
		for(i = first_sigel; i != last_sigel + 1; i++) {
			if(!first_iteration) {
				for(j = i - halfwndsz; j != i; j++) {
					(*j) = *i;
				}
				first_iteration = 1;
			}
			c = 0;
			for(j = i - halfwndsz; j != i + halfwndsz + 1; j++) {
				if(j > last_sigel)
					wndbuf[c++] = *last_sigel;
				else
					wndbuf[c++] = *j;
			}
			#ifdef DEBUG
				fprintf(stderr, "arr for %"SCNi32" (len: %"SCNi32"): { ", *i, wndsz);
				for(l1 = 0; l1 < wndsz; l1++)
					fprintf(stderr, "%"SCNi32", ", wndbuf[l1]);
			#endif
			m = medium(wndbuf, wndsz);
			#ifdef DEBUG
				fprintf(stderr,"}, median: %"SCNi32"\n", m);
			#endif
			fwrite(&m, sizeof(int32_t), 1, out);
		} 
		c = 0;
		for(i = last_sigel - halfwndsz + 1; i != last_sigel + 1; i++) {
			sigbuf[c++] = *i;
		}
		#ifdef DEBUG
			fprintf(stderr, "sigbuf out = {");
			for(l1 = 0; l1 < sigbufsz; l1++)
				fprintf(stderr, "%"SCNi32",", sigbuf[l1]); 
			fprintf(stderr, "}\n");
		#endif
	}
	free(sigbuf);
	free(wndbuf);
}

/* Fast, destructive median selection, credit to Nicolas Devillard for
   the original C implementation. */
#define SWAP(a,b) { register int32_t t=(a);(a)=(b);(b)=t; }
int32_t medium(int32_t* arr, uint32_t n)
{
	long low, high;
	long median;
	long middle, ll, hh;

	low = 0;
	high = n-1;
	median = (low + high) / 2;
	for (;;)
	{
		/* One element only */
		if (high <= low)
			return arr[median];

		/* Two elements only */
		if (high == low + 1)
		{
			if (arr[low] > arr[high])
				SWAP(arr[low], arr[high]);
			return arr[median];
		}

		/* Find median of low, middle and high items; swap to low position */
		middle = (low + high) / 2;
		if (arr[middle] > arr[high])
			SWAP(arr[middle], arr[high]);
		if (arr[low] > arr[high])
			SWAP(arr[low], arr[high]);
		if (arr[middle] > arr[low])
			SWAP(arr[middle], arr[low]);

		/* Swap low item (now in position middle) into position (low+1) */
		SWAP(arr[middle], arr[low+1]);

		/* Work from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;)
		{
			do
				ll++;
			while (arr[low] > arr[ll]);
			do
				hh--;
			while (arr[hh] > arr[low]);

			if (hh < ll)
				break;

			SWAP(arr[ll], arr[hh]);
		}

		/* Swap middle item (in position low) back into correct position */
		SWAP(arr[low], arr[hh]);

		/* Reset active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}

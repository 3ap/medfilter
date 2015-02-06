#include <stdio.h> /* FILE, scanf, printf, freopen, */
#include <stdint.h> /* uint32_t, int32_t, */
#include <stdlib.h> /* exit, EXIT_FAILURE, */
#include <inttypes.h> /* int32_t, uint32_t,  */
#include <limits.h>
#include <sys/mman.h> /* mmap,  */
#define MAX_BUFFER 255

/* Procedure for printing usage information */
void usage(void);

/* Function: find median of array, medium element of sorted array 
   Input:  arr -- pointer to array of int32_t
	   n -- size of array
   Output: median of array 
*/
int32_t medium(int32_t* arr, uint32_t n);

/* Procedure: median filter implementation 
   Input:  wndsz -- window size
	   in, out -- pointers to file streams
*/
void filter(uint32_t wndsz, FILE* in, FILE* out);

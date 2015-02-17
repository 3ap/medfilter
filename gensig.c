#include <stdio.h> /* fprintf, sscanf, */
#include <inttypes.h> /* uint32_t, int32_t, */
#include <stdlib.h> /* srand, rand, exit, */
#include <time.h> /* time, */

int main(int argc, char** argv) {
	srand(time(NULL));
	if(argc < 2) {
		fprintf(stderr, "Usage: %s len\n", argv[0]);
		exit(2);
	}

	uint32_t n;
	sscanf(argv[1], "%"SCNu32, &n);

	const int32_t base = 300;
	int32_t member;
	for(; n != 0; n--) {
		member = rand()%5; // 25% of noise
		if(member == 0) member = rand()%100;
		member += base;
	
		fprintf(stderr, "%"SCNi32" ", member);
		fwrite(&member, sizeof(int32_t), 1, stdout);
	}
	fprintf(stderr, "\n");
}

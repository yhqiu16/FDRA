//#include "convolution2d.h"#define
//typedef int DATA_TYPE;

/* Problem size */
#define NI 32
#define NJ 32
#define SIZE NI*NJ


int A[SIZE];
int B[SIZE];
#define c11 2
#define c12 -3
#define c21 5
#define c22 6

/*For 3x3 filter*/
#define c13 4
#define c23 7
#define c31 -8
#define c32 -9
#define c33 10

__attribute__((noinline))
void convolution2d() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 3x3 filter*/
		B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]  +  c13 * A[(i + 1)*NJ + (j - 1)]
				+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)]  +  c23 * A[(i + 1)*NJ + (j + 0)]
				+ c31 * A[(i - 1)*NJ + (j + 1)]  +  c32 * A[(i + 0)*NJ + (j + 1)]  +  c33 * A[(i + 1)*NJ + (j + 1)];

	// /*For 2x2 filter*/
	// 	B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)] 
	// 			+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)];
		}
	}
}

int main(){


for(int i=0;i<SIZE;i++){
    A[i] = i+1;
  }

convolution2d();
return 0;
}

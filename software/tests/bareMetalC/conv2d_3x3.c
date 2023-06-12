//#include "convolution2d.h"#define
//typedef int DATA_TYPE;

/* Problem size */
#define NI 16
#define NJ 16
#define SIZE NI*NJ


int A[SIZE] __attribute__((aligned(16)));
int B[SIZE] __attribute__((aligned(16)));
int C[SIZE] __attribute__((aligned(16)));
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

#include "include/encoding.h"
#include "include/ISA.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[75][3] __attribute__((aligned(16))) = {
		{0x0501, 0x0700, 0x0004},
		{0x0018, 0x000e, 0x0005},
		{0x0000, 0x0020, 0x0006},
		{0x0611, 0x0700, 0x0008},
		{0x0018, 0x000e, 0x0009},
		{0x0000, 0x0020, 0x000a},
		{0x0420, 0x0700, 0x000c},
		{0x0018, 0x000e, 0x000d},
		{0x0000, 0x0020, 0x000e},
		{0x0400, 0x0700, 0x0014},
		{0x0018, 0x000e, 0x0015},
		{0x0000, 0x0020, 0x0016},
		{0x0510, 0x0700, 0x0018},
		{0x0018, 0x000e, 0x0019},
		{0x0000, 0x0020, 0x001a},
		{0x0100, 0x0000, 0x002c},
		{0x0100, 0x0000, 0x0030},
		{0x0100, 0x0000, 0x0038},
		{0x0100, 0x0000, 0x003c},
		{0x0005, 0x0000, 0x004c},
		{0x1003, 0x0000, 0x004d},
		{0x0006, 0x0000, 0x0050},
		{0x1003, 0x0000, 0x0051},
		{0x0002, 0x0000, 0x0054},
		{0x1005, 0x0000, 0x0055},
		{0xc101, 0x0001, 0x0059},
		{0x0001, 0x0000, 0x005c},
		{0x1005, 0x0000, 0x005d},
		{0xfffd, 0xffff, 0x0060},
		{0x1003, 0x0000, 0x0061},
		{0x0400, 0x0000, 0x0074},
		{0x1800, 0x0100, 0x0078},
		{0x0800, 0x0000, 0x007c},
		{0x0002, 0x0000, 0x0080},
		{0x0100, 0x0001, 0x0084},
		{0xa301, 0x0000, 0x0099},
		{0xa201, 0x0000, 0x009d},
		{0xc401, 0x0000, 0x00a1},
		{0x3701, 0x0002, 0x00a5},
		{0x9001, 0x0000, 0x00a9},
		{0x0100, 0x0040, 0x00bc},
		{0x4000, 0x0050, 0x00c0},
		{0x0200, 0x0000, 0x00c4},
		{0xc001, 0x0200, 0x00c8},
		{0x0004, 0x0001, 0x00cc},
		{0xfff7, 0xffff, 0x00dc},
		{0x4003, 0x0000, 0x00dd},
		{0x1401, 0x0001, 0x00e1},
		{0x0007, 0x0000, 0x00e4},
		{0x4003, 0x0000, 0x00e5},
		{0x1401, 0x0001, 0x00e9},
		{0x000a, 0x0000, 0x00ec},
		{0x3003, 0x0000, 0x00ed},
		{0xfff8, 0xffff, 0x00f0},
		{0x4003, 0x0000, 0x00f1},
		{0x0000, 0x0000, 0x0104},
		{0x0000, 0x0010, 0x010c},
		{0x0320, 0x0000, 0x0110},
		{0x0001, 0x0000, 0x0118},
		{0x0512, 0x0700, 0x0128},
		{0x0018, 0x000e, 0x0129},
		{0x0000, 0x0020, 0x012a},
		{0x0622, 0x0700, 0x012c},
		{0x0018, 0x000e, 0x012d},
		{0x0000, 0x0020, 0x012e},
		{0x0421, 0x0700, 0x0130},
		{0x0018, 0x000e, 0x0131},
		{0x0000, 0x0020, 0x0132},
		{0x0501, 0x0700, 0x0134},
		{0x0018, 0x000e, 0x0135},
		{0x0000, 0x1320, 0x0136},
		{0x0000, 0x0000, 0x0137},
		{0x0402, 0x0700, 0x0138},
		{0x0018, 0x000e, 0x0139},
		{0x0000, 0x0020, 0x013a},
	};

	load_cfg((void*)cin, 0x4000, 450, 0, 0);
	load_data(din_addr[0], 0x1000, 1024, 1, 0, 0);
	load_data(din_addr[1], 0x1400, 1024, 1, 0, 0);
	load_data(din_addr[2], 0x0, 1024, 1, 0, 0);
	load_data(din_addr[3], 0x400, 1024, 1, 0, 0);
	load_data(din_addr[4], 0x800, 1024, 1, 0, 0);
	load_data(din_addr[5], 0x2000, 1024, 1, 0, 0);
	load_data(din_addr[6], 0x3000, 1024, 1, 0, 0);
	load_data(din_addr[7], 0x2400, 1024, 1, 0, 0);
	load_data(din_addr[8], 0x2800, 1024, 0, 0, 0);
	config(0x0, 75, 0, 0);
	execute(0x3e37, 0, 0);
	store(dout_addr[0], 0x3400, 892, 0, 0);
}


/* Array initialization. */
void init_array()
{
    int i,j;
    for (i=0;i<SIZE; i++){
      A[i] = i;
    }

}

__attribute__((noinline))
void convolution2d() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 3x3 filter*/
		C[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]  +  c13 * A[(i + 1)*NJ + (j - 1)]
				+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)]  +  c23 * A[(i + 1)*NJ + (j + 0)]
				+ c31 * A[(i - 1)*NJ + (j + 1)]  +  c32 * A[(i + 0)*NJ + (j + 1)]  +  c33 * A[(i + 1)*NJ + (j + 1)];

	// /*For 2x2 filter*/
	// 	B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)] 
	// 			+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)];
		}
	}
}

void result_check()
{
  int i, j;
  for (int i = 1; i < NI -1; i++) {
  	for (int j = 1; j < NJ-1; j++) {
      int idx = i*NJ + j;
      if (C[idx] != B[idx]) printf("There is an error in location (%d)[%d, %d]\n", idx, C[idx], B[idx]);
    }
  }
}


int main(){
    long long unsigned start;
    long long unsigned end;
    init_array();
    printf("Initialization finished!\n");

    start = rdcycle();
    /* Run kernel. */
    convolution2d();
    end = rdcycle();
    printf("It takes %d cycles for CPU to finish the task.\n", end - start);

    void* cgra_din_addr[9] = {A, A, A, A, A, A, A, A, A};
    void* cgra_dout_addr[1] = {(void*)B+128};
    start = rdcycle();
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

    result_check();
    printf("Done!\n");

    return 0;
}

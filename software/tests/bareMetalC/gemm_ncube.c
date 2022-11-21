#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

//Define compute data type
#define TYPE unsigned int

//Specify row/column sizes
#define row_size 32
#define col_size 32
#define N (row_size*col_size)

TYPE m1[N] __attribute__((aligned(8)));
TYPE m2[N] __attribute__((aligned(8)));
TYPE prod0[N] __attribute__((aligned(8)));
TYPE prod[N] __attribute__((aligned(8)));

__attribute__((noinline))
void gemm(){
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod0[i_col + j]  = sum;
        }
    }
}

#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[57][3] __attribute__((aligned(8))) = {
		{0x0800, 0x2000, 0x000c},
		{0x0040, 0x8100, 0x000d},
		{0x0000, 0x9102, 0x000e},
		{0x0080, 0x0000, 0x000f},
		{0x0060, 0x2010, 0x0010},
		{0x2040, 0x8107, 0x0011},
		{0x0e30, 0x0102, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x9003, 0x2000, 0x0014},
		{0xf900, 0x0107, 0x0015},
		{0x0002, 0x0102, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x0840, 0x2010, 0x0018},
		{0x2040, 0x8107, 0x0019},
		{0x0e30, 0x0102, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x8002, 0x2000, 0x001c},
		{0xf900, 0x0107, 0x001d},
		{0x0002, 0x0102, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x0000, 0x0000, 0x0034},
		{0x0400, 0x0000, 0x0038},
		{0x0100, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x0040},
		{0x100d, 0x0000, 0x0059},
		{0x0000, 0x7001, 0x005a},
		{0x0020, 0x0100, 0x005b},
		{0x2403, 0x0000, 0x005d},
		{0x4403, 0x0000, 0x0061},
		{0x0100, 0x0000, 0x0080},
		{0x4100, 0x0000, 0x0084},
		{0x4421, 0x0000, 0x00a5},
		{0x6411, 0x0000, 0x00a9},
		{0x0040, 0x0000, 0x00cc},
		{0x9001, 0x0000, 0x00ed},
		{0x0001, 0x0000, 0x0114},
		{0x8c03, 0x0000, 0x0135},
		{0x8c03, 0x0000, 0x0139},
		{0x0010, 0x0000, 0x0158},
		{0x0010, 0x0000, 0x015c},
		{0x0000, 0x0000, 0x0160},
		{0x0020, 0x2010, 0x0178},
		{0x2040, 0x8107, 0x0179},
		{0x0e30, 0x0102, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x0800, 0x2010, 0x017c},
		{0x2040, 0x8107, 0x017d},
		{0x0e30, 0x0102, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x9001, 0x2000, 0x0180},
		{0xf900, 0x0107, 0x0181},
		{0x0002, 0x0102, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x8000, 0x2000, 0x0184},
		{0xf900, 0x0107, 0x0185},
		{0x0002, 0x0102, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 342, 0, 0);
	load_data(din_addr[0], 0x18000, 4096, 1, 0, 0);
	load_data(din_addr[1], 0x1c000, 4096, 1, 0, 0);
	load_data(din_addr[2], 0x8000, 4096, 1, 0, 0);
	load_data(din_addr[3], 0xc000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x10000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0xa000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x0, 4096, 0, 0, 0);
	config(0x0, 57, 0, 0);
	execute(0x787c, 0, 0);
	store(dout_addr[0], 0x2000, 4096, 0, 0);
}


int main(){
//#ifndef BAREMETAL
//    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
//      perror("mlockall failed");
//      exit(1);
//    }
//#endif
    long long unsigned start;
    long long unsigned end;

    for (int i=0; i<N; i++){
      m1[i] = i;
      m2[i] = i+1;
    }

    start = rdcycle();
    gemm();
    end = rdcycle();
    printf("It takes %d cycles for CPU to finish the task.\n", end - start);

//    void* cgra_din_addr[2] = {m1, m2};
    void* cgra_din_addr[8] = {m1, m1, m1, m1, m2, m2, m2, m2};
    void* cgra_dout_addr[1] = {prod};
    start = rdcycle();
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    volatile int result = fence(1);
    end = rdcycle();
    printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

//    printf("%x, %x, %x, %x\n", m1, m2, prod0, prod);
//    printf("Execution time (CPU, CGRA): %ul, %ul\n", time0, time1);
    printf("Compare the result:\n");
    for (int i=0; i<N; i++){
        if(prod[i] != prod0[i]){
            printf("%x, %x\n", prod0[i], prod[i]);
        }
    }
    printf("Succeed!(%d)\n", result);

    return 0;
}
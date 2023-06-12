#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

//Define compute data type
#define TYPE unsigned

//Specify row/column sizes
#define row_size 16
#define col_size 16

TYPE m1[row_size][col_size] __attribute__((aligned(16)));
TYPE m2[col_size][row_size] __attribute__((aligned(16)));
TYPE prod[row_size][row_size] __attribute__((aligned(16)));
TYPE prod0[row_size][row_size] __attribute__((aligned(16)));

void gemm(){
    int i, j, k;
    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<row_size;j++) {
            TYPE sum = 0;
//            #pragma unroll 4
            for(k=0;k<col_size;k++) {
                sum += m1[i][k] * m2[k][j];
            }
            prod0[i][j] = sum;
        }
    }
}

#include "include/ISA.h"
#include "include/encoding.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[53][3] __attribute__((aligned(16))) = {
		{0x1002, 0x0200, 0x0004},
		{0xffa0, 0x4010, 0x0005},
		{0x2000, 0x0020, 0x0006},
		{0x0110, 0x0201, 0x000c},
		{0xfa08, 0x1010, 0x000d},
		{0x21f3, 0x0020, 0x000e},
		{0x0220, 0x0201, 0x0010},
		{0xfa08, 0x1010, 0x0011},
		{0x21f3, 0x0020, 0x0012},
		{0x1000, 0x0200, 0x001c},
		{0xffa0, 0x4010, 0x001d},
		{0x2000, 0x0020, 0x001e},
		{0x0000, 0x0000, 0x002c},
		{0x0800, 0x0000, 0x0030},
		{0x0000, 0x0000, 0x0034},
		{0x8000, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x0040},
		{0xa013, 0x0000, 0x0055},
		{0x1013, 0x0002, 0x0059},
		{0x3103, 0x0001, 0x005d},
		{0x0000, 0x0100, 0x007c},
		{0x402c, 0x0000, 0x0080},
		{0x4000, 0x0000, 0x0084},
		{0x1021, 0x0001, 0x00a1},
		{0x1011, 0x0001, 0x00a5},
		{0x9001, 0x0001, 0x00a9},
		{0x0100, 0x0020, 0x00c4},
		{0x0000, 0x0020, 0x00c8},
		{0x0040, 0x0000, 0x00cc},
		{0x100d, 0x0000, 0x00e9},
		{0x0000, 0x0004, 0x00ea},
		{0x0042, 0x0100, 0x00eb},
		{0x3103, 0x0002, 0x00ed},
		{0x0000, 0x0010, 0x0108},
		{0x0000, 0x0020, 0x010c},
		{0x0020, 0x0000, 0x0110},
		{0x0000, 0x0000, 0x0114},
		{0x0130, 0x0201, 0x0128},
		{0xfa08, 0x1010, 0x0129},
		{0x21f3, 0x0020, 0x012a},
		{0x0200, 0x0200, 0x012c},
		{0x0008, 0x1010, 0x012d},
		{0x2000, 0x1260, 0x012e},
		{0x0004, 0x0000, 0x012f},
		{0x0000, 0x0201, 0x0130},
		{0xfa08, 0x1010, 0x0131},
		{0x21f3, 0x0020, 0x0132},
		{0x1001, 0x0200, 0x0134},
		{0xffa0, 0x4010, 0x0135},
		{0x2000, 0x0020, 0x0136},
		{0x1103, 0x0200, 0x0138},
		{0xffa0, 0x4010, 0x0139},
		{0x2000, 0x0020, 0x013a},
	};

	load_cfg((void*)cin, 0x4000, 318, 0, 0);
	load_data(din_addr[0], 0x1000, 1024, 1, 0, 0);
	load_data(din_addr[1], 0x3000, 1024, 1, 0, 0);
	load_data(din_addr[2], 0x0, 1024, 1, 0, 0);
	load_data(din_addr[3], 0x3400, 1024, 0, 0, 0);
	load_data(din_addr[4], 0x2000, 1024, 1, 0, 0);
	load_data(din_addr[5], 0x400, 1024, 1, 0, 0);
	load_data(din_addr[6], 0x800, 1024, 1, 0, 0);
	load_data(din_addr[7], 0x2400, 1024, 0, 0, 0);
	config(0x0, 53, 0, 0);
	execute(0x3e4d, 0, 0);
	store(dout_addr[0], 0x2800, 1024, 0, 0);
}


int main(){
    long long unsigned start;
    long long unsigned end;

    for(int i=0;i<row_size;i++) {
        for(int j=0;j<col_size;j++) {
            m1[i][j] = i+j;
        }
    }
    for(int i=0;i<col_size;i++) {
        for(int j=0;j<row_size;j++) {
            m2[i][j] = i+j;
        }
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
    for(int i=0;i<row_size;i++) {
        for(int j=0;j<row_size;j++) {
            if(prod[i][j] != prod0[i][j]){
                printf("%x, %x\n", prod0[i][j], prod[i][j]);
            }
        }
    }
    printf("Succeed!(%d)\n", result);

    return 0;
}
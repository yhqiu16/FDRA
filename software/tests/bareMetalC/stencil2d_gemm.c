#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
//#include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"

#define TYPE int
#define col_size 32
#define row_size 32
#define f_size 9

TYPE orig[row_size * col_size] __attribute__((aligned(8)));
TYPE sol[row_size * col_size] __attribute__((aligned(8)));
TYPE filter[f_size] __attribute__((aligned(8)));
TYPE res[row_size * col_size] __attribute__((aligned(8)));

//Specify row/column sizes
//#define row_size 32
//#define col_size 32
#define N (row_size*col_size)

TYPE m1[N] __attribute__((aligned(8)));
TYPE m2[N] __attribute__((aligned(8)));
TYPE prod0[N] __attribute__((aligned(8)));
TYPE prod[N] __attribute__((aligned(8)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[105][3] __attribute__((aligned(8))) = {
		{0x1802, 0x0c04, 0x0004},
		{0xf040, 0x80f7, 0x0005},
		{0xefe1, 0x0101, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x7002, 0x0c00, 0x0008},
		{0xfe80, 0x00f7, 0x0009},
		{0xeffd, 0x0101, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0800, 0x0c04, 0x000c},
		{0xf040, 0x80f7, 0x000d},
		{0xefe1, 0x0101, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x6000, 0x0c00, 0x0010},
		{0xfe80, 0x00f7, 0x0011},
		{0xeffd, 0x0101, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x1820, 0x2010, 0x0014},
		{0x2040, 0x8107, 0x0015},
		{0x0e30, 0x0102, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x9001, 0x2000, 0x0018},
		{0xf900, 0x0107, 0x0019},
		{0x0002, 0x0102, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0800, 0x2010, 0x001c},
		{0x2040, 0x8107, 0x001d},
		{0x0e30, 0x0102, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x8000, 0x2000, 0x0020},
		{0xf900, 0x0107, 0x0021},
		{0x0002, 0x0102, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0000, 0x0000, 0x0034},
		{0x0400, 0x0000, 0x003c},
		{0x0400, 0x0000, 0x0044},
		{0x0400, 0x0000, 0x004c},
		{0x4403, 0x0000, 0x005d},
		{0x2403, 0x0000, 0x0069},
		{0x2403, 0x0000, 0x0071},
		{0x2403, 0x0000, 0x0079},
		{0x0000, 0x0000, 0x008c},
		{0x0800, 0x0000, 0x0090},
		{0x0000, 0x0000, 0x0094},
		{0x0000, 0x0000, 0x00a0},
		{0x0000, 0x0000, 0x00a4},
		{0x2801, 0x0000, 0x00bd},
		{0x4401, 0x0000, 0x00cd},
		{0x0000, 0x0000, 0x00ec},
		{0x0000, 0x0000, 0x00f8},
		{0x3081, 0x0000, 0x0119},
		{0x4c11, 0x0000, 0x0121},
		{0x6c21, 0x0000, 0x0125},
		{0x0000, 0x0000, 0x0144},
		{0x0002, 0x0000, 0x0148},
		{0x0010, 0x0003, 0x014c},
		{0x0010, 0x0003, 0x0150},
		{0x0000, 0x0003, 0x0154},
		{0x0000, 0x0001, 0x0158},
		{0x080d, 0x0000, 0x016d},
		{0x0000, 0x7001, 0x016e},
		{0x000c, 0x00e1, 0x016f},
		{0x8c03, 0x0000, 0x0175},
		{0x8c03, 0x0000, 0x0179},
		{0x040d, 0x0000, 0x0181},
		{0x0000, 0x7001, 0x0182},
		{0x0020, 0x0100, 0x0183},
		{0x8c03, 0x0000, 0x0185},
		{0x0100, 0x0000, 0x0198},
		{0x0010, 0x0000, 0x01a0},
		{0x0010, 0x0000, 0x01a4},
		{0x0000, 0x0000, 0x01a8},
		{0x0010, 0x0000, 0x01b0},
		{0x0000, 0x0000, 0x01b4},
		{0x0000, 0x0c00, 0x01c4},
		{0x0040, 0x80f0, 0x01c5},
		{0xe001, 0x9101, 0x01c6},
		{0x0000, 0x0000, 0x01c7},
		{0x1040, 0x2010, 0x01c8},
		{0x2040, 0x8107, 0x01c9},
		{0x0e30, 0x0102, 0x01ca},
		{0x0000, 0x0000, 0x01cb},
		{0x1860, 0x2010, 0x01cc},
		{0x2040, 0x8107, 0x01cd},
		{0x0e30, 0x0102, 0x01ce},
		{0x0000, 0x0000, 0x01cf},
		{0x8802, 0x2000, 0x01d0},
		{0xf900, 0x0107, 0x01d1},
		{0x0002, 0x0102, 0x01d2},
		{0x0000, 0x0000, 0x01d3},
		{0x9003, 0x2000, 0x01d4},
		{0xf900, 0x0107, 0x01d5},
		{0x0002, 0x0102, 0x01d6},
		{0x0000, 0x0000, 0x01d7},
		{0x0801, 0x0c04, 0x01d8},
		{0xf040, 0x80f7, 0x01d9},
		{0xefe1, 0x0101, 0x01da},
		{0x0000, 0x0000, 0x01db},
		{0x1800, 0x2000, 0x01dc},
		{0x0040, 0x8100, 0x01dd},
		{0x0000, 0x9102, 0x01de},
		{0x0000, 0x0000, 0x01df},
		{0x6001, 0x0c00, 0x01e0},
		{0xfe80, 0x00f7, 0x01e1},
		{0xeffd, 0x0101, 0x01e2},
		{0x0000, 0x0000, 0x01e3},
	};

	load_cfg((void*)cin, 0x28000, 630, 0, 0);
	load_data(din_addr[0], 0x8000, 4096, 1, 0, 0);
	load_data(din_addr[1], 0xc000, 4096, 1, 0, 0);
	load_data(din_addr[2], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[3], 0x24000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0xa000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0xe000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x1c000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x1e000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x0, 36, 1, 0, 0);
	load_data(din_addr[9], 0x20000, 36, 1, 0, 0);
	load_data(din_addr[10], 0x4000, 36, 0, 0, 0);
	load_data(din_addr[11], 0x2000, 4096, 1, 0, 0);
	load_data(din_addr[12], 0x22000, 4096, 1, 0, 0);
	load_data(din_addr[13], 0x6000, 4096, 0, 0, 0);
	config(0x0, 105, 0, 0);
	execute(0xff0ff, 0, 0);
	store(dout_addr[0], 0x18000, 3832, 0, 0);
	store(dout_addr[1], 0x26000, 4096, 0, 0);
}


/* Array initialization. */
void init_array()
{
  	int i, j;

  	for (i = 0; i < f_size; i++)
    {
      	filter[i] = i;
    }
	for (i = 0; i < row_size * col_size; i++)
	{
		orig[i] = i;
	}
	for (int i=0; i<N; i++){
      m1[i] = i;
      m2[i] = i+1;
    }
}

__attribute__((noinline))
void stencil2d (){
    int r, c, k1, k2;
    TYPE temp, mul;

    for (r=0; r<row_size-2; r++) {
        for (c=0; c<col_size-2; c++) {
            temp = (TYPE)0;
            for (k1=0;k1<3;k1++){
                for (k2=0;k2<3;k2++){
                    mul = filter[k1*3 + k2] * orig[(r+k1)*col_size + c+k2];
                    temp += mul;
                }
            }
            res[(r*col_size) + c] = temp;
        }
    }
}

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


void result_check()
{
  	int i, j, r, c;

    for (r=0; r<row_size-2; r++) {
        for (c=0; c<col_size-2; c++) {
            i = r*col_size + c;
    		if (res[i] != sol[i]) printf("There is an error in location (%d)[%d, %d]\n", i, res[i], sol[i]);
  		}
	}
	for (int i=0; i<N; i++){
        if(prod[i] != prod0[i]){
            printf("%x, %x\n", prod0[i], prod[i]);
        }
    }
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  stencil2d();
  gemm();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[14] = {m1, m1, m1, m1, m2, m2, m2, m2, filter, filter, filter, orig, orig, orig};
  void* cgra_dout_addr[2] = {sol, prod};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
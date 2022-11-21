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

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[44][3] __attribute__((aligned(8))) = {
		{0x1802, 0x0c04, 0x0014},
		{0xf040, 0x80f7, 0x0015},
		{0xefe1, 0x0101, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x7002, 0x0c00, 0x0018},
		{0xfe80, 0x00f7, 0x0019},
		{0xeffd, 0x0101, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0800, 0x0c04, 0x001c},
		{0xf040, 0x80f7, 0x001d},
		{0xefe1, 0x0101, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x6000, 0x0c00, 0x0020},
		{0xfe80, 0x00f7, 0x0021},
		{0xeffd, 0x0101, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0400, 0x0000, 0x003c},
		{0x0400, 0x0000, 0x0044},
		{0x2403, 0x0000, 0x0061},
		{0x2403, 0x0000, 0x0069},
		{0x0000, 0x0000, 0x0088},
		{0x0000, 0x0000, 0x008c},
		{0x2801, 0x0000, 0x00ad},
		{0x0400, 0x0000, 0x00d0},
		{0x2c11, 0x0000, 0x00f5},
		{0x0110, 0x0000, 0x0118},
		{0x8c03, 0x0000, 0x0139},
		{0x040d, 0x0000, 0x013d},
		{0x0000, 0x6001, 0x013e},
		{0x000c, 0x00e1, 0x013f},
		{0x0010, 0x0000, 0x015c},
		{0x0000, 0x0000, 0x0160},
		{0x0801, 0x0c04, 0x017c},
		{0xf040, 0x80f7, 0x017d},
		{0xefe1, 0x0101, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x1000, 0x0c00, 0x0180},
		{0x0040, 0x80f0, 0x0181},
		{0xe001, 0x8f01, 0x0182},
		{0x0080, 0x0000, 0x0183},
		{0x6001, 0x0c00, 0x0184},
		{0xfe80, 0x00f7, 0x0185},
		{0xeffd, 0x0101, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 264, 0, 0);
	load_data(din_addr[0], 0x8000, 36, 1, 0, 0);
	load_data(din_addr[1], 0x18000, 36, 1, 0, 0);
	load_data(din_addr[2], 0xc000, 36, 0, 0, 0);
	load_data(din_addr[3], 0xa000, 4096, 1, 0, 0);
	load_data(din_addr[4], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0xe000, 4096, 0, 0, 0);
	config(0x0, 44, 0, 0);
	execute(0x70f0, 0, 0);
	store(dout_addr[0], 0x1c000, 3832, 0, 0);
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
            sol[(r*col_size) + c] = temp;
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
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  stencil2d();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[6] = {filter, filter, filter, orig, orig, orig};
  void* cgra_dout_addr[1] = {res};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
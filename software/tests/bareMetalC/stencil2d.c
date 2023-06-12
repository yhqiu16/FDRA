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
#define col_size 16
#define row_size 16
#define f_size 9

TYPE orig[row_size * col_size] __attribute__((aligned(16)));
TYPE sol[row_size * col_size] __attribute__((aligned(16)));
TYPE filter[f_size] __attribute__((aligned(16)));
TYPE res[row_size * col_size] __attribute__((aligned(16)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[41][3] __attribute__((aligned(16))) = {
		{0x0c01, 0x0180, 0x0014},
		{0xffd0, 0xa00e, 0x0015},
		{0x1dff, 0x0020, 0x0016},
		{0x0000, 0x0010, 0x0038},
		{0x0c00, 0x0000, 0x0080},
		{0x3001, 0x0002, 0x00a1},
		{0xc003, 0x0000, 0x00a5},
		{0x0010, 0x0000, 0x00c4},
		{0x0104, 0x0000, 0x00c8},
		{0x0003, 0x0000, 0x00cc},
		{0xc003, 0x0001, 0x00e5},
		{0xc003, 0x0001, 0x00e9},
		{0x9011, 0x0000, 0x00ed},
		{0x300d, 0x0000, 0x00f1},
		{0x0000, 0x8004, 0x00f2},
		{0x0031, 0x00c4, 0x00f3},
		{0x0000, 0x0010, 0x0104},
		{0x0080, 0x0000, 0x0108},
		{0x0040, 0x0000, 0x010c},
		{0x0000, 0x0000, 0x0110},
		{0x0000, 0x0000, 0x0114},
		{0x0000, 0x0000, 0x0118},
		{0x0d00, 0x0180, 0x0124},
		{0xffd0, 0xa00e, 0x0125},
		{0x1dff, 0x0020, 0x0126},
		{0x0e02, 0x0180, 0x012c},
		{0xffd0, 0xa00e, 0x012d},
		{0x1dff, 0x0020, 0x012e},
		{0x4000, 0x0180, 0x0130},
		{0xff08, 0x300e, 0x0131},
		{0x1dfe, 0x0020, 0x0132},
		{0x4102, 0x0180, 0x0134},
		{0xff08, 0x300e, 0x0135},
		{0x1dfe, 0x0020, 0x0136},
		{0x4001, 0x0180, 0x0138},
		{0xff08, 0x300e, 0x0139},
		{0x1dfe, 0x0020, 0x013a},
		{0x0200, 0x0180, 0x013c},
		{0x0008, 0x300e, 0x013d},
		{0x1c00, 0x11e0, 0x013e},
		{0x0000, 0x0000, 0x013f},
	};

	load_cfg((void*)cin, 0x4000, 246, 0, 0);
	load_data(din_addr[0], 0x2400, 36, 1, 0, 0);
	load_data(din_addr[1], 0x1000, 36, 1, 0, 0);
	load_data(din_addr[2], 0x2800, 36, 0, 0, 0);
	load_data(din_addr[3], 0x2000, 1024, 1, 0, 0);
	load_data(din_addr[4], 0x3000, 1024, 1, 0, 0);
	load_data(din_addr[5], 0x3400, 1024, 0, 0, 0);
	config(0x0, 41, 0, 0);
	execute(0x7d10, 0, 0);
	store(dout_addr[0], 0x3800, 888, 0, 0);
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
//    TYPE filter[f_size] = {
//            025, 150, 375, -225, 050, 075, -300, 125, 025};

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
  void* cgra_dout_addr[1] = {sol};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
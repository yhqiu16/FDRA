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

#define NTAPS 32

int input[NTAPS];
int output[NTAPS];
int res[NTAPS];
int coefficients[NTAPS] = {025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125};


void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[63][3] __attribute__((aligned(8))) = {
		{0x8000, 0x2000, 0x000c},
		{0xf900, 0x0107, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x8800, 0x2000, 0x0010},
		{0xf900, 0x0107, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x8801, 0x2000, 0x0014},
		{0xf900, 0x0107, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x8001, 0x2000, 0x0018},
		{0xf900, 0x0107, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x9803, 0x2000, 0x001c},
		{0xf900, 0x0107, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x9003, 0x2000, 0x0020},
		{0xf900, 0x0107, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0000, 0x0000, 0x0034},
		{0x0800, 0x0000, 0x0038},
		{0x0400, 0x0000, 0x003c},
		{0x0400, 0x0000, 0x0044},
		{0x2413, 0x0000, 0x005d},
		{0x2403, 0x0000, 0x0061},
		{0x2403, 0x0000, 0x0069},
		{0x0100, 0x0000, 0x0084},
		{0x0000, 0x0000, 0x008c},
		{0x2411, 0x0000, 0x00a9},
		{0x6831, 0x0000, 0x00ad},
		{0x0400, 0x0000, 0x00cc},
		{0x0140, 0x0000, 0x00d0},
		{0x3021, 0x0000, 0x00f1},
		{0x040d, 0x0000, 0x00f5},
		{0x0000, 0x8001, 0x00f6},
		{0x0020, 0x0008, 0x00f7},
		{0x0000, 0x0000, 0x0118},
		{0x4c41, 0x0000, 0x0139},
		{0x8c03, 0x0000, 0x013d},
		{0x0010, 0x0000, 0x015c},
		{0x0010, 0x0000, 0x0160},
		{0x0000, 0x0000, 0x0164},
		{0x1000, 0x2000, 0x017c},
		{0x0040, 0x0100, 0x017d},
		{0x0000, 0x0500, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x8802, 0x2000, 0x0180},
		{0xf900, 0x0107, 0x0181},
		{0x0000, 0x0100, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x1800, 0x2000, 0x0184},
		{0x0040, 0x0100, 0x0185},
		{0x0000, 0x9500, 0x0186},
		{0x0000, 0x0000, 0x0187},
		{0x8002, 0x2000, 0x0188},
		{0xf900, 0x0107, 0x0189},
		{0x0000, 0x0100, 0x018a},
		{0x0000, 0x0000, 0x018b},
	};

	load_cfg((void*)cin, 0x20000, 378, 0, 0);
	load_data(din_addr[0], 0x2000, 128, 1, 0, 0);
	load_data(din_addr[1], 0xa000, 128, 1, 0, 0);
	load_data(din_addr[2], 0x1a000, 128, 1, 0, 0);
	load_data(din_addr[3], 0xe000, 128, 0, 0, 0);
	load_data(din_addr[4], 0x0, 128, 1, 0, 0);
	load_data(din_addr[5], 0x8000, 128, 1, 0, 0);
	load_data(din_addr[6], 0x18000, 128, 1, 0, 0);
	load_data(din_addr[7], 0xc000, 128, 0, 0, 0);
	load_data(din_addr[8], 0x1c000, 128, 0, 0, 0);
	config(0x0, 63, 0, 0);
	execute(0xf0fc, 0, 0);
	store(dout_addr[0], 0x1e000, 128, 0, 0);
}



/* Array initialization. */
void init_array()
{
  int i, j;

  for (i = 0; i < NTAPS; i++)
    {
      input[i] = i;
      output[i] = 0;
      res[i] = 0;
    }
}


__attribute__((noinline))
void fir()//(int input[], int output[], int coefficients[])
/*   input :           input sample array */
/*   output:           output sample array */
/*   coefficient:      coefficient array */
{
  int i;
  int j = 0;

 for(j=0; j< NTAPS; ++j) {
 	  int sum = 0;
//    #pragma unroll 4
    for (i = 0; i < NTAPS; ++i) {
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      sum += input[i] * coefficients[i];
    }
    res[j] += sum;
 }
}


void result_check()
{
  int i, j;

  for (i = 0; i < NTAPS; i++)
  {
    if (res[i] != output[i]) printf("There is an error in location (%d)[%d, %d]\n", i, res[i], output[i]);
  }
}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  init_array();
  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernel. */
  fir();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[9] = {coefficients, coefficients, coefficients, coefficients, input, input, input, input, output};
//  void* cgra_din_addr[3] = {coefficients, input, output};
  void* cgra_dout_addr[1] = {output};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
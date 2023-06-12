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

#define NDATA 256
#define NTAPS 8

int input[NDATA] __attribute__((aligned(16)));
int output[NDATA] __attribute__((aligned(16)));
int res[NDATA] __attribute__((aligned(16)));
int coefficients[NTAPS] __attribute__((aligned(16))) = {
    025, 150, 375, -225, 050, 075, -300, 125};


void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[52][3] __attribute__((aligned(16))) = {
		{0xf104, 0x017f, 0x0008},
		{0x0028, 0x00f9, 0x0009},
		{0x0000, 0x0020, 0x000a},
		{0xf007, 0x017f, 0x000c},
		{0x0028, 0x00f9, 0x000d},
		{0x0000, 0x0020, 0x000e},
		{0x1103, 0x0100, 0x0014},
		{0xffe0, 0x00f9, 0x0015},
		{0x0000, 0x0020, 0x0016},
		{0xf005, 0x017f, 0x0018},
		{0x0028, 0x00f9, 0x0019},
		{0x0000, 0x0020, 0x001a},
		{0x0000, 0x0010, 0x0030},
		{0x4800, 0x0000, 0x0034},
		{0x0000, 0x0000, 0x0038},
		{0x0000, 0x0010, 0x003c},
		{0x4001, 0x0001, 0x0055},
		{0xa003, 0x0000, 0x0059},
		{0x0c00, 0x0000, 0x0078},
		{0x0001, 0x0000, 0x007c},
		{0x0800, 0x0000, 0x0080},
		{0x0000, 0x0600, 0x0084},
		{0xc003, 0x0000, 0x009d},
		{0x400d, 0x0000, 0x00a1},
		{0x0000, 0xc004, 0x00a2},
		{0x0021, 0x00f9, 0x00a3},
		{0xc011, 0x0000, 0x00a5},
		{0x0003, 0x0000, 0x00c4},
		{0x1001, 0x0000, 0x00c8},
		{0x8000, 0x0000, 0x00cc},
		{0x2011, 0x0001, 0x00e9},
		{0x3013, 0x0001, 0x00ed},
		{0x4003, 0x0002, 0x00f1},
		{0x0000, 0x0010, 0x010c},
		{0x0320, 0x0000, 0x0110},
		{0x0001, 0x0000, 0x0118},
		{0x1102, 0x0100, 0x012c},
		{0xffe0, 0x00f9, 0x012d},
		{0x0000, 0x0020, 0x012e},
		{0x1000, 0x0100, 0x0130},
		{0xffe0, 0x00f9, 0x0131},
		{0x0000, 0x0020, 0x0132},
		{0x0203, 0x0100, 0x0134},
		{0x0008, 0x00f9, 0x0135},
		{0x0000, 0x1220, 0x0136},
		{0x0000, 0x0000, 0x0137},
		{0x1101, 0x0100, 0x0138},
		{0xffe0, 0x00f9, 0x0139},
		{0x0000, 0x0020, 0x013a},
		{0xf006, 0x017f, 0x013c},
		{0x0028, 0x00f9, 0x013d},
		{0x0000, 0x0020, 0x013e},
	};

	load_cfg((void*)cin, 0x4000, 312, 0, 0);
	load_data(din_addr[0], 0x2000, 32, 1, 0, 0);
	load_data(din_addr[1], 0x3400, 32, 1, 0, 0);
	load_data(din_addr[2], 0x2400, 32, 1, 0, 0);
	load_data(din_addr[3], 0x1400, 32, 0, 0, 0);
	load_data(din_addr[4], 0x0, 1024, 1, 0, 0);
	load_data(din_addr[5], 0x3000, 1024, 1, 0, 0);
	load_data(din_addr[6], 0x1000, 1024, 1, 0, 0);
	load_data(din_addr[7], 0x400, 1024, 0, 0, 0);
	config(0x0, 52, 0, 0);
	execute(0x7c36, 0, 0);
	store(dout_addr[0], 0x3800, 1008, 0, 0);
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


__attribute__((noinline)) void fir() //(int input[], int output[], int coefficients[])
/*   input :           input sample array */
/*   output:           output sample array */
/*   coefficient:      coefficient array */
{
  int i, j;
//  int coefficients[NTAPS] = {
//    025, 150, 375, -225, 050, 075, -300, 125};
  for (j = NTAPS-1; j < NDATA; ++j)
  {
#ifdef CGRA_COMPILER
       please_map_me();
 #endif
    int sum = 0;
    #pragma unroll
    for (i = 0; i < NTAPS; ++i)
    {
      sum += input[j - i] * coefficients[i];
    }
    res[j] = sum;
  }
}


void result_check()
{
  int i, j;

  for (i = NTAPS-1; i < NDATA; ++i)
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

  void* cgra_din_addr[8] = {coefficients, coefficients, coefficients, coefficients, input, input, input, input};
  void* cgra_dout_addr[1] = {(void*)output+16};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
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

#define SIZE 256

int acc_vector[SIZE] __attribute__((aligned(16)));
int res_vector[SIZE] __attribute__((aligned(16)));

int acceleration_x[SIZE] __attribute__((aligned(16)));
int acceleration_y[SIZE] __attribute__((aligned(16)));
int acceleration_z[SIZE] __attribute__((aligned(16)));
int acc_avg_x[SIZE] __attribute__((aligned(16)));
int acc_avg_y[SIZE] __attribute__((aligned(16)));
int acc_avg_z[SIZE] __attribute__((aligned(16)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[41][3] __attribute__((aligned(16))) = {
		{0x0400, 0x8000, 0x000c},
		{0x0000, 0x0000, 0x000d},
		{0x0000, 0x0020, 0x000e},
		{0x0500, 0x8000, 0x0010},
		{0x0000, 0x0000, 0x0011},
		{0x0000, 0x0020, 0x0012},
		{0x0400, 0x8000, 0x0018},
		{0x0000, 0x0000, 0x0019},
		{0x0000, 0x11e0, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0000, 0x0010, 0x0030},
		{0x0000, 0x0010, 0x0034},
		{0x0010, 0x0000, 0x003c},
		{0xb003, 0x0001, 0x0055},
		{0x4003, 0x0002, 0x0059},
		{0xb001, 0x0001, 0x005d},
		{0x2150, 0x0000, 0x0078},
		{0x0300, 0x0040, 0x007c},
		{0x0025, 0x0000, 0x0080},
		{0xa002, 0x0001, 0x0099},
		{0x1001, 0x0002, 0x009d},
		{0x9102, 0x0001, 0x00a1},
		{0x00c0, 0x0000, 0x00bc},
		{0x00c4, 0x0000, 0x00c4},
		{0xc002, 0x0001, 0x00e1},
		{0xb003, 0x0001, 0x00e5},
		{0x0040, 0x0000, 0x0104},
		{0x0000, 0x0000, 0x0108},
		{0x0000, 0x0000, 0x010c},
		{0x0400, 0x8000, 0x0124},
		{0x0000, 0x0000, 0x0125},
		{0x0000, 0x0020, 0x0126},
		{0x0500, 0x8000, 0x0128},
		{0x0000, 0x0000, 0x0129},
		{0x0000, 0x0020, 0x012a},
		{0x0700, 0x8000, 0x012c},
		{0x0000, 0x0000, 0x012d},
		{0x0000, 0x0020, 0x012e},
		{0x0600, 0x8000, 0x0130},
		{0x0000, 0x0000, 0x0131},
		{0x0000, 0x0020, 0x0132},
	};

	load_cfg((void*)cin, 0x4000, 246, 0, 0);
	load_data(din_addr[0], 0x2000, 1024, 0, 0, 0);
	load_data(din_addr[1], 0x2400, 1024, 0, 0, 0);
	load_data(din_addr[2], 0x2800, 1024, 0, 0, 0);
	load_data(din_addr[3], 0x2c00, 1024, 0, 0, 0);
	load_data(din_addr[4], 0x0, 1024, 0, 0, 0);
	load_data(din_addr[5], 0x400, 1024, 0, 0, 0);
	config(0x0, 41, 0, 0);
	execute(0xf2c, 0, 0);
	store(dout_addr[0], 0x1000, 1024, 0, 0);
}


/* Array initialization. */
static
void init_array()
{
	for(int i=0;i<SIZE;i++){
    	acceleration_x[i] = 3*i;
    	acceleration_y[i] = 3*i+1;
    	acceleration_z[i] = 3*i+2;
    
    	acc_avg_x[i] = i;
    	acc_avg_y[i] = i;
    	acc_avg_z[i] = i;
 	}
}


__attribute__((noinline))
void pedometer(){
	for(int i=0;i<SIZE;i++){
		acc_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}

static
void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    if (acc_vector[i] != res_vector[i]) printf("There is an error in location (%d)[%d, %d]\n", i, acc_vector[i], res_vector[i]);
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
  pedometer();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[6] = {acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
  void* cgra_dout_addr[1] = {res_vector};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
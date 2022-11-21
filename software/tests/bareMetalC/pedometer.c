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

#define SIZE 1024

//typedef struct Acceleration Acceleration;
//struct Acceleration {
//    int x;
//    int y;
//    int z;
//};

//unsigned char second = 0;
//unsigned char minute = 0;
//unsigned char hour = 0;
//unsigned char state = 0;
//unsigned char I1_flag = 0;
//unsigned char I2_flag = 0;
//unsigned char timerFlag1 = 0;
//unsigned char timerFlag2 = 0;
//unsigned char aux=0;
//char Int_SourceSystem =0;
//char Int_SourceTrans=0;
//unsigned char length;
//char buffer[14];
//int step[SIZE]={};
//int km[SIZE]={};
int acc_vector[SIZE];
int res_vector[SIZE];

//int i;
//int sub_x;
//int sub_y;
//int sub_z;
int acceleration_x[SIZE];
int acceleration_y[SIZE];
int acceleration_z[SIZE];
int acc_avg_x[SIZE];
int acc_avg_y[SIZE];
int acc_avg_z[SIZE];

//long long unsigned start;
//long long unsigned end;
//int result;

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[45][3] __attribute__((aligned(8))) = {
		{0x2000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x3000, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0400, 0x0000, 0x0038},
		{0x4000, 0x0000, 0x0040},
		{0x5400, 0x0000, 0x0044},
		{0x2402, 0x0000, 0x005d},
		{0x5001, 0x0000, 0x0061},
		{0x4803, 0x0000, 0x0065},
		{0x2402, 0x0000, 0x0069},
		{0x0000, 0x0200, 0x0084},
		{0x0001, 0x0000, 0x0088},
		{0x2403, 0x0000, 0x00a9},
		{0x0300, 0x0000, 0x00cc},
		{0x6481, 0x0000, 0x00f1},
		{0x5040, 0x0200, 0x0114},
		{0x4803, 0x0000, 0x0135},
		{0x8c02, 0x0000, 0x0139},
		{0x0310, 0x0000, 0x015c},
		{0x0000, 0x0000, 0x0160},
		{0x2000, 0x0000, 0x017c},
		{0x0010, 0x0000, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x3000, 0x0000, 0x0180},
		{0x0010, 0x0000, 0x0181},
		{0x0000, 0x8f00, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x2800, 0x0000, 0x0184},
		{0x0010, 0x0000, 0x0185},
		{0x0000, 0x0100, 0x0186},
		{0x0000, 0x0000, 0x0187},
	};

	load_cfg((void*)cin, 0x20000, 270, 0, 0);
	load_data(din_addr[0], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0xc000, 4096, 0, 0, 0);
	load_data(din_addr[2], 0x1a000, 4096, 0, 0, 0);
	load_data(din_addr[3], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[5], 0x18000, 4096, 0, 0, 0);
	config(0x0, 45, 0, 0);
	execute(0x70d8, 0, 0);
	store(dout_addr[0], 0x1c000, 4096, 0, 0);
}


/* Array initialization. */
static
void init_array()
{
	for(int i=0;i<SIZE;i++){    
//    	sub_x = 0;
//    	sub_y = 0;
//    	sub_z = 0;

    	acceleration_x[i] = 3*i;
    	acceleration_y[i] = 3*i+1;
    	acceleration_z[i] = 3*i+2;
    
    	acc_avg_x[i] = i;
    	acc_avg_y[i] = i;
    	acc_avg_z[i] = i;
 	}
}


__attribute__((noinline))
void pedometer(){//(int input[], int output[], int coefficients[])
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
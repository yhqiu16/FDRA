#include <string.h>
#include <stdio.h>
#include "include/encoding.h"
#include "include/ISA.h"
void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[37][3] __attribute__((aligned(16))) = {
		{0x60f0, 0x1800, 0x000c},
		{0xf840, 0x8087, 0x000d},
		{0x4070, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0000, 0x0010, 0x0030},
		{0x6c03, 0x0000, 0x0055},
		{0x0150, 0x0600, 0x0078},
		{0xffff, 0xffff, 0x0098},
		{0x000d, 0x000f, 0x0099},
		{0x0000, 0x0006, 0x009a},
		{0x0040, 0x0401, 0x009b},
		{0x0004, 0x0000, 0x009c},
		{0x0403, 0x0000, 0x009d},
		{0x7000, 0x0000, 0x00bc},
		{0x2000, 0x0201, 0x00c0},
		{0x4831, 0x0000, 0x00dd},
		{0x8823, 0x0000, 0x00e1},
		{0x9001, 0x0000, 0x00e5},
		{0x0000, 0x0000, 0x0100},
		{0x2004, 0x0000, 0x0108},
		{0x0004, 0x0000, 0x010c},
		{0x78f0, 0x1800, 0x0124},
		{0xf840, 0x8087, 0x0125},
		{0x4170, 0x9100, 0x0126},
		{0x0000, 0x0000, 0x0127},
		{0x1000, 0x0000, 0x0128},
		{0x0006, 0x0000, 0x0129},
		{0x0000, 0x0700, 0x012a},
		{0x0081, 0x0000, 0x012b},
		{0x0000, 0x1800, 0x012c},
		{0x0000, 0x0080, 0x012d},
		{0x4002, 0x0100, 0x012e},
		{0x0000, 0x0000, 0x012f},
		{0x680f, 0x1800, 0x0130},
		{0xfc00, 0x0087, 0x0131},
		{0x4000, 0x0100, 0x0132},
		{0x0000, 0x0000, 0x0133},
	};

	load_cfg((void*)cin, 0x20000, 222, 0, 0);
	load_data(din_addr[0], 0x10000, 52, 0, 0, 0);
	load_data(din_addr[1], 0x12000, 124, 0, 0, 0);
	load_data(din_addr[2], 0x14000, 1024, 0, 0, 0);
	load_data(din_addr[3], 0x0, 1024, 0, 0, 0);
	config(0x0, 37, 0, 0);
	execute(0xf04, 0, 0);
	store(dout_addr[0], 0x16000, 7168, 0, 0);
}

#define NI 10
#define NJ 16
#define NK 16
#define ASIZE (2*NI)
#define BSIZE (NJ+NK)
#define CSIZE (NJ*NJ)
int A[ASIZE] __attribute__((aligned(16)));
int B[BSIZE] __attribute__((aligned(16)));
int C[CSIZE] __attribute__((aligned(16)));
int D[NI][NJ][NK] __attribute__((aligned(16)));
int D1[NI][NJ][NK] __attribute__((aligned(16)));
int E[NJ][NK] __attribute__((aligned(16)));

void test(){
  for (int i=2; i<NI; i=i+2){
    for (int j=NJ-1; j>=0; j--){
   	  for(int k=0; k<NK; k=k+3){
   		  int tmp = A[i*2] + B[j+k];
   		  D1[i][j][k] = C[j*j] * E[j][k] + tmp;
   		}
   	}
  }
}


int main(){
  long long unsigned start;
  long long unsigned end;
  for (int i=0; i<ASIZE; i++){
    A[i] = i * 2 + 5;
  }
  for (int i=0; i<BSIZE; i++){
    B[i] = i * 3;
  }
  for (int i=0; i<CSIZE; i++){
    C[i] = i;
  }
  for (int j=0; j<NJ; j++){
    for(int k=0; k<NK; k++)
      E[j][k] = k+j;
  }
  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernel. */
  test();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[4] = {(void*)A+16, B, C, E};
  void* cgra_dout_addr[1] = {(void*)D+2048};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  for (int i=2; i<NI; i=i+2)
    for (int j=NJ-1; j>=0; j--)
      for(int k=0; k<NK; k=k+3)
        if(D[i][j][k] != D1[i][j][k]) printf("(%d, %d, %d): %d, %d\n", i, j, k, D[i][j][k], D1[i][j][k]);

  printf("Done!\n");
  return 0;

}

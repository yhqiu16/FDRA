#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"

#define TYPE int
#define N 1024
TYPE XR0[N] __attribute__((aligned(16)));
TYPE XR1[N] __attribute__((aligned(16)));
TYPE XR2[N] __attribute__((aligned(16)));
TYPE XR3[N] __attribute__((aligned(16)));
TYPE XI0[N] __attribute__((aligned(16)));
TYPE XI1[N] __attribute__((aligned(16)));
TYPE XI2[N] __attribute__((aligned(16)));
TYPE XI3[N] __attribute__((aligned(16)));
TYPE YR0[N] __attribute__((aligned(16)));
TYPE YR1[N] __attribute__((aligned(16)));
TYPE YR2[N] __attribute__((aligned(16)));
TYPE YR3[N] __attribute__((aligned(16)));
TYPE YI0[N] __attribute__((aligned(16)));
TYPE YI1[N] __attribute__((aligned(16)));
TYPE YI2[N] __attribute__((aligned(16)));
TYPE YI3[N] __attribute__((aligned(16)));
TYPE ZR0[N] __attribute__((aligned(16)));
TYPE ZR1[N] __attribute__((aligned(16)));
TYPE ZR2[N] __attribute__((aligned(16)));
TYPE ZR3[N] __attribute__((aligned(16)));
TYPE ZI0[N] __attribute__((aligned(16)));
TYPE ZI1[N] __attribute__((aligned(16)));
TYPE ZI2[N] __attribute__((aligned(16)));
TYPE ZI3[N] __attribute__((aligned(16)));


void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[113][3] __attribute__((aligned(16))) = {
		{0x7000, 0x0000, 0x0004},
		{0x0020, 0x0000, 0x0005},
		{0x0000, 0x1a00, 0x0006},
		{0x0041, 0x0000, 0x0007},
		{0x5000, 0x0000, 0x0008},
		{0x0020, 0x0000, 0x0009},
		{0x0000, 0x0200, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x4000, 0x0000, 0x000c},
		{0x0020, 0x0000, 0x000d},
		{0x0000, 0x0200, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x6000, 0x0000, 0x0010},
		{0x0020, 0x0000, 0x0011},
		{0x0000, 0x1e00, 0x0012},
		{0x0041, 0x0000, 0x0013},
		{0x7000, 0x0000, 0x0014},
		{0x0020, 0x0000, 0x0015},
		{0x0000, 0x1a00, 0x0016},
		{0x0041, 0x0000, 0x0017},
		{0x4000, 0x0000, 0x0018},
		{0x0020, 0x0000, 0x0019},
		{0x0000, 0x0200, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x5000, 0x0000, 0x001c},
		{0x0020, 0x0000, 0x001d},
		{0x0000, 0x0200, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x6000, 0x0000, 0x0020},
		{0x0020, 0x0000, 0x0021},
		{0x0000, 0x0200, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x5000, 0x0000, 0x0024},
		{0x0020, 0x0000, 0x0025},
		{0x0000, 0x1e00, 0x0026},
		{0x0001, 0x0000, 0x0027},
		{0x0000, 0x0010, 0x0030},
		{0x0000, 0x0010, 0x0034},
		{0x0003, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x0040},
		{0x1000, 0x0010, 0x0044},
		{0x0600, 0x000c, 0x0048},
		{0x0020, 0x0000, 0x004c},
		{0x9001, 0x0000, 0x0059},
		{0x8812, 0x0000, 0x0069},
		{0x2801, 0x0000, 0x006d},
		{0x2402, 0x0000, 0x0071},
		{0x0300, 0x0600, 0x0080},
		{0x0c04, 0x0600, 0x0084},
		{0x0000, 0x0000, 0x008c},
		{0xc000, 0x0000, 0x0090},
		{0x8404, 0x0603, 0x0094},
		{0x0000, 0x0208, 0x0098},
		{0x6401, 0x0000, 0x00a9},
		{0x3081, 0x0000, 0x00ad},
		{0x4c01, 0x0000, 0x00b5},
		{0x4c81, 0x0000, 0x00b9},
		{0x2c81, 0x0000, 0x00bd},
		{0x01c0, 0x0600, 0x00d0},
		{0x0000, 0x00c0, 0x00d4},
		{0x0803, 0x0000, 0x00d8},
		{0x0010, 0x0040, 0x00dc},
		{0x0030, 0x0080, 0x00e0},
		{0x0800, 0x0600, 0x00e4},
		{0x2000, 0x0000, 0x00e8},
		{0x4402, 0x0000, 0x00f9},
		{0x2c02, 0x0000, 0x0101},
		{0x8c82, 0x0000, 0x0105},
		{0x2822, 0x0000, 0x010d},
		{0x0000, 0x02e0, 0x0120},
		{0x0800, 0x0000, 0x0124},
		{0x0030, 0x0060, 0x0128},
		{0x0820, 0x0000, 0x012c},
		{0x1000, 0x0020, 0x0130},
		{0x2000, 0x0000, 0x0134},
		{0x0000, 0x0000, 0x0138},
		{0x2c22, 0x0000, 0x014d},
		{0x2811, 0x0000, 0x0155},
		{0x6882, 0x0000, 0x0159},
		{0x2000, 0x0000, 0x0170},
		{0x0010, 0x0000, 0x0174},
		{0x0000, 0x0000, 0x0178},
		{0x0040, 0x0034, 0x0180},
		{0x0200, 0x0000, 0x0184},
		{0x0300, 0x0000, 0x0188},
		{0x6000, 0x0000, 0x0194},
		{0x0020, 0x0000, 0x0195},
		{0x0000, 0x1a00, 0x0196},
		{0x0041, 0x0000, 0x0197},
		{0x4000, 0x0000, 0x0198},
		{0x0020, 0x0000, 0x0199},
		{0x0000, 0x0200, 0x019a},
		{0x0000, 0x0000, 0x019b},
		{0x5000, 0x0000, 0x01a0},
		{0x0020, 0x0000, 0x01a1},
		{0x0000, 0x0200, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x4000, 0x0000, 0x01a4},
		{0x0020, 0x0000, 0x01a5},
		{0x0000, 0x0200, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x6000, 0x0000, 0x01a8},
		{0x0020, 0x0000, 0x01a9},
		{0x0000, 0x1e00, 0x01aa},
		{0x0001, 0x0000, 0x01ab},
		{0x7000, 0x0000, 0x01ac},
		{0x0020, 0x0000, 0x01ad},
		{0x0000, 0x2200, 0x01ae},
		{0x0001, 0x0000, 0x01af},
		{0x4000, 0x0000, 0x01b0},
		{0x0020, 0x0000, 0x01b1},
		{0x0000, 0x1e00, 0x01b2},
		{0x0001, 0x0000, 0x01b3},
	};

	load_cfg((void*)cin, 0x48000, 678, 0, 0);
	load_data(din_addr[0], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x14000, 4096, 0, 0, 0);
	load_data(din_addr[2], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[3], 0x30000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0x20000, 4096, 0, 0, 0);
	load_data(din_addr[5], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[6], 0x4000, 4096, 0, 0, 0);
	load_data(din_addr[7], 0x34000, 4096, 0, 0, 0);
	config(0x0, 113, 0, 0);
	execute(0x1f7ff, 0, 0);
	store(dout_addr[0], 0xc000, 4096, 0, 0);
	store(dout_addr[1], 0x24000, 4096, 0, 0);
	store(dout_addr[2], 0x28000, 4096, 0, 0);
	store(dout_addr[3], 0x1c000, 4096, 0, 0);
	store(dout_addr[4], 0x38000, 4096, 0, 0);
	store(dout_addr[5], 0x8000, 4096, 0, 0);
	store(dout_addr[6], 0x3c000, 4096, 0, 0);
	store(dout_addr[7], 0x40000, 4096, 0, 0);
}


#define cmplx_M_x(a_x, a_y, b_x, b_y) (a_x*b_x - a_y *b_y)
#define cmplx_M_y(a_x, a_y, b_x, b_y) (a_x*b_y + a_y *b_x)
#define cmplx_MUL_x(a_x, a_y, b_x, b_y ) (a_x*b_x - a_y*b_y)
#define cmplx_MUL_y(a_x, a_y, b_x, b_y ) (a_x*b_y + a_y*b_x)
#define cmplx_mul_x(a_x, a_y, b_x, b_y) (a_x*b_x - a_y*b_y)
#define cmplx_mul_y(a_x, a_y, b_x, b_y) (a_x*b_y + a_y*b_x)
#define cmplx_add_x(a_x, b_x) (a_x + b_x)
#define cmplx_add_y(a_y, b_y) (a_y + b_y)
#define cmplx_sub_x(a_x, b_x) (a_x - b_x)
#define cmplx_sub_y(a_y, b_y) (a_y - b_y)
#define cm_fl_mul_x(a_x, b) (b*a_x)
#define cm_fl_mul_y(a_y, b) (b*a_y)

#define FF2(a0_x, a0_y, a1_x, a1_y){			\
    TYPE c0_x = a0_x;		\
    TYPE c0_y = a0_y;		\
    a0_x = cmplx_add_x(c0_x, a1_x);	\
    a0_y = cmplx_add_y(c0_y, a1_y);	\
    a1_x = cmplx_sub_x(c0_x, a1_x);	\
    a1_y = cmplx_sub_y(c0_y, a1_y);	\
}

#define FFT4(a0_x, a0_y, a1_x, a1_y, a2_x, a2_y, a3_x, a3_y){           \
    TYPE exp_1_44_x;		\
    TYPE exp_1_44_y;		\
    TYPE tmp;			\
    exp_1_44_x =  0;		\
    exp_1_44_y =  -1;		\
    FF2( a0_x, a0_y, a2_x, a2_y);   \
    FF2( a1_x, a1_y, a3_x, a3_y);   \
    tmp = a3_x;			\
    a3_x = a3_x*exp_1_44_x-a3_y*exp_1_44_y;     	\
    a3_y = tmp*exp_1_44_y - a3_y*exp_1_44_x;    	\
    FF2( a0_x, a0_y, a1_x, a1_y );                  \
    FF2( a2_x, a2_y, a3_x, a3_y );                  \
}

/* Array initialization. */
static
void init_array()
{
    int i;
    for (i = 0; i < N; i++) {
        XR0[i] = i;
        XR1[i] = i+1;
        XR2[i] = i+2;
        XR3[i] = i+3;
        XI0[i] = i+4;
        XI1[i] = i+5;
        XI2[i] = i+6;
        XI3[i] = i+7;
    }    
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void fft4(){
    int i;
    for(i = 0; i < N; i++){
//    please_map_me();
        TYPE a0_x = XR0[i];
        TYPE a0_y = XI0[i];
        TYPE a1_x = XR1[i];
        TYPE a1_y = XI1[i];
        TYPE a2_x = XR2[i];
        TYPE a2_y = XI2[i];
        TYPE a3_x = XR3[i];
        TYPE a3_y = XI3[i];
        FFT4(a0_x, a0_y, a1_x, a1_y, a2_x, a2_y, a3_x, a3_y);
        ZR0[i] = a0_x;
        ZI0[i] = a0_y;
        ZR1[i] = a1_x;
        ZI1[i] = a1_y;
        ZR2[i] = a2_x;
        ZI2[i] = a2_y;
        ZR3[i] = a3_x;
        ZI3[i] = a3_y;
    }
}


static
void result_check()
{
  int i, j;

  for (i = 0; i < N; i++)
  {
    if (ZR0[i] != YR0[i] ||
        ZI0[i] != YI0[i] ||
        ZR1[i] != YR1[i] ||
        ZI1[i] != YI1[i] ||
        ZR2[i] != YR2[i] ||
        ZI2[i] != YI2[i] ||
        ZR3[i] != YR3[i] ||
        ZI3[i] != YI3[i])
        printf("There is an error in location (%d)[%d, %d]\n", i, ZR0[i], YR0[i]);
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
  fft4();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[8] = {XI0, XI1, XI2, XI3, XR0, XR1, XR2, XR3};
  void* cgra_dout_addr[8] = {YR0, YI0, YR1, YI1, YR2, YI2, YR3, YI3};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
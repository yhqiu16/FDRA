#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"

#define TYPE int
#define N 256
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
	static unsigned short cin[98][3] __attribute__((aligned(16))) = {
		{0x0700, 0x8000, 0x0004},
		{0x0000, 0x0000, 0x0005},
		{0x0000, 0x11e0, 0x0006},
		{0x0004, 0x0000, 0x0007},
		{0x0500, 0x8000, 0x0008},
		{0x0000, 0x0000, 0x0009},
		{0x0000, 0x11e0, 0x000a},
		{0x0004, 0x0000, 0x000b},
		{0x0400, 0x8000, 0x000c},
		{0x0000, 0x0000, 0x000d},
		{0x0000, 0x0020, 0x000e},
		{0x0600, 0x8000, 0x0010},
		{0x0000, 0x0000, 0x0011},
		{0x0000, 0x11e0, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0700, 0x8000, 0x0014},
		{0x0000, 0x0000, 0x0015},
		{0x0000, 0x11e0, 0x0016},
		{0x0004, 0x0000, 0x0017},
		{0x0500, 0x8000, 0x0018},
		{0x0000, 0x0000, 0x0019},
		{0x0000, 0x11a0, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0400, 0x8000, 0x001c},
		{0x0000, 0x0000, 0x001d},
		{0x0000, 0x0020, 0x001e},
		{0x0600, 0x8000, 0x0020},
		{0x0000, 0x0000, 0x0021},
		{0x0000, 0x11e0, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0002, 0x0000, 0x002c},
		{0x0003, 0x0015, 0x0030},
		{0x2030, 0x0008, 0x0034},
		{0x0200, 0x0006, 0x0038},
		{0x8031, 0x000a, 0x003c},
		{0x0400, 0x0019, 0x0040},
		{0x0020, 0x0000, 0x0044},
		{0xa012, 0x0001, 0x0051},
		{0xa101, 0x0001, 0x0055},
		{0x3002, 0x0002, 0x0059},
		{0x1102, 0x0001, 0x005d},
		{0xb102, 0x0000, 0x0065},
		{0x00c0, 0x0000, 0x0074},
		{0x8000, 0x0040, 0x0078},
		{0xc830, 0x0500, 0x007c},
		{0x080c, 0x0803, 0x0080},
		{0x0000, 0x0109, 0x0084},
		{0x2020, 0x0000, 0x0088},
		{0x3001, 0x0001, 0x0099},
		{0x3012, 0x0001, 0x009d},
		{0xc101, 0x0000, 0x00a1},
		{0xc001, 0x0000, 0x00a5},
		{0xa001, 0x0001, 0x00a9},
		{0x0030, 0x0020, 0x00bc},
		{0x0010, 0x0040, 0x00c0},
		{0x0800, 0x00c8, 0x00c4},
		{0x2002, 0x0080, 0x00c8},
		{0x02c1, 0x0023, 0x00cc},
		{0x4000, 0x0001, 0x00d0},
		{0xc001, 0x0001, 0x00e1},
		{0x3002, 0x0002, 0x00e5},
		{0xa101, 0x0000, 0x00e9},
		{0xc102, 0x0001, 0x00ed},
		{0x1102, 0x0001, 0x00f1},
		{0xc011, 0x0001, 0x00f5},
		{0x0040, 0x0010, 0x0104},
		{0x0020, 0x0000, 0x0108},
		{0x0004, 0x0000, 0x010c},
		{0x0040, 0x0010, 0x0110},
		{0x0002, 0x0020, 0x0114},
		{0x0080, 0x0003, 0x0118},
		{0x0000, 0x0001, 0x011c},
		{0x0400, 0x8000, 0x0124},
		{0x0000, 0x0000, 0x0125},
		{0x0000, 0x0020, 0x0126},
		{0x0700, 0x8000, 0x0128},
		{0x0000, 0x0000, 0x0129},
		{0x0000, 0x0020, 0x012a},
		{0x0500, 0x8000, 0x012c},
		{0x0000, 0x0000, 0x012d},
		{0x0000, 0x0020, 0x012e},
		{0x0600, 0x8000, 0x0130},
		{0x0000, 0x0000, 0x0131},
		{0x0000, 0x0020, 0x0132},
		{0x0600, 0x8000, 0x0134},
		{0x0000, 0x0000, 0x0135},
		{0x0000, 0x11a0, 0x0136},
		{0x0000, 0x0000, 0x0137},
		{0x0400, 0x8000, 0x0138},
		{0x0000, 0x0000, 0x0139},
		{0x0000, 0x0020, 0x013a},
		{0x0700, 0x8000, 0x013c},
		{0x0000, 0x0000, 0x013d},
		{0x0000, 0x11e0, 0x013e},
		{0x0000, 0x0000, 0x013f},
		{0x0500, 0x8000, 0x0140},
		{0x0000, 0x0000, 0x0141},
		{0x0000, 0x0020, 0x0142},
	};

	load_cfg((void*)cin, 0x4000, 588, 0, 0);
	load_data(din_addr[0], 0x2000, 1024, 0, 0, 0);
	load_data(din_addr[1], 0x3000, 1024, 0, 0, 0);
	load_data(din_addr[2], 0x2400, 1024, 0, 0, 0);
	load_data(din_addr[3], 0x1000, 1024, 0, 0, 0);
	load_data(din_addr[4], 0x0, 1024, 0, 0, 0);
	load_data(din_addr[5], 0x2800, 1024, 0, 0, 0);
	load_data(din_addr[6], 0x2c00, 1024, 0, 0, 0);
	load_data(din_addr[7], 0x3400, 1024, 0, 0, 0);
	config(0x0, 98, 0, 0);
	execute(0xffff, 0, 0);
	store(dout_addr[0], 0x800, 1024, 0, 0);
	store(dout_addr[1], 0x3800, 1024, 0, 0);
	store(dout_addr[2], 0x3c00, 1024, 0, 0);
	store(dout_addr[3], 0x400, 1024, 0, 0);
	store(dout_addr[4], 0xc00, 1024, 0, 0);
	store(dout_addr[5], 0x1400, 1024, 0, 0);
	store(dout_addr[6], 0x1c00, 1024, 0, 0);
	store(dout_addr[7], 0x1800, 1024, 0, 0);
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
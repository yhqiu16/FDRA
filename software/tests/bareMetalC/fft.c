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
TYPE XR0[N];
TYPE XR1[N];
TYPE XR2[N];
TYPE XR3[N];
TYPE XI0[N];
TYPE XI1[N];
TYPE XI2[N];
TYPE XI3[N];
TYPE YR0[N];
TYPE YR1[N];
TYPE YR2[N];
TYPE YR3[N];
TYPE YI0[N];
TYPE YI1[N];
TYPE YI2[N];
TYPE YI3[N];
TYPE ZR0[N];
TYPE ZR1[N];
TYPE ZR2[N];
TYPE ZR3[N];
TYPE ZI0[N];
TYPE ZI1[N];
TYPE ZI2[N];
TYPE ZI3[N];


void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[104][3] __attribute__((aligned(8))) = {
		{0x3800, 0x0000, 0x0004},
		{0x0010, 0x0000, 0x0005},
		{0x0000, 0x8d00, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2800, 0x0000, 0x0008},
		{0x0010, 0x0000, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x2000, 0x0000, 0x000c},
		{0x0010, 0x0000, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x3000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x8d00, 0x0012},
		{0x0080, 0x0000, 0x0013},
		{0x3000, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x8d00, 0x0016},
		{0x0080, 0x0000, 0x0017},
		{0x2800, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x2000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x8d00, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0400, 0x0000, 0x002c},
		{0x0400, 0x0000, 0x0030},
		{0x0000, 0x0004, 0x0034},
		{0x0200, 0x0008, 0x0038},
		{0x8800, 0x0000, 0x003c},
		{0x1400, 0x0001, 0x0040},
		{0x0830, 0x0000, 0x0044},
		{0x2802, 0x0000, 0x0051},
		{0x2401, 0x0000, 0x0055},
		{0x4481, 0x0000, 0x005d},
		{0x2812, 0x0000, 0x0061},
		{0x2401, 0x0000, 0x0065},
		{0x2402, 0x0000, 0x0069},
		{0x0000, 0x0000, 0x0070},
		{0x0000, 0x0200, 0x0074},
		{0x0400, 0x0200, 0x008c},
		{0x5002, 0x0000, 0x0095},
		{0x2c81, 0x0000, 0x00b1},
		{0x0303, 0x0000, 0x00bc},
		{0x0330, 0x0000, 0x00d4},
		{0x8401, 0x0000, 0x00e1},
		{0x6412, 0x0000, 0x00f9},
		{0x0000, 0x0040, 0x0104},
		{0x0004, 0x0080, 0x0108},
		{0x0800, 0x0080, 0x010c},
		{0x3200, 0x0000, 0x0110},
		{0x4000, 0x0001, 0x0114},
		{0x0040, 0x0208, 0x011c},
		{0x9001, 0x0000, 0x0125},
		{0x8c02, 0x0000, 0x0129},
		{0x2811, 0x0000, 0x0131},
		{0x4482, 0x0000, 0x0135},
		{0x8c01, 0x0000, 0x0139},
		{0x8c82, 0x0000, 0x013d},
		{0x0014, 0x0000, 0x014c},
		{0x0304, 0x0000, 0x0150},
		{0x0100, 0x0000, 0x0154},
		{0x0010, 0x0010, 0x015c},
		{0x0020, 0x0000, 0x0160},
		{0x0304, 0x0000, 0x0164},
		{0x2000, 0x0000, 0x016c},
		{0x0010, 0x0000, 0x016d},
		{0x0000, 0x0100, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x2800, 0x0000, 0x0170},
		{0x0010, 0x0000, 0x0171},
		{0x0000, 0x0100, 0x0172},
		{0x0000, 0x0000, 0x0173},
		{0x3800, 0x0000, 0x0174},
		{0x0010, 0x0000, 0x0175},
		{0x0000, 0x8d00, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x3000, 0x0000, 0x0178},
		{0x0010, 0x0000, 0x0179},
		{0x0000, 0x8d00, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x2800, 0x0000, 0x017c},
		{0x0010, 0x0000, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x3000, 0x0000, 0x0180},
		{0x0010, 0x0000, 0x0181},
		{0x0000, 0x8d00, 0x0182},
		{0x0000, 0x0000, 0x0183},
		{0x2000, 0x0000, 0x0184},
		{0x0010, 0x0000, 0x0185},
		{0x0000, 0x0100, 0x0186},
		{0x0000, 0x0000, 0x0187},
		{0x3800, 0x0000, 0x0188},
		{0x0010, 0x0000, 0x0189},
		{0x0000, 0x8d00, 0x018a},
		{0x0000, 0x0000, 0x018b},
	};

	load_cfg((void*)cin, 0x20000, 624, 0, 0);
	load_data(din_addr[0], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[2], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[3], 0x2000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[5], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[6], 0x12000, 4096, 0, 0, 0);
	load_data(din_addr[7], 0x1a000, 4096, 0, 0, 0);
	config(0x0, 104, 0, 0);
	execute(0xffff, 0, 0);
	store(dout_addr[0], 0x14000, 4096, 0, 0);
	store(dout_addr[1], 0x4000, 4096, 0, 0);
	store(dout_addr[2], 0x1c000, 4096, 0, 0);
	store(dout_addr[3], 0xc000, 4096, 0, 0);
	store(dout_addr[4], 0x16000, 4096, 0, 0);
	store(dout_addr[5], 0xe000, 4096, 0, 0);
	store(dout_addr[6], 0x6000, 4096, 0, 0);
	store(dout_addr[7], 0x1e000, 4096, 0, 0);
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
#include "include/encoding.h"
#include "include/ISA.h"


// ################################ conv2d_3x3 ##########################################
#define NI 32
#define NJ 32
#define SIZE NI*NJ

int conv2d_3x3_A[SIZE];
int conv2d_3x3_B[SIZE];
int conv2d_3x3_C[SIZE];

/*For 3x3 filter*/
#define c11 2
#define c12 -3
#define c21 5
#define c22 6
#define c13 4
#define c23 7
#define c31 -8
#define c32 -9
#define c33 10

__attribute__((noinline))
void conv2d_3x3() {
	for (int i = 1; i < NI -1; i++) {
		for (int j = 1; j < NJ-1; j++) {
#ifdef CGRA_COMPILER
please_map_me();
#endif
	/*For 3x3 filter*/
		conv2d_3x3_C[i*NJ + j] = c11 * conv2d_3x3_A[(i - 1)*NJ + (j - 1)]  +  c12 * conv2d_3x3_A[(i + 0)*NJ + (j - 1)]  +  c13 * conv2d_3x3_A[(i + 1)*NJ + (j - 1)]
				+ c21 * conv2d_3x3_A[(i - 1)*NJ + (j + 0)]  +  c22 * conv2d_3x3_A[(i + 0)*NJ + (j + 0)]  +  c23 * conv2d_3x3_A[(i + 1)*NJ + (j + 0)]
				+ c31 * conv2d_3x3_A[(i - 1)*NJ + (j + 1)]  +  c32 * conv2d_3x3_A[(i + 0)*NJ + (j + 1)]  +  c33 * conv2d_3x3_A[(i + 1)*NJ + (j + 1)];
		}
	}
}


// ################################ fir-unroll4 ##########################################
#define NTAPS 32
int fir_unroll4_input[NTAPS];
int fir_unroll4_output[NTAPS];
int fir_unroll4_res[NTAPS];
int coefficients[NTAPS] = {025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125,
025, 150, 375, -225, 050, 075, -300, 125};

__attribute__((noinline))
void fir_unroll4()//(int fir_unroll4_input[], int fir_unroll4_output[], int coefficients[])
/*   fir_unroll4_input :           fir_unroll4_input sample array */
/*   fir_unroll4_output:           fir_unroll4_output sample array */
/*   coefficient:      coefficient array */
{
  int i;
  int j = 0;

 for(j=0; j< NTAPS; ++j) {
 	  int sum = 0;
    #pragma unroll 4
    for (i = 0; i < NTAPS; ++i) {
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      sum += fir_unroll4_input[i] * coefficients[i];
    }
    fir_unroll4_res[j] += sum;
 }
}


// ################################ pedometer ##########################################
#define pedometer_SIZE  1024
#define pedometer_SIZE  1024

int acc_vector[pedometer_SIZE ];
int res_vector[pedometer_SIZE ];
int acceleration_x[pedometer_SIZE ];
int acceleration_y[pedometer_SIZE ];
int acceleration_z[pedometer_SIZE ];
int acc_avg_x[pedometer_SIZE ];
int acc_avg_y[pedometer_SIZE ];
int acc_avg_z[pedometer_SIZE];

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
__attribute__((noinline))
void pedometer(){
	for(int i=0;i<pedometer_SIZE;i++){
		res_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}


// ################################ stencil2d ##########################################
#define TYPE unsigned
#define col_size 32
#define row_size 32
#define f_size 9

TYPE orig[row_size * col_size] __attribute__((aligned(8)));
TYPE sol[row_size * col_size] __attribute__((aligned(8)));
TYPE filter[f_size] __attribute__((aligned(8)));
TYPE res[row_size * col_size] __attribute__((aligned(8)));

__attribute__((noinline))
void stencil2d(){
    int r, c, k1, k2;
    TYPE temp, mul;

    for (r=0; r<row_size-2; r++) {
        for (c=0; c<col_size-2; c++) {
            temp = (TYPE)0;
            for (k1=0;k1<3;k1++){
                #pragma unroll
                for (k2=0;k2<3;k2++){
                    mul = filter[k1*3 + k2] * orig[(r+k1)*col_size + c+k2];
                    temp += mul;
                }
            }
            res[(r*col_size) + c] = temp;
        }
    }
}



// ################################ gemm-unroll4 ##########################################
#define N 1024
TYPE gemm_unroll4_m1[N];
TYPE gemm_unroll4_m2[N];
TYPE gemm_unroll4_prod0[N];
TYPE gemm_unroll4_prod[N];

__attribute__((noinline))
void gemm_unroll4()
{
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            #pragma unroll 4
            for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = gemm_unroll4_m1[i_col + k] * gemm_unroll4_m2[k_col + j];
                sum += mult;
            }
            gemm_unroll4_prod0[i_col + j]  = sum;
        }
    }
}


// ################################ fft ##########################################
#define DATA_TYPE int
#define gemver_N 20
//#define _PB_N 20

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


// ################################ knn ##########################################
#define nAtoms        64
#define maxNeighbors  16
// LJ coefficients
#define lj1           1//1.5
#define lj2           2//2.0

TYPE force_x[nAtoms];
TYPE force_y[nAtoms];
TYPE force_z[nAtoms];
TYPE force_x0[nAtoms];
TYPE force_y0[nAtoms];
TYPE force_z0[nAtoms];
TYPE position_x[nAtoms];
TYPE position_y[nAtoms];
TYPE position_z[nAtoms];
unsigned NL[nAtoms*maxNeighbors];

__attribute__((noinline))
void md_kernel()
{
    TYPE delx, dely, delz, r2inv;
    TYPE r6inv, potential, force, j_x, j_y, j_z;
    TYPE i_x, i_y, i_z, fx, fy, fz;

    int32_t i, j, jidx;

loop_i : for (i = 0; i < nAtoms; i++){
             i_x = position_x[i];
             i_y = position_y[i];
             i_z = position_z[i];
             fx = 0;
             fy = 0;
             fz = 0;
loop_j : for( j = 0; j < maxNeighbors; j++){
//	please_map_me();
             // Get neighbor
             jidx = NL[i*maxNeighbors + j];
             // Look up x,y,z positions
             j_x = position_x[jidx];
             j_y = position_y[jidx];
             j_z = position_z[jidx];
             // Calc distance
             delx = i_x - j_x;
             dely = i_y - j_y;
             delz = i_z - j_z;
             r2inv = 10/( delx*delx + dely*dely + delz*delz );//r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
             // Assume no cutoff and aways account for all nodes in area
             r6inv = r2inv * r2inv * r2inv;
             potential = r6inv*(lj1*r6inv - lj2*1000);
             // Sum changes in force
             force = r2inv*potential;
             /*potential = r6inv/100000000*(lj1*r6inv - lj2*1000000000);
             // Sum changes in force
             force = r2inv*potential/10000000000;*/
             fx += delx * force;
             fy += dely * force;
             fz += delz * force;
         }
         //Update forces after all neighbors accounted for.
         force_x0[i] = fx;
         force_y0[i] = fy;
         force_z0[i] = fz;
         //printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
         }
}



void pedometer_knn_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[154][3] __attribute__((aligned(8))) = {
		{0x1800, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0x0300, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x1000, 0x0000, 0x0008},
		{0x0010, 0x0000, 0x0009},
		{0x0000, 0x0b00, 0x000a},
		{0x0081, 0x0000, 0x000b},
		{0x2800, 0x4000, 0x000c},
		{0x0040, 0x0200, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x2000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x2000, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2800, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x3000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x9500, 0x0022},
		{0x0080, 0x0000, 0x0023},
		{0x2000, 0x0000, 0x002c},
		{0x0010, 0x0000, 0x002d},
		{0x0000, 0x0100, 0x002e},
		{0x0000, 0x0000, 0x002f},
		{0x2800, 0x0000, 0x0030},
		{0x0010, 0x0000, 0x0031},
		{0x0000, 0x0100, 0x0032},
		{0x0000, 0x0000, 0x0033},
		{0x0000, 0x0004, 0x003c},
		{0x0002, 0x0018, 0x0040},
		{0x0000, 0x0009, 0x0044},
		{0x0400, 0x0008, 0x0048},
		{0x0200, 0x0000, 0x004c},
		{0x0400, 0x0000, 0x0050},
		{0x0000, 0x0000, 0x0058},
		{0x5400, 0x0000, 0x0064},
		{0x4442, 0x0000, 0x006d},
		{0x6c03, 0x0000, 0x0071},
		{0x6c03, 0x0000, 0x0075},
		{0x0004, 0x0000, 0x0078},
		{0x0403, 0x0000, 0x0079},
		{0x2402, 0x0000, 0x007d},
		{0x0400, 0x0000, 0x0081},
		{0x2402, 0x0000, 0x0085},
		{0x6d23, 0x0000, 0x0089},
		{0x7101, 0x0000, 0x008d},
		{0x4803, 0x0000, 0x0095},
		{0x2402, 0x0000, 0x0099},
		{0x0000, 0x0040, 0x00a4},
		{0x10e0, 0x0600, 0x00a8},
		{0x0000, 0x0000, 0x00ac},
		{0x0200, 0x0040, 0x00b0},
		{0x0100, 0x0080, 0x00b4},
		{0x0000, 0x0080, 0x00b8},
		{0x0000, 0x0080, 0x00bc},
		{0x0000, 0x0080, 0x00c0},
		{0x0800, 0x0000, 0x00c4},
		{0x0000, 0x0000, 0x00c8},
		{0x9003, 0x0000, 0x00d5},
		{0x8881, 0x0000, 0x00d9},
		{0x8401, 0x0000, 0x00dd},
		{0x4803, 0x0000, 0x00e1},
		{0xf830, 0xffff, 0x00e4},
		{0x0401, 0x0000, 0x00e5},
		{0x0400, 0x0000, 0x00e9},
		{0x0c00, 0x0000, 0x00ed},
		{0x2831, 0x0000, 0x00f9},
		{0x0005, 0x0040, 0x010c},
		{0x0004, 0x0690, 0x0110},
		{0x0b04, 0x0100, 0x0114},
		{0x0400, 0x0080, 0x0118},
		{0x0400, 0x0080, 0x011c},
		{0x0020, 0x0000, 0x0120},
		{0x0000, 0x0000, 0x0124},
		{0x000a, 0x0000, 0x013c},
		{0x4004, 0x0000, 0x013d},
		{0x6c03, 0x0000, 0x0141},
		{0x9003, 0x0000, 0x0145},
		{0x2483, 0x0000, 0x0149},
		{0x2c03, 0x0000, 0x014d},
		{0x2e03, 0x0000, 0x0151},
		{0x0c0d, 0x0000, 0x0155},
		{0x0000, 0x2001, 0x0156},
		{0x0041, 0x0010, 0x0157},
		{0x0400, 0x0000, 0x0159},
		{0x0050, 0x0040, 0x0174},
		{0x2300, 0x0080, 0x0178},
		{0x0000, 0x0010, 0x017c},
		{0x0000, 0x0000, 0x0180},
		{0x0000, 0x0000, 0x0184},
		{0x0000, 0x0280, 0x0188},
		{0x0600, 0x0000, 0x018c},
		{0x9042, 0x0000, 0x01a5},
		{0x0004, 0x0000, 0x01a8},
		{0x0803, 0x0000, 0x01a9},
		{0x0004, 0x0000, 0x01ac},
		{0x0403, 0x0000, 0x01ad},
		{0x7042, 0x0000, 0x01b1},
		{0x0c30, 0x0000, 0x01b5},
		{0x6603, 0x0000, 0x01b9},
		{0x0c0d, 0x0000, 0x01bd},
		{0x0000, 0x2001, 0x01be},
		{0x0041, 0x0010, 0x01bf},
		{0x2603, 0x0000, 0x01c1},
		{0x0c0d, 0x0000, 0x01c5},
		{0x0000, 0x3001, 0x01c6},
		{0x0041, 0x0010, 0x01c7},
		{0x0101, 0x0000, 0x01dc},
		{0x0100, 0x0000, 0x01e0},
		{0x0040, 0x0000, 0x01e4},
		{0x0000, 0x0000, 0x01e8},
		{0x0000, 0x0000, 0x01ec},
		{0x2100, 0x0000, 0x01f0},
		{0x0100, 0x0000, 0x01f8},
		{0x1000, 0x4000, 0x020c},
		{0x0040, 0x0200, 0x020d},
		{0x0000, 0x0300, 0x020e},
		{0x0000, 0x0000, 0x020f},
		{0x0800, 0x0000, 0x0210},
		{0x0010, 0x0000, 0x0211},
		{0x0000, 0x0b00, 0x0212},
		{0x0001, 0x0000, 0x0213},
		{0x0000, 0x0000, 0x0214},
		{0x0010, 0x0000, 0x0215},
		{0x0000, 0x0b00, 0x0216},
		{0x0001, 0x0000, 0x0217},
		{0x0000, 0x4000, 0x021c},
		{0x0040, 0x0200, 0x021d},
		{0x0000, 0x0300, 0x021e},
		{0x0000, 0x0000, 0x021f},
		{0x1000, 0x4000, 0x0220},
		{0x0040, 0x0200, 0x0221},
		{0x0000, 0xa900, 0x0222},
		{0x0080, 0x0000, 0x0223},
		{0x0800, 0x4000, 0x0224},
		{0x0040, 0x0200, 0x0225},
		{0x0000, 0xa700, 0x0226},
		{0x0000, 0x0000, 0x0227},
		{0x0000, 0x4000, 0x022c},
		{0x0040, 0x0200, 0x022d},
		{0x0000, 0xa900, 0x022e},
		{0x0000, 0x0000, 0x022f},
	};

	load_cfg((void*)cin, 0x30000, 924, 0, 0);
	load_data(din_addr[0], 0x2000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x18000, 256, 1, 0, 0);
	load_data(din_addr[2], 0x20000, 256, 0, 0, 0);
	load_data(din_addr[3], 0x4000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x6000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x1a000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x1c000, 256, 0, 0, 0);
	load_data(din_addr[7], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x12000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0xc000, 4096, 0, 0, 0);
	load_data(din_addr[10], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[11], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0xa000, 4096, 0, 0, 0);
	config(0x0, 154, 0, 0);
	execute(0x177cff, 0, 0);
	store(dout_addr[0], 0xe000, 4096, 0, 0);
	store(dout_addr[1], 0x22000, 256, 0, 0);
	store(dout_addr[2], 0x24000, 256, 0, 0);
	store(dout_addr[3], 0x28000, 256, 0, 0);
}


void conv3x3_fir_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[160][3] __attribute__((aligned(8))) = {
		{0x2440, 0x7800, 0x0004},
		{0x00c0, 0x00f0, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x2c02, 0x7800, 0x0008},
		{0x00c0, 0x00f0, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x1840, 0x2000, 0x000c},
		{0x0040, 0x0100, 0x000d},
		{0x0000, 0x0b00, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x9042, 0x2000, 0x0010},
		{0xf900, 0x0107, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x8402, 0x2000, 0x0014},
		{0xf900, 0x0107, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x9403, 0x2000, 0x0018},
		{0xf900, 0x0107, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x8c03, 0x2000, 0x001c},
		{0xf900, 0x0107, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2420, 0x7800, 0x0024},
		{0x00c0, 0x00f0, 0x0025},
		{0x0000, 0x0100, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x9801, 0x2000, 0x0028},
		{0xf900, 0x0107, 0x0029},
		{0x0000, 0x0100, 0x002a},
		{0x0000, 0x0000, 0x002b},
		{0x2c21, 0x7800, 0x002c},
		{0x00c0, 0x00f0, 0x002d},
		{0x0000, 0x0100, 0x002e},
		{0x0000, 0x0000, 0x002f},
		{0x9001, 0x2000, 0x0030},
		{0xf900, 0x0107, 0x0031},
		{0x0000, 0x0100, 0x0032},
		{0x0000, 0x0000, 0x0033},
		{0x0000, 0x0004, 0x003c},
		{0x0000, 0x0008, 0x0040},
		{0x0000, 0x0008, 0x0044},
		{0x0400, 0x0008, 0x0048},
		{0x0000, 0x0008, 0x004c},
		{0x1400, 0x0038, 0x0050},
		{0x0000, 0x000a, 0x0054},
		{0x0000, 0x0022, 0x0058},
		{0x0000, 0x0002, 0x005c},
		{0x0000, 0x0003, 0x0060},
		{0x0000, 0x0000, 0x0064},
		{0xfff8, 0xffff, 0x006c},
		{0x0813, 0x0000, 0x006d},
		{0x0002, 0x0000, 0x0070},
		{0x0405, 0x0000, 0x0071},
		{0x7181, 0x0000, 0x0075},
		{0x8441, 0x0000, 0x0079},
		{0x2493, 0x0000, 0x007d},
		{0x8841, 0x0000, 0x0081},
		{0x2523, 0x0000, 0x0085},
		{0xfffd, 0xffff, 0x0090},
		{0x0403, 0x0000, 0x0091},
		{0x4403, 0x0000, 0x0095},
		{0x0006, 0x0000, 0x0098},
		{0x0433, 0x0000, 0x0099},
		{0x0000, 0x0000, 0x00a8},
		{0x0000, 0x0200, 0x00ac},
		{0x3000, 0x0080, 0x00b0},
		{0x0000, 0x0083, 0x00b4},
		{0x8204, 0x0003, 0x00b8},
		{0x0000, 0x0043, 0x00bc},
		{0x0c00, 0x0083, 0x00c0},
		{0x1000, 0x0080, 0x00c4},
		{0x1200, 0x0220, 0x00c8},
		{0x0000, 0x0000, 0x00cc},
		{0x6901, 0x0000, 0x00e1},
		{0x080d, 0x0000, 0x00e5},
		{0x0000, 0xb001, 0x00e6},
		{0x0020, 0x0008, 0x00e7},
		{0x4441, 0x0000, 0x00e9},
		{0x8581, 0x0000, 0x00ed},
		{0x2a01, 0x0000, 0x00f5},
		{0x6a01, 0x0000, 0x00f9},
		{0x4601, 0x0000, 0x00fd},
		{0x00c0, 0x0600, 0x0114},
		{0x0008, 0x0000, 0x0124},
		{0x0000, 0x0203, 0x0128},
		{0x00c0, 0x0003, 0x012c},
		{0x0300, 0x0004, 0x0130},
		{0x7201, 0x0000, 0x0155},
		{0x6481, 0x0000, 0x0165},
		{0x0000, 0x0040, 0x0178},
		{0x0000, 0x0688, 0x017c},
		{0x0000, 0x0080, 0x0180},
		{0x0000, 0x0080, 0x0184},
		{0x0180, 0x0000, 0x0188},
		{0x0002, 0x0000, 0x018c},
		{0x0000, 0x0002, 0x0190},
		{0x0000, 0x0040, 0x0194},
		{0x0080, 0x0000, 0x0198},
		{0xfff7, 0xffff, 0x01a8},
		{0x0c43, 0x0000, 0x01a9},
		{0x0001, 0x0000, 0x01ac},
		{0x0c05, 0x0000, 0x01ad},
		{0x000a, 0x0000, 0x01b8},
		{0x0c43, 0x0000, 0x01b9},
		{0x6601, 0x0000, 0x01bd},
		{0x8c03, 0x0000, 0x01c5},
		{0x0007, 0x0000, 0x01c8},
		{0x0c43, 0x0000, 0x01c9},
		{0x0005, 0x0000, 0x01cc},
		{0x0c03, 0x0000, 0x01cd},
		{0x0010, 0x0000, 0x01dc},
		{0x0010, 0x0000, 0x01e0},
		{0x0300, 0x0000, 0x01e4},
		{0x0010, 0x0000, 0x01ec},
		{0x0100, 0x0000, 0x01f0},
		{0x0010, 0x0000, 0x01f8},
		{0x0018, 0x0000, 0x01fc},
		{0x0010, 0x0001, 0x0200},
		{0x2862, 0x7800, 0x020c},
		{0x00c0, 0x00f0, 0x020d},
		{0x0000, 0x0300, 0x020e},
		{0x0000, 0x0000, 0x020f},
		{0x2040, 0x7800, 0x0210},
		{0x00c0, 0x00f0, 0x0211},
		{0x0000, 0x0100, 0x0212},
		{0x0000, 0x0000, 0x0213},
		{0x1000, 0x2000, 0x0218},
		{0x0040, 0x0100, 0x0219},
		{0x0000, 0x9d00, 0x021a},
		{0x0000, 0x0000, 0x021b},
		{0x2082, 0x7800, 0x021c},
		{0x00c0, 0x00f0, 0x021d},
		{0x0000, 0x0900, 0x021e},
		{0x0000, 0x0000, 0x021f},
		{0x2841, 0x7800, 0x0224},
		{0x00c0, 0x00f0, 0x0225},
		{0x0000, 0xa300, 0x0226},
		{0x0000, 0x0000, 0x0227},
		{0x9800, 0x2000, 0x0228},
		{0xf900, 0x0107, 0x0229},
		{0x0000, 0x0100, 0x022a},
		{0x0000, 0x0000, 0x022b},
		{0x3041, 0x7800, 0x022c},
		{0x00c0, 0x00f0, 0x022d},
		{0x0000, 0x0100, 0x022e},
		{0x0000, 0x0000, 0x022f},
		{0x2801, 0x7800, 0x0230},
		{0x00c0, 0x00f0, 0x0231},
		{0x0000, 0x0100, 0x0232},
		{0x0000, 0x0000, 0x0233},
		{0x9800, 0x2000, 0x0234},
		{0xf900, 0x0107, 0x0235},
		{0x0000, 0x0100, 0x0236},
		{0x0000, 0x0000, 0x0237},
	};

	load_cfg((void*)cin, 0x30000, 960, 1, 2);
	load_data(din_addr[0], 0x11000, 4096, 1, 1, 0);
	load_data(din_addr[1], 0x1000, 4096, 1, 1, 0);
	load_data(din_addr[2], 0x2a000, 4096, 1, 1, 0);
	load_data(din_addr[3], 0x13000, 4096, 1, 1, 0);
	load_data(din_addr[4], 0x2c000, 4096, 1, 1, 0);
	load_data(din_addr[5], 0x3000, 4096, 1, 1, 0);
	load_data(din_addr[6], 0x1a100, 4096, 1, 1, 0);
	load_data(din_addr[7], 0x20100, 4096, 1, 1, 0);
	load_data(din_addr[8], 0x18100, 4096, 0, 1, 0);
	load_data(din_addr[9], 0x26000, 128, 1, 1, 0);
	load_data(din_addr[10], 0xd000, 128, 1, 1, 0);
	load_data(din_addr[11], 0x16000, 128, 1, 1, 0);
	load_data(din_addr[12], 0x4100, 128, 0, 1, 0);
	load_data(din_addr[13], 0x2e000, 128, 1, 1, 0);
	load_data(din_addr[14], 0xb000, 128, 1, 1, 0);
	load_data(din_addr[15], 0x9000, 128, 1, 1, 0);
	load_data(din_addr[16], 0x14000, 128, 0, 1, 0);
	load_data(din_addr[17], 0x6100, 128, 0, 1, 0);
	config(0x0, 160, 1, 0);
	execute(0x7dbf7f, 1, 0);
	store(dout_addr[0], 0x22100, 3836, 1, 0);
	store(dout_addr[1], 0x1c000, 128, 1, 0);
}


void stencil2d_gemm_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[108][3] __attribute__((aligned(8))) = {
		{0x0801, 0x0c04, 0x0004},
		{0xf040, 0x80f7, 0x0005},
		{0xefe1, 0x0101, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x6001, 0x0c00, 0x0008},
		{0xfe80, 0x00f7, 0x0009},
		{0xeffd, 0x0101, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x1000, 0x0c00, 0x000c},
		{0x0040, 0x80f0, 0x000d},
		{0xe001, 0x9301, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x18c0, 0x2010, 0x0010},
		{0x2040, 0x8107, 0x0011},
		{0x0e30, 0x0102, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x9003, 0x2000, 0x0014},
		{0xf900, 0x0107, 0x0015},
		{0x0002, 0x0102, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x0800, 0x2010, 0x0018},
		{0x2040, 0x8107, 0x0019},
		{0x0e30, 0x0102, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x8000, 0x2000, 0x001c},
		{0xf900, 0x0107, 0x001d},
		{0x0002, 0x0102, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x1800, 0x2000, 0x0020},
		{0x0040, 0x8100, 0x0021},
		{0x0000, 0x9502, 0x0022},
		{0x0080, 0x0000, 0x0023},
		{0x1860, 0x2010, 0x0024},
		{0x2040, 0x8107, 0x0025},
		{0x0e30, 0x0102, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x9022, 0x2000, 0x0028},
		{0xf900, 0x0107, 0x0029},
		{0x0002, 0x0102, 0x002a},
		{0x0000, 0x0000, 0x002b},
		{0x0820, 0x2010, 0x002c},
		{0x2040, 0x8107, 0x002d},
		{0x0e30, 0x0102, 0x002e},
		{0x0000, 0x0000, 0x002f},
		{0x8001, 0x2000, 0x0030},
		{0xf900, 0x0107, 0x0031},
		{0x0002, 0x0102, 0x0032},
		{0x0000, 0x0000, 0x0033},
		{0x0400, 0x0000, 0x003c},
		{0x0030, 0x0000, 0x0040},
		{0x0400, 0x0000, 0x0048},
		{0x0400, 0x0000, 0x0050},
		{0x0000, 0x0004, 0x0054},
		{0x0801, 0x0000, 0x0058},
		{0x2400, 0x0000, 0x005c},
		{0x0100, 0x0001, 0x0060},
		{0x0000, 0x0000, 0x0064},
		{0x2403, 0x0000, 0x0071},
		{0x2403, 0x0000, 0x007d},
		{0x2403, 0x0000, 0x0085},
		{0x0c0d, 0x0000, 0x0089},
		{0x0000, 0x9001, 0x008a},
		{0x0020, 0x0100, 0x008b},
		{0x2811, 0x0000, 0x008d},
		{0x2403, 0x0000, 0x0091},
		{0x4403, 0x0000, 0x0095},
		{0x0000, 0x0000, 0x00a8},
		{0x0000, 0x0000, 0x00b4},
		{0x0200, 0x0000, 0x00b8},
		{0xc010, 0x0000, 0x00bc},
		{0x0400, 0x0001, 0x00c0},
		{0x0000, 0x0000, 0x00c4},
		{0x100d, 0x0000, 0x00dd},
		{0x0000, 0x7001, 0x00de},
		{0x000c, 0x00e1, 0x00df},
		{0x4441, 0x0000, 0x00ed},
		{0x2821, 0x0000, 0x00f5},
		{0x0300, 0x0000, 0x0110},
		{0x0001, 0x0000, 0x0114},
		{0x8411, 0x0000, 0x0145},
		{0x0000, 0x0000, 0x017c},
		{0xc000, 0x0000, 0x0180},
		{0x0000, 0x0003, 0x0184},
		{0x0000, 0x0003, 0x0188},
		{0x0000, 0x0001, 0x018c},
		{0x8c03, 0x0000, 0x01ad},
		{0x4c11, 0x0000, 0x01b1},
		{0x8c03, 0x0000, 0x01c1},
		{0x0010, 0x0000, 0x01e0},
		{0x0000, 0x0000, 0x01e4},
		{0x0010, 0x0000, 0x01f4},
		{0x0000, 0x0000, 0x01f8},
		{0x1800, 0x0c04, 0x0210},
		{0xf040, 0x80f7, 0x0211},
		{0xefe1, 0x0101, 0x0212},
		{0x0000, 0x0000, 0x0213},
		{0x6440, 0x0c00, 0x0218},
		{0xfe80, 0x00f7, 0x0219},
		{0xeffd, 0x0101, 0x021a},
		{0x0000, 0x0000, 0x021b},
		{0x1822, 0x0c04, 0x0224},
		{0xf040, 0x80f7, 0x0225},
		{0xefe1, 0x0101, 0x0226},
		{0x0000, 0x0000, 0x0227},
		{0x6c02, 0x0c00, 0x022c},
		{0xfe80, 0x00f7, 0x022d},
		{0xeffd, 0x0101, 0x022e},
		{0x0000, 0x0000, 0x022f},
	};

	load_cfg((void*)cin, 0x30000, 648, 2, 2);
	load_data(din_addr[0], 0x8000, 4096, 1, 2, 0);
	load_data(din_addr[1], 0x10000, 4096, 1, 2, 0);
	load_data(din_addr[2], 0x14080, 4096, 1, 2, 0);
	load_data(din_addr[3], 0xc000, 4096, 0, 2, 0);
	load_data(din_addr[4], 0xa000, 4096, 1, 2, 0);
	load_data(din_addr[5], 0x12000, 4096, 1, 2, 0);
	load_data(din_addr[6], 0x16080, 4096, 1, 2, 0);
	load_data(din_addr[7], 0x6180, 4096, 0, 2, 0);
	load_data(din_addr[8], 0x19100, 36, 1, 2, 0);
	load_data(din_addr[9], 0x0, 36, 1, 2, 0);
	load_data(din_addr[10], 0x2b000, 36, 0, 2, 0);
	load_data(din_addr[11], 0x1e000, 4096, 1, 2, 0);
	load_data(din_addr[12], 0x2000, 4096, 1, 2, 0);
	load_data(din_addr[13], 0x26080, 4096, 0, 2, 0);
	config(0x0, 108, 2, 0);
	execute(0x14afff, 2, 0);
	store(dout_addr[0], 0x4000, 3832, 2, 0);
	store(dout_addr[1], 0xe000, 4096, 2, 0);
}


void fft_cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[107][3] __attribute__((aligned(8))) = {
		{0x2c00, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x2400, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3000, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x8b00, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x2000, 0x0000, 0x0024},
		{0x0010, 0x0000, 0x0025},
		{0x0000, 0x8d00, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x2800, 0x0000, 0x0028},
		{0x0010, 0x0000, 0x0029},
		{0x0000, 0x8f00, 0x002a},
		{0x0000, 0x0000, 0x002b},
		{0x3000, 0x0000, 0x002c},
		{0x0010, 0x0000, 0x002d},
		{0x0000, 0x8f00, 0x002e},
		{0x0000, 0x0000, 0x002f},
		{0x0400, 0x0000, 0x004c},
		{0x0400, 0x0004, 0x0050},
		{0x0030, 0x0008, 0x0054},
		{0x0810, 0x0020, 0x0058},
		{0x0010, 0x000c, 0x005c},
		{0x0020, 0x0000, 0x0060},
		{0x2802, 0x0000, 0x0081},
		{0x2401, 0x0000, 0x0085},
		{0x6c12, 0x0000, 0x0089},
		{0x3081, 0x0000, 0x008d},
		{0x00c0, 0x0000, 0x00bc},
		{0x0300, 0x0000, 0x00c0},
		{0x0003, 0x0008, 0x00c4},
		{0x2c01, 0x0000, 0x00f1},
		{0x8402, 0x0000, 0x00f5},
		{0x0030, 0x0020, 0x0124},
		{0x000c, 0x0020, 0x012c},
		{0xd000, 0x0000, 0x017c},
		{0x0000, 0x0043, 0x0180},
		{0x0800, 0x0043, 0x0184},
		{0x1000, 0x0081, 0x0188},
		{0x0000, 0x0080, 0x018c},
		{0x0200, 0x0080, 0x0190},
		{0xc000, 0x0080, 0x0194},
		{0x0800, 0x0003, 0x0198},
		{0x1000, 0x0001, 0x019c},
		{0x4812, 0x0000, 0x01ad},
		{0x9082, 0x0000, 0x01b1},
		{0x8c01, 0x0000, 0x01b5},
		{0x2821, 0x0000, 0x01b9},
		{0x9002, 0x0000, 0x01bd},
		{0x8c01, 0x0000, 0x01c1},
		{0x4402, 0x0000, 0x01c5},
		{0x9012, 0x0000, 0x01c9},
		{0x2821, 0x0000, 0x01cd},
		{0x8c01, 0x0000, 0x01d1},
		{0x0100, 0x0000, 0x01e0},
		{0x0016, 0x0000, 0x01e8},
		{0x0000, 0x0001, 0x01ec},
		{0x0016, 0x0000, 0x01f4},
		{0x0000, 0x0001, 0x01f8},
		{0x0008, 0x0000, 0x0200},
		{0x0010, 0x0001, 0x0204},
		{0x2800, 0x0000, 0x0214},
		{0x0010, 0x0000, 0x0215},
		{0x0000, 0x8f00, 0x0216},
		{0x0000, 0x0000, 0x0217},
		{0x2000, 0x0000, 0x0218},
		{0x0010, 0x0000, 0x0219},
		{0x0000, 0x0100, 0x021a},
		{0x0000, 0x0000, 0x021b},
		{0x3800, 0x0000, 0x021c},
		{0x0010, 0x0000, 0x021d},
		{0x0000, 0x8f00, 0x021e},
		{0x0080, 0x0000, 0x021f},
		{0x2000, 0x0000, 0x0220},
		{0x0010, 0x0000, 0x0221},
		{0x0000, 0x0100, 0x0222},
		{0x0000, 0x0000, 0x0223},
		{0x3000, 0x0000, 0x0224},
		{0x0010, 0x0000, 0x0225},
		{0x0000, 0x0100, 0x0226},
		{0x0000, 0x0000, 0x0227},
		{0x2800, 0x0000, 0x0228},
		{0x0010, 0x0000, 0x0229},
		{0x0000, 0x8d00, 0x022a},
		{0x0080, 0x0000, 0x022b},
		{0x2800, 0x0000, 0x022c},
		{0x0010, 0x0000, 0x022d},
		{0x0000, 0x0100, 0x022e},
		{0x0000, 0x0000, 0x022f},
		{0x3800, 0x0000, 0x0230},
		{0x0010, 0x0000, 0x0231},
		{0x0000, 0x8f00, 0x0232},
		{0x0080, 0x0000, 0x0233},
		{0x3000, 0x0000, 0x0234},
		{0x0010, 0x0000, 0x0235},
		{0x0000, 0x0100, 0x0236},
		{0x0000, 0x0000, 0x0237},
		{0x2000, 0x0000, 0x0238},
		{0x0010, 0x0000, 0x0239},
		{0x0000, 0x0100, 0x023a},
		{0x0000, 0x0000, 0x023b},
	};

	load_cfg((void*)cin, 0x30000, 642, 3, 2);
	load_data(din_addr[0], 0x9000, 4096, 0, 3, 0);
	load_data(din_addr[1], 0x2a000, 4096, 0, 3, 0);
	load_data(din_addr[2], 0xb000, 4096, 0, 3, 0);
	load_data(din_addr[3], 0x24000, 4096, 0, 3, 0);
	load_data(din_addr[4], 0x20000, 4096, 0, 3, 0);
	load_data(din_addr[5], 0x28000, 4096, 0, 3, 0);
	load_data(din_addr[6], 0x18000, 4096, 0, 3, 0);
	load_data(din_addr[7], 0x2c000, 4096, 0, 3, 0);
	config(0x0, 107, 3, 0);
	execute(0xffc7e0, 3, 0);
	store(dout_addr[0], 0x2e000, 4096, 3, 0);
	store(dout_addr[1], 0xc000, 4096, 3, 0);
	store(dout_addr[2], 0x22000, 4096, 3, 0);
	store(dout_addr[3], 0x10000, 4096, 3, 0);
	store(dout_addr[4], 0x26000, 4096, 3, 0);
	store(dout_addr[5], 0x12000, 4096, 3, 0);
	store(dout_addr[6], 0x1a000, 4096, 3, 0);
	store(dout_addr[7], 0x14000, 4096, 3, 0);
}


///* Array initialization. */
//void init_array()
//{
//int i,j;
//for (i=0;i<array_add_SIZE; i++){
//      gemver_A[i] = i * 2 + 5;
//      conv2d_2x2_B[i] = i * 3;
//    }
//
//}
//
//void result_check()
//{
//  int i, j;
//
//  for (i = 0; i < array_add_SIZE; i++)
//  {
//    if (conv2d_2x2_C[i] != D[i]) printf("There is an error in location (%d)[%d, %d]\n", i, conv2d_2x2_C[i], D[i]);
//  }
//}


int main(int argc, char** argv)
{
  long long unsigned start;
  long long unsigned end;
  volatile int result;
//  init_array();
//  printf("Initialization finished!\n");

  start = rdcycle();
  /* Run kernels. */
  pedometer();
  md_kernel();
  conv2d_3x3();
  fir_unroll4();
  stencil2d();
  gemm_unroll4();
  fft4();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  start = rdcycle();

//  void* pedometer_cgra_din_addr[6] = {acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
//  void* pedometer_cgra_dout_addr[1] = {res_vector};
  void* pedometer_knn_cgra_din_addr[13] = {NL, position_x, position_x, position_y, position_y, position_z, position_z, acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
  void* pedometer_knn_cgra_dout_addr[4] = {acc_vector, force_x, force_y, force_z};
  pedometer_knn_cgra_execute(pedometer_knn_cgra_din_addr, pedometer_knn_cgra_dout_addr);
  result = fence(1);

  void* conv3x3_fir_cgra_din_addr[18] = {conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, conv2d_3x3_A, coefficients, coefficients, coefficients, coefficients, fir_unroll4_input, fir_unroll4_input, fir_unroll4_input, fir_unroll4_input, fir_unroll4_output};
  void* conv3x3_fir_cgra_dout_addr[2] = {(void*)conv2d_3x3_B+128, fir_unroll4_output};
  conv3x3_fir_cgra_execute(conv3x3_fir_cgra_din_addr, conv3x3_fir_cgra_dout_addr);
  result = fence(1);

//  void* gemm_unroll4_cgra_din_addr[8] = {gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m2, gemm_unroll4_m2, gemm_unroll4_m2, gemm_unroll4_m2};
//  void* gemm_unroll4_cgra_dout_addr[1] = {gemm_unroll4_prod};
  void* stencil2d_gemm_cgra_din_addr[14] = {gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m1, gemm_unroll4_m2, gemm_unroll4_m2, gemm_unroll4_m2, gemm_unroll4_m2, filter, filter, filter, orig, orig, orig};
  void* stencil2d_gemm_cgra_dout_addr[2] = {sol, gemm_unroll4_prod};
  stencil2d_gemm_cgra_execute(stencil2d_gemm_cgra_din_addr, stencil2d_gemm_cgra_dout_addr);
  result = fence(1);

  void* fft_cgra_din_addr[8] = {XI0, XI1, XI2, XI3, XR0, XR1, XR2, XR3};
  void* fft_cgra_dout_addr[8] = {YR0, YI0, YR1, YI1, YR2, YI2, YR3, YI3};
  fft_cgra_execute(fft_cgra_din_addr, fft_cgra_dout_addr);
  result = fence(1);

  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

//  result_check();
  printf("Done!\n");

  return 0;
}
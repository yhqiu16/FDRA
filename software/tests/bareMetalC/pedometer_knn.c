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

#define TYPE unsigned

// Problem Constants
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

void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[152][3] __attribute__((aligned(8))) = {
		{0x1000, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x0000, 0x0000, 0x0008},
		{0x0010, 0x0000, 0x0009},
		{0x0000, 0x0900, 0x000a},
		{0x0081, 0x0000, 0x000b},
		{0x1800, 0x4000, 0x000c},
		{0x0040, 0x0200, 0x000d},
		{0x0000, 0x0300, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0800, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0b00, 0x0012},
		{0x0081, 0x0000, 0x0013},
		{0x3800, 0x4000, 0x0014},
		{0x0040, 0x0200, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x3000, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x9100, 0x001a},
		{0x0080, 0x0000, 0x001b},
		{0x2800, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2000, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x2000, 0x0000, 0x0024},
		{0x0010, 0x0000, 0x0025},
		{0x0000, 0x0100, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x2800, 0x0000, 0x0028},
		{0x0010, 0x0000, 0x0029},
		{0x0000, 0x0100, 0x002a},
		{0x0000, 0x0000, 0x002b},
		{0x0400, 0x0004, 0x0034},
		{0x0000, 0x0008, 0x0038},
		{0x2400, 0x0038, 0x003c},
		{0x0802, 0x0004, 0x0040},
		{0x0000, 0x0009, 0x0044},
		{0x0202, 0x0000, 0x0048},
		{0x0000, 0x0001, 0x004c},
		{0x0400, 0x0008, 0x0050},
		{0x0900, 0x0000, 0x0054},
		{0x1010, 0x0000, 0x005d},
		{0x2442, 0x0000, 0x0061},
		{0x0004, 0x0000, 0x0064},
		{0x0803, 0x0000, 0x0065},
		{0x2442, 0x0000, 0x0069},
		{0x3203, 0x0000, 0x006d},
		{0x0004, 0x0000, 0x0070},
		{0x0403, 0x0000, 0x0071},
		{0x0400, 0x0000, 0x0075},
		{0x8c01, 0x0000, 0x0079},
		{0x2402, 0x0000, 0x007d},
		{0x2412, 0x0000, 0x0081},
		{0x0501, 0x0000, 0x008c},
		{0x0000, 0x0600, 0x0094},
		{0x0001, 0x0000, 0x009c},
		{0x4010, 0x0000, 0x00a4},
		{0x0208, 0x0000, 0x00a8},
		{0x0500, 0x0001, 0x00ac},
		{0x0800, 0x0000, 0x00b5},
		{0x2403, 0x0000, 0x00b9},
		{0x4803, 0x0000, 0x00bd},
		{0x9103, 0x0000, 0x00c1},
		{0x8c03, 0x0000, 0x00c5},
		{0x040d, 0x0000, 0x00c9},
		{0x0000, 0x2001, 0x00ca},
		{0x0041, 0x0010, 0x00cb},
		{0x5001, 0x0000, 0x00cd},
		{0x4803, 0x0000, 0x00d1},
		{0x0420, 0x0000, 0x00d5},
		{0x2403, 0x0000, 0x00d9},
		{0x0400, 0x0000, 0x00e4},
		{0x0000, 0x0000, 0x00e8},
		{0x0000, 0x0600, 0x00ec},
		{0x0004, 0x0000, 0x00f0},
		{0x0000, 0x0200, 0x00f4},
		{0x0200, 0x0400, 0x00f8},
		{0xc000, 0x0000, 0x00fc},
		{0x0000, 0x0000, 0x0100},
		{0x1000, 0x0000, 0x010d},
		{0x2881, 0x0000, 0x0111},
		{0x6c01, 0x0000, 0x0115},
		{0x000a, 0x0000, 0x0118},
		{0x6004, 0x0000, 0x0119},
		{0x7083, 0x0000, 0x011d},
		{0xf830, 0xffff, 0x0120},
		{0x0c01, 0x0000, 0x0121},
		{0x4603, 0x0000, 0x0125},
		{0x9003, 0x0000, 0x0129},
		{0x3001, 0x0000, 0x013c},
		{0x5040, 0x0081, 0x0140},
		{0x0300, 0x0080, 0x0144},
		{0x0000, 0x0080, 0x0148},
		{0x0001, 0x0680, 0x014c},
		{0x0b00, 0x0000, 0x0150},
		{0x0000, 0x0000, 0x0158},
		{0x0800, 0x0000, 0x0165},
		{0x4803, 0x0000, 0x0169},
		{0x8c42, 0x0000, 0x016d},
		{0x0004, 0x0000, 0x0170},
		{0x0403, 0x0000, 0x0171},
		{0x2403, 0x0000, 0x0175},
		{0x080d, 0x0000, 0x0179},
		{0x0000, 0x3001, 0x017a},
		{0x0041, 0x0010, 0x017b},
		{0x2583, 0x0000, 0x017d},
		{0x0c0d, 0x0000, 0x0181},
		{0x0000, 0x3001, 0x0182},
		{0x0041, 0x0010, 0x0183},
		{0x8c02, 0x0000, 0x0185},
		{0x0010, 0x0000, 0x0198},
		{0x0100, 0x0000, 0x019c},
		{0x0300, 0x0000, 0x01a4},
		{0x0000, 0x0000, 0x01ac},
		{0x0010, 0x0000, 0x01b0},
		{0x0000, 0x0000, 0x01b4},
		{0x1000, 0x4000, 0x01c0},
		{0x0040, 0x0200, 0x01c1},
		{0x0000, 0x0500, 0x01c2},
		{0x0000, 0x0000, 0x01c3},
		{0x0000, 0x0000, 0x01c8},
		{0x0010, 0x0000, 0x01c9},
		{0x0000, 0x0d00, 0x01ca},
		{0x0001, 0x0000, 0x01cb},
		{0x1000, 0x4000, 0x01cc},
		{0x0040, 0x0200, 0x01cd},
		{0x0000, 0xa900, 0x01ce},
		{0x0080, 0x0000, 0x01cf},
		{0x0800, 0x4000, 0x01d0},
		{0x0040, 0x0200, 0x01d1},
		{0x0000, 0xa900, 0x01d2},
		{0x0000, 0x0000, 0x01d3},
		{0x1000, 0x4000, 0x01d4},
		{0x0040, 0x0200, 0x01d5},
		{0x0000, 0xa900, 0x01d6},
		{0x0080, 0x0000, 0x01d7},
		{0x2000, 0x0000, 0x01d8},
		{0x0010, 0x0000, 0x01d9},
		{0x0000, 0x0100, 0x01da},
		{0x0000, 0x0000, 0x01db},
		{0x2800, 0x0000, 0x01e0},
		{0x0010, 0x0000, 0x01e1},
		{0x0000, 0x0100, 0x01e2},
		{0x0000, 0x0000, 0x01e3},
	};

	load_cfg((void*)cin, 0x28000, 912, 0, 0);
	load_data(din_addr[0], 0xe000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x0, 256, 1, 0, 0);
	load_data(din_addr[2], 0x4000, 256, 0, 0, 0);
	load_data(din_addr[3], 0x2000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x6000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x18000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x14000, 256, 0, 0, 0);
	load_data(din_addr[7], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x22000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[10], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[11], 0x20000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0x12000, 4096, 0, 0, 0);
	config(0x0, 152, 0, 0);
	execute(0xbebff, 0, 0);
	store(dout_addr[0], 0xc000, 4096, 0, 0);
	store(dout_addr[1], 0x1a000, 256, 0, 0);
	store(dout_addr[2], 0x1c000, 256, 0, 0);
	store(dout_addr[3], 0x24000, 256, 0, 0);
}



/* Array initialization. */
static
void init_array()
{
    int i, j;
	for(i=0;i<SIZE;i++){
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
     for  (i = 0; i < nAtoms; i++)
     {
         position_x[i] = i;
         position_y[i] = i+1;
         position_z[i] = i+2;
 //        force_x[i] = 0;
 //        force_y[i] = 0;
 //        force_z[i] = 0;
     }
     for(i=0; i<nAtoms; i++){
         for(j = 0; j < maxNeighbors; ++j){
             NL[i*maxNeighbors+j] = (i+j)%nAtoms;
         }
     }
}


__attribute__((noinline))
void pedometer(){//(int input[], int output[], int coefficients[])
	for(int i=0;i<SIZE;i++){
		res_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}

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


static
void result_check()
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    if (acc_vector[i] != res_vector[i]) printf("There is an error in location (%d)[%d, %d]\n", i, acc_vector[i], res_vector[i]);
  }
  for (i = 0; i < nAtoms; i++)
  {
    if (force_x0[i] != force_x[i] || force_y0[i] != force_y[i] || force_z0[i] != force_z[i])
        printf("There is an error in location (%d)[%d, %d, %d; %d, %d, %d]\n",
            i, force_x0[i], force_x[i], force_y0[i], force_y[i], force_z0[i], force_z[i]);
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
  md_kernel();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[13] = {NL, position_x, position_x, position_y, position_y, position_z, position_z, acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
  void* cgra_dout_addr[4] = {acc_vector, force_x, force_y, force_z};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
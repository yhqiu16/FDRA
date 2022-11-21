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
	volatile unsigned short cin[107][3] __attribute__((aligned(8))) = {
		{0x0800, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0xa700, 0x0006},
		{0x0080, 0x0000, 0x0007},
		{0x0000, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0x0300, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0800, 0x4000, 0x0018},
		{0x0040, 0x0200, 0x0019},
		{0x0000, 0xa500, 0x001a},
		{0x0080, 0x0000, 0x001b},
		{0x0000, 0x4000, 0x001c},
		{0x0040, 0x0200, 0x001d},
		{0x0000, 0xa500, 0x001e},
		{0x0080, 0x0000, 0x001f},
		{0x1001, 0x0000, 0x002c},
		{0x2000, 0x0000, 0x0030},
		{0x0000, 0x0002, 0x0034},
		{0x0000, 0x0001, 0x0038},
		{0x0000, 0x0000, 0x0040},
		{0x0000, 0x0000, 0x0044},
		{0x080d, 0x0000, 0x004d},
		{0x0000, 0x2001, 0x004e},
		{0x0041, 0x0010, 0x004f},
		{0x6a03, 0x0000, 0x0051},
		{0x6442, 0x0000, 0x0055},
		{0x9003, 0x0000, 0x0059},
		{0x8c03, 0x0000, 0x005d},
		{0x6d83, 0x0000, 0x0061},
		{0x0c0d, 0x0000, 0x0065},
		{0x0000, 0x1001, 0x0066},
		{0x0041, 0x0010, 0x0067},
		{0x0c0d, 0x0000, 0x0069},
		{0x0000, 0x1001, 0x006a},
		{0x0041, 0x0010, 0x006b},
		{0x30c0, 0x0040, 0x0074},
		{0x00c0, 0x0080, 0x0078},
		{0x0000, 0x0080, 0x007c},
		{0x0035, 0x0080, 0x0080},
		{0x0080, 0x0000, 0x0084},
		{0x0200, 0x0000, 0x0088},
		{0x0010, 0x0000, 0x008c},
		{0x0800, 0x0000, 0x0095},
		{0x4803, 0x0000, 0x0099},
		{0x7001, 0x0000, 0x009d},
		{0x000a, 0x0000, 0x00a0},
		{0x6004, 0x0000, 0x00a1},
		{0x6483, 0x0000, 0x00a5},
		{0xf830, 0xffff, 0x00a8},
		{0x0c01, 0x0000, 0x00a9},
		{0x8603, 0x0000, 0x00ad},
		{0x0000, 0x0008, 0x00bc},
		{0x0000, 0x0020, 0x00c0},
		{0x3401, 0x0000, 0x00c4},
		{0x0000, 0x0009, 0x00c8},
		{0x0000, 0x0000, 0x00cc},
		{0x0004, 0x0000, 0x00d4},
		{0x1020, 0x0000, 0x00dd},
		{0x9003, 0x0000, 0x00e1},
		{0x6801, 0x0000, 0x00e5},
		{0x2903, 0x0000, 0x00e9},
		{0x9003, 0x0000, 0x00ed},
		{0x0c00, 0x0000, 0x00f1},
		{0x0c20, 0x0000, 0x00f5},
		{0x1001, 0x0000, 0x0104},
		{0x0005, 0x0020, 0x0108},
		{0x0015, 0x0000, 0x0114},
		{0x0000, 0x0000, 0x0118},
		{0x0800, 0x0000, 0x0125},
		{0x8c42, 0x0000, 0x0129},
		{0x0004, 0x0000, 0x012c},
		{0x1003, 0x0000, 0x012d},
		{0x0004, 0x0000, 0x0130},
		{0x1003, 0x0000, 0x0131},
		{0x8c42, 0x0000, 0x0135},
		{0x0004, 0x0000, 0x0138},
		{0x0c03, 0x0000, 0x0139},
		{0x0010, 0x0000, 0x014c},
		{0x0000, 0x0004, 0x0150},
		{0x0002, 0x0000, 0x0154},
		{0x0010, 0x0001, 0x0158},
		{0x0110, 0x0000, 0x015c},
		{0x1000, 0x4000, 0x016c},
		{0x0040, 0x0200, 0x016d},
		{0x0000, 0x0100, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x0800, 0x0000, 0x0170},
		{0x0010, 0x0000, 0x0171},
		{0x0000, 0x0900, 0x0172},
		{0x0081, 0x0000, 0x0173},
		{0x0000, 0x0000, 0x0174},
		{0x0010, 0x0000, 0x0175},
		{0x0000, 0x0900, 0x0176},
		{0x0081, 0x0000, 0x0177},
		{0x1800, 0x4000, 0x0178},
		{0x0040, 0x0200, 0x0179},
		{0x0000, 0x0100, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x2000, 0x4000, 0x017c},
		{0x0040, 0x0200, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x0800, 0x0000, 0x0180},
		{0x0010, 0x0000, 0x0181},
		{0x0000, 0x0900, 0x0182},
		{0x0001, 0x0000, 0x0183},
	};

	load_cfg((void*)cin, 0x20000, 642, 0, 0);
	load_data(din_addr[0], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x10000, 256, 1, 0, 0);
	load_data(din_addr[2], 0x14000, 256, 0, 0, 0);
	load_data(din_addr[3], 0x1a000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x16000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x12000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x0, 256, 0, 0, 0);
	config(0x0, 107, 0, 0);
	execute(0x3f63, 0, 0);
	store(dout_addr[0], 0x2000, 256, 0, 0);
	store(dout_addr[1], 0x8000, 256, 0, 0);
	store(dout_addr[2], 0xa000, 256, 0, 0);
}


/* Array initialization. */
void init_array()
{
    int i, j;
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


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
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

void result_check()
{
  int i, j;

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
  md_kernel();
  end = rdcycle();
  printf("It takes %d cycles for CPU to finish the task.\n", end - start);

  void* cgra_din_addr[7] = {NL, position_x, position_x, position_y, position_y, position_z, position_z};
  void* cgra_dout_addr[3] = {force_x, force_y, force_z};
  start = rdcycle();
  cgra_execute(cgra_din_addr, cgra_dout_addr);
  volatile int result = fence(1);
  end = rdcycle();
  printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

  result_check();
  printf("Done!\n");

  return 0;
}
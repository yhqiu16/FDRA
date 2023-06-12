#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
// #include "include/polybench.h"

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "include/encoding.h"
#include "include/ISA.h"

#define SIZE 1024

int acc_vector[SIZE] __attribute__((aligned(16)));
int res_vector[SIZE] __attribute__((aligned(16)));

int acceleration_x[SIZE] __attribute__((aligned(16)));
int acceleration_y[SIZE] __attribute__((aligned(16)));
int acceleration_z[SIZE] __attribute__((aligned(16)));
int acc_avg_x[SIZE] __attribute__((aligned(16)));
int acc_avg_y[SIZE] __attribute__((aligned(16)));
int acc_avg_z[SIZE] __attribute__((aligned(16)));

#define TYPE unsigned
// Problem Constants
#define nAtoms 64
#define maxNeighbors 16
// LJ coefficients
#define lj1 1 // 1.5
#define lj2 2 // 2.0

TYPE force_x[nAtoms] __attribute__((aligned(16)));
TYPE force_y[nAtoms] __attribute__((aligned(16)));
TYPE force_z[nAtoms] __attribute__((aligned(16)));
TYPE force_x0[nAtoms] __attribute__((aligned(16)));
TYPE force_y0[nAtoms] __attribute__((aligned(16)));
TYPE force_z0[nAtoms] __attribute__((aligned(16)));
TYPE position_x[nAtoms] __attribute__((aligned(16)));
TYPE position_y[nAtoms] __attribute__((aligned(16)));
TYPE position_z[nAtoms] __attribute__((aligned(16)));
unsigned NL[nAtoms * maxNeighbors] __attribute__((aligned(16)));

void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[158][3] __attribute__((aligned(16))) = {
		{0x1800, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0xb500, 0x0006},
		{0x0020, 0x0000, 0x0007},
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
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x2000, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x3000, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x9100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0b00, 0x0022},
		{0x0021, 0x0000, 0x0023},
		{0x2000, 0x0000, 0x0024},
		{0x0010, 0x0000, 0x0025},
		{0x0000, 0x0100, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x0402, 0x0000, 0x0030},
		{0x0000, 0x0001, 0x0034},
		{0x0100, 0x0000, 0x0038},
		{0x0010, 0x0000, 0x0040},
		{0x0200, 0x0034, 0x0044},
		{0x8200, 0x0000, 0x0048},
		{0x0003, 0x0000, 0x004c},
		{0x100d, 0x0000, 0x0055},
		{0x0000, 0x8001, 0x0056},
		{0x0041, 0x0010, 0x0057},
		{0x3082, 0x0000, 0x0059},
		{0x100d, 0x0000, 0x005d},
		{0x0000, 0x9001, 0x005e},
		{0x0041, 0x0010, 0x005f},
		{0x2402, 0x0000, 0x0061},
		{0x7001, 0x0000, 0x0065},
		{0x0c10, 0x0000, 0x0069},
		{0x4482, 0x0000, 0x006d},
		{0x0440, 0x0000, 0x0071},
		{0x1040, 0x0000, 0x0075},
		{0x0000, 0x0000, 0x007c},
		{0x0501, 0x0000, 0x0080},
		{0x0003, 0x0040, 0x0084},
		{0xc501, 0x0080, 0x0088},
		{0x0080, 0x0043, 0x008c},
		{0x0830, 0x0003, 0x0090},
		{0x3000, 0x0604, 0x0094},
		{0x0000, 0x0201, 0x0098},
		{0x0000, 0x0220, 0x009c},
		{0x0000, 0x0000, 0x00a0},
		{0x7203, 0x0000, 0x00a5},
		{0x2403, 0x0000, 0x00a9},
		{0x4e03, 0x0000, 0x00ad},
		{0x2403, 0x0000, 0x00b1},
		{0x6c03, 0x0000, 0x00b5},
		{0x2801, 0x0000, 0x00b9},
		{0x9011, 0x0000, 0x00bd},
		{0x2403, 0x0000, 0x00c1},
		{0x0c00, 0x0000, 0x00c5},
		{0x0020, 0x0000, 0x00cc},
		{0x1003, 0x0900, 0x00d0},
		{0x0020, 0x0023, 0x00d4},
		{0x4000, 0x0003, 0x00d8},
		{0x4050, 0x0203, 0x00dc},
		{0x3000, 0x0021, 0x00e0},
		{0x8000, 0x0018, 0x00e4},
		{0x0004, 0x0902, 0x00e8},
		{0x0020, 0x0002, 0x00ec},
		{0x080d, 0x0000, 0x00f5},
		{0x0000, 0x8001, 0x00f6},
		{0x0041, 0x0010, 0x00f7},
		{0x9203, 0x0000, 0x00f9},
		{0x5103, 0x0000, 0x00fd},
		{0x000a, 0x0000, 0x0100},
		{0x4004, 0x0000, 0x0101},
		{0x8811, 0x0000, 0x0105},
		{0x4c42, 0x0000, 0x0109},
		{0x6c03, 0x0000, 0x010d},
		{0x6c03, 0x0000, 0x0111},
		{0x0004, 0x0000, 0x0114},
		{0x0c03, 0x0000, 0x0115},
		{0x0020, 0x0000, 0x011c},
		{0x2000, 0x0000, 0x0120},
		{0x4009, 0x0020, 0x0124},
		{0x1400, 0x0003, 0x0128},
		{0x2000, 0x0003, 0x012c},
		{0x0034, 0x0003, 0x0130},
		{0x0050, 0x0003, 0x0134},
		{0x0050, 0x080a, 0x0138},
		{0x0030, 0x0001, 0x013c},
		{0x0830, 0x0000, 0x0145},
		{0x4803, 0x0000, 0x0149},
		{0xf830, 0xffff, 0x014c},
		{0x0801, 0x0000, 0x014d},
		{0x2883, 0x0000, 0x0151},
		{0x4803, 0x0000, 0x0155},
		{0x9042, 0x0000, 0x0159},
		{0x8c32, 0x0000, 0x015d},
		{0x0004, 0x0000, 0x0160},
		{0x1003, 0x0000, 0x0161},
		{0x0004, 0x0000, 0x0164},
		{0x0c03, 0x0000, 0x0165},
		{0x0001, 0x0000, 0x016c},
		{0x0300, 0x0000, 0x0170},
		{0x0000, 0x0000, 0x0174},
		{0x0000, 0x0010, 0x017c},
		{0x0000, 0x0024, 0x0180},
		{0x0021, 0x0000, 0x0184},
		{0x2008, 0x0000, 0x0188},
		{0x0011, 0x0005, 0x018c},
		{0x1800, 0x4000, 0x0194},
		{0x0040, 0x0200, 0x0195},
		{0x0000, 0xb700, 0x0196},
		{0x0000, 0x0000, 0x0197},
		{0x1000, 0x4000, 0x0198},
		{0x0040, 0x0200, 0x0199},
		{0x0000, 0xb300, 0x019a},
		{0x0000, 0x0000, 0x019b},
		{0x2800, 0x0000, 0x019c},
		{0x0010, 0x0000, 0x019d},
		{0x0000, 0x0100, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x1000, 0x4000, 0x01a0},
		{0x0040, 0x0200, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x1800, 0x4000, 0x01a4},
		{0x0040, 0x0200, 0x01a5},
		{0x0000, 0x0500, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x0000, 0x4000, 0x01a8},
		{0x0040, 0x0200, 0x01a9},
		{0x0000, 0x0300, 0x01aa},
		{0x0000, 0x0000, 0x01ab},
		{0x0800, 0x0000, 0x01ac},
		{0x0010, 0x0000, 0x01ad},
		{0x0000, 0x0b00, 0x01ae},
		{0x0021, 0x0000, 0x01af},
		{0x2000, 0x4000, 0x01b0},
		{0x0040, 0x0200, 0x01b1},
		{0x0000, 0x0100, 0x01b2},
		{0x0000, 0x0000, 0x01b3},
		{0x0800, 0x0000, 0x01b4},
		{0x0010, 0x0000, 0x01b5},
		{0x0000, 0x0900, 0x01b6},
		{0x0001, 0x0000, 0x01b7},
	};

	load_cfg((void*)cin, 0x24000, 948, 0, 0);
	load_data(din_addr[0], 0x20000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x1a000, 256, 1, 0, 0);
	load_data(din_addr[2], 0x18000, 256, 0, 0, 0);
	load_data(din_addr[3], 0x22000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x1c000, 256, 0, 0, 0);
	load_data(din_addr[5], 0xa000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x1e000, 256, 0, 0, 0);
	load_data(din_addr[7], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0x2000, 4096, 0, 0, 0);
	load_data(din_addr[10], 0x4000, 4096, 0, 0, 0);
	load_data(din_addr[11], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0x12000, 4096, 0, 0, 0);
	config(0x0, 158, 0, 0);
	execute(0x3ffbf, 0, 0);
	store(dout_addr[0], 0xc000, 4096, 0, 0);
	store(dout_addr[1], 0x6000, 256, 0, 0);
	store(dout_addr[2], 0x14000, 256, 0, 0);
	store(dout_addr[3], 0x16000, 256, 0, 0);
}


/* Array initialization. */
void init_array()
{
	int i, j;

	for(int i=0;i<SIZE;i++){
    	acceleration_x[i] = 3*i;
    	acceleration_y[i] = 3*i+1;
    	acceleration_z[i] = 3*i+2;

    	acc_avg_x[i] = i;
    	acc_avg_y[i] = i;
    	acc_avg_z[i] = i;
 	}
	for (i = 0; i < nAtoms; i++)
	{
		position_x[i] = i;
		position_y[i] = i + 1;
		position_z[i] = i + 2;
		//        force_x[i] = 0;
		//        force_y[i] = 0;
		//        force_z[i] = 0;
	}
	for (i = 0; i < nAtoms; i++)
	{
		for (j = 0; j < maxNeighbors; ++j)
		{
			NL[i * maxNeighbors + j] = (i + j + 3) % nAtoms;
		}
	}
}

__attribute__((noinline))
void pedometer(){
	for(int i=0;i<SIZE;i++){
		res_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}

__attribute__((noinline)) void md_kernel()
{
	TYPE delx, dely, delz, r2inv;
	TYPE r6inv, potential, force, j_x, j_y, j_z;
	TYPE i_x, i_y, i_z, fx, fy, fz;

	int32_t i, j, jidx;

loop_i:
	for (i = 0; i < nAtoms; i++)
	{
		i_x = position_x[i];
		i_y = position_y[i];
		i_z = position_z[i];
		fx = 0;
		fy = 0;
		fz = 0;
	loop_j:
		for (j = 0; j < maxNeighbors; j++)
		{
			//	please_map_me();
			// Get neighbor
			jidx = NL[i * maxNeighbors + j];
			// Look up x,y,z positions
			j_x = position_x[jidx];
			j_y = position_y[jidx];
			j_z = position_z[jidx];
			// Calc distance
			delx = i_x - j_x;
			dely = i_y - j_y;
			delz = i_z - j_z;
			r2inv = 10 / (delx * delx + dely * dely + delz * delz); // r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
			// Assume no cutoff and aways account for all nodes in area
			r6inv = r2inv * r2inv * r2inv;
			potential = r6inv * (lj1 * r6inv - lj2 * 1000);
			// Sum changes in force
			force = r2inv * potential;
			/*potential = r6inv/100000000*(lj1*r6inv - lj2*1000000000);
			// Sum changes in force
			force = r2inv*potential/10000000000;*/
			fx += delx * force;
			fy += dely * force;
			fz += delz * force;
		}
		// Update forces after all neighbors accounted for.
		force_x0[i] = fx;
		force_y0[i] = fy;
		force_z0[i] = fz;
		// printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
	}
}

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
			printf("There is an error in location (%d)\n", i);
		//          printf("There is an error in location (%d)[%d, %d, %d; %d, %d, %d]\n",
		//              i, force_x0[i], force_x[i], force_y0[i], force_y[i], force_z0[i], force_z[i]);
	}
}

int main(int argc, char **argv)
{
	long long unsigned start;
	long long unsigned end;
	init_array();
	printf("Initialization finished!\n");

	start = rdcycle();
	pedometer();
	md_kernel();
	end = rdcycle();
	printf("It takes %d cycles for CPU to finish the task.\n", end - start);

	void *cgra_din_addr[13] = {NL, position_x, position_x, position_y, position_y, position_z, position_z, acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
	void *cgra_dout_addr[4] = {acc_vector, force_x, force_y, force_z};
	start = rdcycle();
	cgra_execute(cgra_din_addr, cgra_dout_addr);
	volatile int result = fence(1);
	end = rdcycle();
	printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

	result_check();
	printf("Done!\n");

	return 0;
}
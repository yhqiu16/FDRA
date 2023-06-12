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

#define TYPE unsigned

// Problem Constants
#define nAtoms 16
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
	static unsigned short cin[98][3] __attribute__((aligned(16))) = {
		{0x0200, 0x0800, 0x0004},
		{0x0008, 0x0010, 0x0005},
		{0x0000, 0x0020, 0x0006},
		{0x0300, 0x0800, 0x0008},
		{0x0008, 0x0010, 0x0009},
		{0x0000, 0x0020, 0x000a},
		{0x0100, 0x0800, 0x000c},
		{0x0008, 0x0010, 0x000d},
		{0x0000, 0x0020, 0x000e},
		{0x0000, 0x8000, 0x0010},
		{0x0000, 0x0000, 0x0011},
		{0x0000, 0x2120, 0x0012},
		{0x0004, 0x0000, 0x0013},
		{0x0400, 0x0800, 0x0014},
		{0x0008, 0x0010, 0x0015},
		{0x0000, 0x0020, 0x0016},
		{0x0100, 0x8000, 0x0018},
		{0x0000, 0x0000, 0x0019},
		{0x0000, 0x2120, 0x001a},
		{0x0004, 0x0000, 0x001b},
		{0x0200, 0x0800, 0x001c},
		{0x0008, 0x0010, 0x001d},
		{0x0000, 0x1660, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x0300, 0x0800, 0x0020},
		{0x0008, 0x0010, 0x0021},
		{0x0000, 0x1660, 0x0022},
		{0x0004, 0x0000, 0x0023},
		{0x0000, 0x0010, 0x002c},
		{0x5200, 0x0010, 0x0030},
		{0x8000, 0x0014, 0x0034},
		{0x0100, 0x000a, 0x0038},
		{0x0000, 0x0008, 0x003c},
		{0x0831, 0x0000, 0x0040},
		{0x0000, 0x0000, 0x0048},
		{0x2003, 0x0001, 0x0051},
		{0x1042, 0x0001, 0x0055},
		{0x0004, 0x0000, 0x0058},
		{0x2003, 0x0000, 0x0059},
		{0x0004, 0x0000, 0x005c},
		{0x1003, 0x0000, 0x005d},
		{0x0004, 0x0000, 0x0060},
		{0x1003, 0x0000, 0x0061},
		{0xbb03, 0x0000, 0x0065},
		{0x400d, 0x0000, 0x0069},
		{0x0000, 0x0004, 0x006a},
		{0x0106, 0x0010, 0x006b},
		{0x0000, 0x00c0, 0x0074},
		{0xe200, 0x0040, 0x0078},
		{0x0000, 0x0282, 0x007c},
		{0x0000, 0x0080, 0x0080},
		{0x0000, 0x0400, 0x0084},
		{0x0030, 0x0100, 0x0088},
		{0x0200, 0x0000, 0x008c},
		{0x0000, 0x0000, 0x0090},
		{0x2001, 0x0002, 0x0095},
		{0x2052, 0x0001, 0x0099},
		{0x9052, 0x0001, 0x009d},
		{0x4203, 0x0002, 0x00a1},
		{0xb003, 0x0001, 0x00a5},
		{0xf830, 0xffff, 0x00a8},
		{0x3001, 0x0000, 0x00a9},
		{0x400d, 0x0000, 0x00ad},
		{0x0000, 0xc004, 0x00ae},
		{0x0105, 0x0010, 0x00af},
		{0x9a03, 0x0001, 0x00b1},
		{0x0004, 0x0000, 0x00bc},
		{0x45c0, 0x0040, 0x00c0},
		{0x0800, 0x0600, 0x00c4},
		{0x0154, 0x0400, 0x00c8},
		{0x0610, 0x00c0, 0x00cc},
		{0x0800, 0x0108, 0x00d0},
		{0x0381, 0x0000, 0x00d4},
		{0x2003, 0x0001, 0x00dd},
		{0x1001, 0x0001, 0x00e1},
		{0x9003, 0x0000, 0x00e5},
		{0x000a, 0x0000, 0x00e8},
		{0x8004, 0x0000, 0x00e9},
		{0x9103, 0x0001, 0x00ed},
		{0x9003, 0x0000, 0x00f1},
		{0xba03, 0x0000, 0x00f5},
		{0x100d, 0x0000, 0x00f9},
		{0x0000, 0xc004, 0x00fa},
		{0x0105, 0x0010, 0x00fb},
		{0x0000, 0x0000, 0x0108},
		{0x2000, 0x0000, 0x010c},
		{0x0000, 0x0030, 0x0110},
		{0x0000, 0x0020, 0x0114},
		{0x0000, 0x0008, 0x0118},
		{0x0100, 0x0000, 0x011c},
		{0x0000, 0x8000, 0x012c},
		{0x0000, 0x0000, 0x012d},
		{0x0000, 0x2160, 0x012e},
		{0x0004, 0x0000, 0x012f},
		{0x0000, 0x0800, 0x0140},
		{0x0008, 0x0010, 0x0141},
		{0x0000, 0x1620, 0x0142},
		{0x0000, 0x0000, 0x0143},
	};

	load_cfg((void*)cin, 0x4000, 588, 0, 0);
	load_data(din_addr[0], 0x1000, 1024, 0, 0, 0);
	load_data(din_addr[1], 0x0, 64, 1, 0, 0);
	load_data(din_addr[2], 0x400, 64, 0, 0, 0);
	load_data(din_addr[3], 0x1400, 64, 1, 0, 0);
	load_data(din_addr[4], 0x800, 64, 0, 0, 0);
	load_data(din_addr[5], 0x2000, 64, 1, 0, 0);
	load_data(din_addr[6], 0xc00, 64, 0, 0, 0);
	config(0x0, 98, 0, 0);
	execute(0x84ff, 0, 0);
	store(dout_addr[0], 0x1800, 64, 0, 0);
	store(dout_addr[1], 0x3000, 64, 0, 0);
	store(dout_addr[2], 0x1c00, 64, 0, 0);
}


/* Array initialization. */
void init_array()
{
	int i, j;
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

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
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

	for (i = 0; i < nAtoms; i++)
	{
		if (force_x0[i] != force_x[i] || force_y0[i] != force_y[i] || force_z0[i] != force_z[i])
			printf("There is an error in location (%d)[%d, %d, %d; %d, %d, %d]\n",
				   i, force_x0[i], force_x[i], force_y0[i], force_y[i], force_z0[i], force_z[i]);
	}
}

int main(int argc, char **argv)
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

	void *cgra_din_addr[7] = {NL, position_x, position_x, position_y, position_y, position_z, position_z};
	void *cgra_dout_addr[3] = {force_x, force_y, force_z};
	start = rdcycle();
	cgra_execute(cgra_din_addr, cgra_dout_addr);
	volatile int result = fence(1);
	end = rdcycle();
	printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

	result_check();
	printf("Done!\n");

	return 0;
}
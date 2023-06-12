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
#define col_size 32
#define row_size 32
#define f_size 9

TYPE orig[row_size * col_size] __attribute__((aligned(16)));
TYPE sol[row_size * col_size] __attribute__((aligned(16)));
TYPE filter[f_size] __attribute__((aligned(16)));
TYPE res[row_size * col_size] __attribute__((aligned(16)));

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

void cgra_execute(void **din_addr, void **dout_addr)
{
	static unsigned short cin[154][3] __attribute__((aligned(16))) = {
		{0x1000, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0xb700, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x1800, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0xb500, 0x000a},
		{0x0020, 0x0000, 0x000b},
		{0x0800, 0x4000, 0x000c},
		{0x0040, 0x0200, 0x000d},
		{0x0000, 0x0300, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x0000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0b00, 0x0012},
		{0x0021, 0x0000, 0x0013},
		{0x1000, 0x4000, 0x0014},
		{0x0040, 0x0200, 0x0015},
		{0x0000, 0x0300, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2000, 0x4000, 0x0018},
		{0x0040, 0x0200, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x1800, 0x0c00, 0x001c},
		{0x0040, 0x80f0, 0x001d},
		{0xe001, 0x9101, 0x001e},
		{0x0020, 0x0000, 0x001f},
		{0x0802, 0x0c04, 0x0020},
		{0xf040, 0x80f7, 0x0021},
		{0xefe1, 0x0101, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x0800, 0x4000, 0x0024},
		{0x0040, 0x0200, 0x0025},
		{0x0000, 0x0500, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x1001, 0x0010, 0x0034},
		{0x0002, 0x0000, 0x003c},
		{0x0800, 0x0002, 0x0040},
		{0x0000, 0x0005, 0x0044},
		{0x0203, 0x0010, 0x0048},
		{0x0100, 0x0000, 0x004c},
		{0x080d, 0x0000, 0x0059},
		{0x0000, 0x9001, 0x005a},
		{0x0041, 0x0010, 0x005b},
		{0x9203, 0x0000, 0x005d},
		{0xf830, 0xffff, 0x0060},
		{0x1001, 0x0000, 0x0061},
		{0x0004, 0x0000, 0x0064},
		{0x0803, 0x0000, 0x0065},
		{0x2442, 0x0000, 0x0069},
		{0x0004, 0x0000, 0x006c},
		{0x0403, 0x0000, 0x006d},
		{0x0430, 0x0000, 0x0071},
		{0x6442, 0x0000, 0x0075},
		{0x0000, 0x0000, 0x007c},
		{0x1000, 0x0000, 0x0080},
		{0x1000, 0x0600, 0x0084},
		{0x000d, 0x0800, 0x0088},
		{0xc000, 0x0000, 0x008c},
		{0x0500, 0x0001, 0x0090},
		{0x0300, 0x0000, 0x0094},
		{0x0000, 0x0600, 0x0098},
		{0x05c0, 0x0000, 0x009c},
		{0x0000, 0x0000, 0x00a0},
		{0x080d, 0x0000, 0x00a5},
		{0x0000, 0x9001, 0x00a6},
		{0x0041, 0x0010, 0x00a7},
		{0x8a03, 0x0000, 0x00a9},
		{0x5003, 0x0000, 0x00ad},
		{0x4c81, 0x0000, 0x00b1},
		{0x8d03, 0x0000, 0x00b5},
		{0x2403, 0x0000, 0x00b9},
		{0x0004, 0x0000, 0x00bc},
		{0x0403, 0x0000, 0x00bd},
		{0x100d, 0x0000, 0x00c1},
		{0x0000, 0x7001, 0x00c2},
		{0x000c, 0x00e1, 0x00c3},
		{0x2403, 0x0000, 0x00c5},
		{0x0004, 0x0600, 0x00d4},
		{0x0010, 0x0620, 0x00d8},
		{0xc030, 0x0400, 0x00dc},
		{0xc000, 0x0001, 0x00e0},
		{0x0000, 0x0003, 0x00e4},
		{0x2000, 0x0003, 0x00e8},
		{0x0301, 0x0020, 0x00ec},
		{0x0040, 0x0000, 0x00f0},
		{0x1040, 0x0000, 0x00f5},
		{0x0c40, 0x0000, 0x00f9},
		{0x6c03, 0x0000, 0x00fd},
		{0x4903, 0x0000, 0x0101},
		{0x4401, 0x0000, 0x0105},
		{0x000a, 0x0000, 0x0108},
		{0x6004, 0x0000, 0x0109},
		{0x6803, 0x0000, 0x010d},
		{0x7021, 0x0000, 0x0111},
		{0x0440, 0x0000, 0x0115},
		{0x0000, 0x0000, 0x0120},
		{0x2050, 0x0000, 0x0124},
		{0x1000, 0x0620, 0x0128},
		{0xe000, 0x0100, 0x012c},
		{0x0000, 0x0083, 0x0130},
		{0x0480, 0x0003, 0x0134},
		{0x0040, 0x0603, 0x0138},
		{0x0001, 0x0020, 0x013c},
		{0x8842, 0x0000, 0x0149},
		{0x080d, 0x0000, 0x014d},
		{0x0000, 0xa001, 0x014e},
		{0x0041, 0x0010, 0x014f},
		{0x4a03, 0x0000, 0x0151},
		{0x4803, 0x0000, 0x0155},
		{0x8c03, 0x0000, 0x0159},
		{0x2c11, 0x0000, 0x015d},
		{0x7003, 0x0000, 0x0161},
		{0x0840, 0x0000, 0x0165},
		{0x0000, 0x0000, 0x0174},
		{0x2000, 0x000c, 0x0178},
		{0x0000, 0x0007, 0x017c},
		{0x0010, 0x0003, 0x0180},
		{0x0004, 0x0003, 0x0184},
		{0x0340, 0x0003, 0x0188},
		{0x0000, 0x0004, 0x018c},
		{0x1000, 0x4000, 0x0198},
		{0x0040, 0x0200, 0x0199},
		{0x0000, 0xb700, 0x019a},
		{0x0020, 0x0000, 0x019b},
		{0x0000, 0x0000, 0x019c},
		{0x0010, 0x0000, 0x019d},
		{0x0000, 0x0d00, 0x019e},
		{0x0021, 0x0000, 0x019f},
		{0x6802, 0x0c00, 0x01a0},
		{0xfe80, 0x00f7, 0x01a1},
		{0xeffd, 0x0101, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x0000, 0x0c04, 0x01a4},
		{0xf040, 0x80f7, 0x01a5},
		{0xefe1, 0x0101, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x7800, 0x0c00, 0x01a8},
		{0xfe80, 0x00f7, 0x01a9},
		{0xeffd, 0x0101, 0x01aa},
		{0x0000, 0x0000, 0x01ab},
		{0x7001, 0x0c00, 0x01ac},
		{0xfe80, 0x00f7, 0x01ad},
		{0xeffd, 0x0101, 0x01ae},
		{0x0000, 0x0000, 0x01af},
		{0x0800, 0x0000, 0x01b0},
		{0x0010, 0x0000, 0x01b1},
		{0x0000, 0x0b00, 0x01b2},
		{0x0001, 0x0000, 0x01b3},
		{0x0001, 0x0c04, 0x01b4},
		{0xf040, 0x80f7, 0x01b5},
		{0xefe1, 0x0101, 0x01b6},
		{0x0000, 0x0000, 0x01b7},
	};

	load_cfg((void *)cin, 0x24000, 924, 0, 0);
	load_data(din_addr[0], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x0, 256, 1, 0, 0);
	load_data(din_addr[2], 0xc000, 256, 0, 0, 0);
	load_data(din_addr[3], 0x10000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x2000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x22000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x12000, 256, 0, 0, 0);
	load_data(din_addr[7], 0x1e000, 36, 1, 0, 0);
	load_data(din_addr[8], 0x1c000, 36, 1, 0, 0);
	load_data(din_addr[9], 0x1a000, 36, 0, 0, 0);
	load_data(din_addr[10], 0x18000, 4096, 1, 0, 0);
	load_data(din_addr[11], 0x20000, 4096, 1, 0, 0);
	load_data(din_addr[12], 0xa000, 4096, 0, 0, 0);
	config(0x0, 154, 0, 0);
	execute(0x3fdff, 0, 0);
	store(dout_addr[0], 0xe000, 3832, 0, 0);
	store(dout_addr[1], 0x14000, 256, 0, 0);
	store(dout_addr[2], 0x4000, 256, 0, 0);
	store(dout_addr[3], 0x6000, 256, 0, 0);
}

/* Array initialization. */
void init_array()
{
	int i, j;

	for (i = 0; i < f_size; i++)
	{
		filter[i] = i;
	}
	for (i = 0; i < row_size * col_size; i++)
	{
		orig[i] = i;
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

__attribute__((noinline)) void stencil2d()
{
	int r, c, k1, k2;
	TYPE temp, mul;

	for (r = 0; r < row_size - 2; r++)
	{
		for (c = 0; c < col_size - 2; c++)
		{
			temp = (TYPE)0;
			for (k1 = 0; k1 < 3; k1++)
			{
				for (k2 = 0; k2 < 3; k2++)
				{
					mul = filter[k1 * 3 + k2] * orig[(r + k1) * col_size + c + k2];
					temp += mul;
				}
			}
			res[(r * col_size) + c] = temp;
		}
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
	int i, j, r, c;

	for (r = 0; r < row_size - 2; r++)
	{
		for (c = 0; c < col_size - 2; c++)
		{
			i = r * col_size + c;
			if (res[i] != sol[i])
				printf("There is an error in location (%d)[%d, %d]\n", i, res[i], sol[i]);
		}
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
	stencil2d();
	md_kernel();
	end = rdcycle();
	printf("It takes %d cycles for CPU to finish the task.\n", end - start);

	void *cgra_din_addr[13] = {NL, position_x, position_x, position_y, position_y, position_z, position_z, filter, filter, filter, orig, orig, orig};
	void *cgra_dout_addr[4] = {sol, force_x, force_y, force_z};
	start = rdcycle();
	cgra_execute(cgra_din_addr, cgra_dout_addr);
	volatile int result = fence(1);
	end = rdcycle();
	printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

	result_check();
	printf("Done!\n");

	return 0;
}
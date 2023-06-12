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

#define NDATA 1024
#define NTAPS 8

int input[NDATA] __attribute__((aligned(16)));
int output[NDATA] __attribute__((aligned(16)));
int res[NDATA] __attribute__((aligned(16)));
int coefficients[NTAPS] __attribute__((aligned(16))) = {
	025, 150, 375, -225, 050, 075, -300, 125};

// Specify row/column sizes
#define row_size 32
#define col_size 32
#define N (row_size * col_size)

TYPE m1[N] __attribute__((aligned(16)));
TYPE m2[N] __attribute__((aligned(16)));
TYPE prod0[N] __attribute__((aligned(16)));
TYPE prod[N] __attribute__((aligned(16)));

void cgra_execute(void **din_addr, void **dout_addr)
{
	static unsigned short cin[125][3] __attribute__((aligned(16))) = {
		{0x0840, 0x2010, 0x0004},
		{0x2040, 0x8107, 0x0005},
		{0x0e30, 0x0102, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x8002, 0x2000, 0x0008},
		{0xf900, 0x0107, 0x0009},
		{0x0002, 0x0102, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x9801, 0x0800, 0x000c},
		{0xff00, 0x1fcf, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x1060, 0x2010, 0x0010},
		{0x2040, 0x8107, 0x0011},
		{0x0e30, 0x0102, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x1803, 0x0800, 0x0014},
		{0x0040, 0x1fc8, 0x0015},
		{0x0000, 0x9700, 0x0016},
		{0x0020, 0x0000, 0x0017},
		{0x0800, 0x2010, 0x0018},
		{0x2040, 0x8107, 0x0019},
		{0x0e30, 0x0102, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x9003, 0x0800, 0x001c},
		{0xff00, 0x1fcf, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x8000, 0x2000, 0x0020},
		{0xf900, 0x0107, 0x0021},
		{0x0002, 0x0102, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x9001, 0x2000, 0x0024},
		{0xf900, 0x0107, 0x0025},
		{0x0002, 0x0102, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x0400, 0x0000, 0x0030},
		{0x0000, 0x0030, 0x0038},
		{0x0000, 0x0002, 0x003c},
		{0x0003, 0x0002, 0x0040},
		{0x1000, 0x0000, 0x0044},
		{0x0000, 0x0030, 0x0048},
		{0x0000, 0x0000, 0x004c},
		{0x2403, 0x0000, 0x0059},
		{0x8423, 0x0000, 0x0061},
		{0x6811, 0x0000, 0x0069},
		{0x4403, 0x0000, 0x006d},
		{0x0000, 0x0000, 0x0084},
		{0x0100, 0x0600, 0x0088},
		{0x0308, 0x0000, 0x008c},
		{0x0040, 0x0004, 0x0090},
		{0x0000, 0x0000, 0x0094},
		{0x0000, 0x0600, 0x0098},
		{0x8441, 0x0000, 0x00ad},
		{0x6421, 0x0000, 0x00b1},
		{0x6413, 0x0000, 0x00b5},
		{0x0c0d, 0x0000, 0x00b9},
		{0x0000, 0xa001, 0x00ba},
		{0x4008, 0x00fe, 0x00bb},
		{0x3001, 0x0000, 0x00bd},
		{0x20c8, 0x0000, 0x00d8},
		{0x00c0, 0x0003, 0x00dc},
		{0x0010, 0x0023, 0x00e0},
		{0x0000, 0x0000, 0x00e4},
		{0x8001, 0x0000, 0x00e8},
		{0x6883, 0x0000, 0x00fd},
		{0x040d, 0x0000, 0x0101},
		{0x0000, 0x9001, 0x0102},
		{0x0020, 0x0100, 0x0103},
		{0x2c41, 0x0000, 0x0105},
		{0x4c83, 0x0000, 0x010d},
		{0x0000, 0x0100, 0x0120},
		{0x0480, 0x0100, 0x0124},
		{0x3000, 0x0088, 0x0128},
		{0x0010, 0x00a1, 0x012c},
		{0x3000, 0x00a0, 0x0130},
		{0x0020, 0x0003, 0x0134},
		{0x0000, 0x0004, 0x0138},
		{0x2881, 0x0000, 0x014d},
		{0x7083, 0x0000, 0x0151},
		{0x6903, 0x0000, 0x0155},
		{0x0000, 0x0004, 0x0170},
		{0x0000, 0x0004, 0x0174},
		{0x0040, 0x0000, 0x0178},
		{0x0042, 0x0030, 0x017c},
		{0x0000, 0x0023, 0x0180},
		{0x0000, 0x0021, 0x0184},
		{0x0000, 0x0020, 0x0188},
		{0x0200, 0x0000, 0x018c},
		{0x8804, 0x0bff, 0x0194},
		{0x0140, 0x1fc8, 0x0195},
		{0x0000, 0x0100, 0x0196},
		{0x0000, 0x0000, 0x0197},
		{0x1820, 0x2010, 0x0198},
		{0x2040, 0x8107, 0x0199},
		{0x0e30, 0x0102, 0x019a},
		{0x0000, 0x0000, 0x019b},
		{0x8005, 0x0bff, 0x019c},
		{0x0140, 0x1fc8, 0x019d},
		{0x0000, 0x0100, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x8807, 0x0bff, 0x01a0},
		{0x0140, 0x1fc8, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x9003, 0x2000, 0x01a4},
		{0xf900, 0x0107, 0x01a5},
		{0x0002, 0x0102, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x8006, 0x0bff, 0x01a8},
		{0x0140, 0x1fc8, 0x01a9},
		{0x0000, 0x0100, 0x01aa},
		{0x0000, 0x0000, 0x01ab},
		{0x9802, 0x0800, 0x01ac},
		{0xff00, 0x1fcf, 0x01ad},
		{0x0000, 0x0100, 0x01ae},
		{0x0000, 0x0000, 0x01af},
		{0x8000, 0x0800, 0x01b0},
		{0xff00, 0x1fcf, 0x01b1},
		{0x0000, 0x0100, 0x01b2},
		{0x0000, 0x0000, 0x01b3},
		{0x0800, 0x2000, 0x01b4},
		{0x0040, 0x8100, 0x01b5},
		{0x0000, 0x9b02, 0x01b6},
		{0x0000, 0x0000, 0x01b7},
	};

	load_cfg((void *)cin, 0x24000, 750, 0, 0);
	load_data(din_addr[0], 0x20000, 32, 1, 0, 0);
	load_data(din_addr[1], 0x6000, 32, 1, 0, 0);
	load_data(din_addr[2], 0x1e000, 32, 1, 0, 0);
	load_data(din_addr[3], 0xc000, 32, 0, 0, 0);
	load_data(din_addr[4], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x18000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x10000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x12000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x8000, 4096, 1, 0, 0);
	load_data(din_addr[9], 0x14000, 4096, 1, 0, 0);
	load_data(din_addr[10], 0x0, 4096, 1, 0, 0);
	load_data(din_addr[11], 0x1c000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0xa000, 4096, 1, 0, 0);
	load_data(din_addr[13], 0x16000, 4096, 1, 0, 0);
	load_data(din_addr[14], 0x2000, 4096, 1, 0, 0);
	load_data(din_addr[15], 0x4000, 4096, 0, 0, 0);
	config(0x0, 125, 0, 0);
	execute(0x3ffff, 0, 0);
	store(dout_addr[0], 0xe000, 4080, 0, 0);
	store(dout_addr[1], 0x22000, 4096, 0, 0);
}

/* Array initialization. */
static void init_array()
{
	int i, j;
	for (i = 0; i < NTAPS; i++)
	{
		input[i] = i;
		output[i] = 0;
		res[i] = 0;
	}
	for (int i = 0; i < N; i++)
	{
		m1[i] = i;
		m2[i] = i + 1;
	}
}

__attribute__((noinline)) void fir() //(int input[], int output[], int coefficients[])
/*   input :           input sample array */
/*   output:           output sample array */
/*   coefficient:      coefficient array */
{
	int i, j;
	for (j = NTAPS - 1; j < NDATA; ++j)
	{
		int sum = 0;
#pragma unroll 4
		for (i = 0; i < NTAPS; ++i)
		{
#ifdef CGRA_COMPILER
			please_map_me();
#endif
			sum += input[j - i] * coefficients[i];
		}
		res[j] = sum;
	}
}

__attribute__((noinline)) void gemm()
{
	int i, j, k;
	int k_col, i_col;
	TYPE mult;

outer:
	for (i = 0; i < row_size; i++)
	{
	middle:
		for (j = 0; j < col_size; j++)
		{
			i_col = i * col_size;
			TYPE sum = 0;
		inner:
			for (k = 0; k < row_size; k++)
			{
				k_col = k * col_size;
				mult = m1[i_col + k] * m2[k_col + j];
				sum += mult;
			}
			prod0[i_col + j] = sum;
		}
	}
}

void result_check()
{
	int i, j;

	for (i = NTAPS - 1; i < NDATA; ++i)
	{
		if (res[i] != output[i])
			printf("There is an error in location (%d)[%d, %d]\n", i, res[i], output[i]);
	}
	for (int i = 0; i < N; i++)
	{
		if (prod[i] != prod0[i])
		{
			printf("%x, %x\n", prod0[i], prod[i]);
		}
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
	fir();
	gemm();
	end = rdcycle();
	printf("It takes %d cycles for CPU to finish the task.\n", end - start);

	void *cgra_din_addr[16] = {coefficients, coefficients, coefficients, coefficients, input, input, input, input, m1, m1, m1, m1, m2, m2, m2, m2};
	void *cgra_dout_addr[2] = {(void *)output + 16, prod};
	start = rdcycle();
	cgra_execute(cgra_din_addr, cgra_dout_addr);
	volatile int result = fence(1);
	end = rdcycle();
	printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

	result_check();
	printf("Done!\n");

	return 0;
}
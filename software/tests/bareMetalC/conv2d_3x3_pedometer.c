#include "include/encoding.h"
#include "include/ISA.h"

/* Problem size */
#define NI 32
#define NJ 32
#define SIZE NI *NJ
#define c11 2
#define c12 -3
#define c21 5
#define c22 6

/*For 3x3 filter*/
#define c13 4
#define c23 7
#define c31 -8
#define c32 -9
#define c33 10

int A[SIZE] __attribute__((aligned(16)));
int B[SIZE] __attribute__((aligned(16)));
int C[SIZE] __attribute__((aligned(16)));

int acc_vector[SIZE] __attribute__((aligned(16)));
int res_vector[SIZE] __attribute__((aligned(16)));

int acceleration_x[SIZE] __attribute__((aligned(16)));
int acceleration_y[SIZE] __attribute__((aligned(16)));
int acceleration_z[SIZE] __attribute__((aligned(16)));
int acc_avg_x[SIZE] __attribute__((aligned(16)));
int acc_avg_y[SIZE] __attribute__((aligned(16)));
int acc_avg_z[SIZE] __attribute__((aligned(16)));

__attribute__((noinline)) void convolution2d()
{
	for (int i = 1; i < NI - 1; i++)
	{
		for (int j = 1; j < NJ - 1; j++)
		{
			/*For 3x3 filter*/
			C[i * NJ + j] = c11 * A[(i - 1) * NJ + (j - 1)] + c12 * A[(i + 0) * NJ + (j - 1)] + c13 * A[(i + 1) * NJ + (j - 1)] + c21 * A[(i - 1) * NJ + (j + 0)] + c22 * A[(i + 0) * NJ + (j + 0)] + c23 * A[(i + 1) * NJ + (j + 0)] + c31 * A[(i - 1) * NJ + (j + 1)] + c32 * A[(i + 0) * NJ + (j + 1)] + c33 * A[(i + 1) * NJ + (j + 1)];

			// /*For 2x2 filter*/
			// 	B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]
			// 			+ c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)];
		}
	}
}

__attribute__((noinline)) void pedometer()
{
	for (int i = 0; i < SIZE; i++)
	{
		res_vector[i] = (acceleration_x[i] - acc_avg_x[i]) * (acceleration_x[i] - acc_avg_x[i]) + (acceleration_y[i] - acc_avg_y[i]) * (acceleration_y[i] - acc_avg_y[i]) + (acceleration_z[i] - acc_avg_z[i]) * (acceleration_z[i] - acc_avg_z[i]);
	}
}

/* Array initialization. */
void init_array()
{
	int i, j;
	for (i = 0; i < SIZE; i++)
	{
		A[i] = i;
	}
	for (int i = 0; i < SIZE; i++)
	{
		acceleration_x[i] = 3 * i;
		acceleration_y[i] = 3 * i + 1;
		acceleration_z[i] = 3 * i + 2;

		acc_avg_x[i] = i;
		acc_avg_y[i] = i;
		acc_avg_z[i] = i;
	}
}

void result_check()
{
	int i, j;
	for (int i = 1; i < NI - 1; i++)
	{
		for (int j = 1; j < NJ - 1; j++)
		{
			int idx = i * NJ + j;
			if (C[idx] != B[idx])
				printf("There is an error in location (%d)[%d, %d]\n", idx, C[idx], B[idx]);
		}
	}
	for (i = 0; i < SIZE; i++)
	{
		if (acc_vector[i] != res_vector[i])
			printf("There is an error in location (%d)[%d, %d]\n", i, acc_vector[i], res_vector[i]);
	}
}

void cgra_execute(void **din_addr, void **dout_addr)
{
	static unsigned short cin[135][3] __attribute__((aligned(16))) = {
		{0x3002, 0x7800, 0x0008},
		{0x00c0, 0x00f0, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x2800, 0x7800, 0x000c},
		{0x00c0, 0x00f0, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x2000, 0x0000, 0x0010},
		{0x0010, 0x0000, 0x0011},
		{0x0000, 0x0100, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x3800, 0x0000, 0x0014},
		{0x0010, 0x0000, 0x0015},
		{0x0000, 0x9500, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x3000, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x2000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x3041, 0x7800, 0x0024},
		{0x00c0, 0x00f0, 0x0025},
		{0x0000, 0x0100, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x0100, 0x0000, 0x0030},
		{0x0000, 0x0004, 0x0034},
		{0x0000, 0x0008, 0x0038},
		{0x0030, 0x0008, 0x003c},
		{0x8000, 0x0008, 0x0040},
		{0x0000, 0x0008, 0x0044},
		{0x0800, 0x0010, 0x0048},
		{0x0100, 0x0000, 0x004c},
		{0xfff8, 0xffff, 0x0058},
		{0x0413, 0x0000, 0x0059},
		{0x0001, 0x0000, 0x0060},
		{0x0405, 0x0000, 0x0061},
		{0x4802, 0x0000, 0x0065},
		{0x6c03, 0x0000, 0x0069},
		{0x8c01, 0x0000, 0x006d},
		{0x3201, 0x0000, 0x0071},
		{0x0007, 0x0000, 0x0074},
		{0x0423, 0x0000, 0x0075},
		{0x0300, 0x0020, 0x008c},
		{0x0050, 0x0880, 0x0090},
		{0x3800, 0x0000, 0x0094},
		{0x0000, 0x0600, 0x0098},
		{0x0400, 0x0000, 0x009c},
		{0x4893, 0x0000, 0x00b1},
		{0x8412, 0x0000, 0x00b5},
		{0x6a01, 0x0000, 0x00b9},
		{0x3011, 0x0000, 0x00bd},
		{0x6c03, 0x0000, 0x00c1},
		{0x2e01, 0x0000, 0x00c5},
		{0x0000, 0x0100, 0x00d4},
		{0x3000, 0x0080, 0x00d8},
		{0x0000, 0x0083, 0x00dc},
		{0x208c, 0x0000, 0x00e0},
		{0x0000, 0x0000, 0x00e4},
		{0x2053, 0x0000, 0x00e8},
		{0x0010, 0x0000, 0x00ec},
		{0x6a01, 0x0000, 0x00fd},
		{0x2901, 0x0000, 0x0105},
		{0x6901, 0x0000, 0x0109},
		{0x6802, 0x0000, 0x010d},
		{0x8e01, 0x0000, 0x0111},
		{0x0000, 0x0040, 0x0120},
		{0x0080, 0x0208, 0x0124},
		{0x0140, 0x0020, 0x0130},
		{0x40c0, 0x0040, 0x0134},
		{0x0020, 0x0000, 0x0138},
		{0x0000, 0x0000, 0x013c},
		{0x000a, 0x0000, 0x0144},
		{0x1043, 0x0000, 0x0145},
		{0xfff7, 0xffff, 0x0148},
		{0x1023, 0x0000, 0x0149},
		{0x0002, 0x0000, 0x0154},
		{0x0c05, 0x0000, 0x0155},
		{0x4581, 0x0000, 0x0159},
		{0x0005, 0x0000, 0x015c},
		{0x1003, 0x0000, 0x015d},
		{0xfffd, 0xffff, 0x0160},
		{0x1003, 0x0000, 0x0161},
		{0x0006, 0x0000, 0x0164},
		{0x1013, 0x0000, 0x0165},
		{0x0001, 0x0000, 0x0170},
		{0x0301, 0x0000, 0x0174},
		{0x0010, 0x0000, 0x017c},
		{0x0000, 0x0010, 0x0180},
		{0x0000, 0x0008, 0x0184},
		{0x0001, 0x0000, 0x0188},
		{0x0001, 0x0000, 0x018c},
		{0x2042, 0x7800, 0x0194},
		{0x00c0, 0x00f0, 0x0195},
		{0x0000, 0x0500, 0x0196},
		{0x0000, 0x0000, 0x0197},
		{0x2822, 0x7800, 0x0198},
		{0x00c0, 0x00f0, 0x0199},
		{0x0000, 0x0100, 0x019a},
		{0x0000, 0x0000, 0x019b},
		{0x3801, 0x7800, 0x019c},
		{0x00c0, 0x00f0, 0x019d},
		{0x0000, 0xa100, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x3840, 0x7800, 0x01a0},
		{0x00c0, 0x00f0, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x2000, 0x0000, 0x01a4},
		{0x0010, 0x0000, 0x01a5},
		{0x0000, 0x0100, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x2800, 0x0000, 0x01a8},
		{0x0010, 0x0000, 0x01a9},
		{0x0000, 0x0100, 0x01aa},
		{0x0000, 0x0000, 0x01ab},
		{0x3001, 0x7800, 0x01ac},
		{0x00c0, 0x00f0, 0x01ad},
		{0x0000, 0x0100, 0x01ae},
		{0x0000, 0x0000, 0x01af},
		{0x2820, 0x7800, 0x01b0},
		{0x00c0, 0x00f0, 0x01b1},
		{0x0000, 0x0100, 0x01b2},
		{0x0000, 0x0000, 0x01b3},
		{0x2021, 0x7800, 0x01b4},
		{0x00c0, 0x00f0, 0x01b5},
		{0x0000, 0x0100, 0x01b6},
		{0x0000, 0x0000, 0x01b7},
	};

	load_cfg((void *)cin, 0x24000, 810, 0, 0);
	load_data(din_addr[0], 0x2000, 4096, 1, 0, 0);
	load_data(din_addr[1], 0x22000, 4096, 1, 0, 0);
	load_data(din_addr[2], 0x1e000, 4096, 1, 0, 0);
	load_data(din_addr[3], 0x1c000, 4096, 1, 0, 0);
	load_data(din_addr[4], 0x20000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x14000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x4000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x12000, 4096, 1, 0, 0);
	load_data(din_addr[8], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[10], 0x1a000, 4096, 0, 0, 0);
	load_data(din_addr[11], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[13], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[14], 0xc000, 4096, 0, 0, 0);
	config(0x0, 135, 0, 0);
	execute(0x3fffe, 0, 0);
	store(dout_addr[0], 0x16000, 3836, 0, 0);
	store(dout_addr[1], 0xe000, 4096, 0, 0);
}

int main()
{
	long long unsigned start;
	long long unsigned end;
	init_array();
	printf("Initialization finished!\n");

	start = rdcycle();
	/* Run kernel. */
	convolution2d();
	pedometer();
	end = rdcycle();
	printf("It takes %d cycles for CPU to finish the task.\n", end - start);

	void *cgra_din_addr[15] = {A, A, A, A, A, A, A, A, A, acc_avg_x, acc_avg_y, acc_avg_z, acceleration_x, acceleration_y, acceleration_z};
	void *cgra_dout_addr[2] = {(void *)B + 128, acc_vector};
	start = rdcycle();
	cgra_execute(cgra_din_addr, cgra_dout_addr);
	volatile int result = fence(1);
	end = rdcycle();
	printf("It takes %d cycles for CGRA to finish the task(%d).\n", end - start, result);

	result_check();
	printf("Done!\n");

	return 0;
}

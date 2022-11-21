void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[59][3] __attribute__((aligned(8))) = {
		{0x0c00, 0x2010, 0x0004},
		{0x2040, 0x8107, 0x0005},
		{0x0e30, 0x0102, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x800a, 0x2000, 0x0008},
		{0xf900, 0x0107, 0x0009},
		{0x0002, 0x0102, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x1bbe, 0x2000, 0x000c},
		{0x0040, 0x8100, 0x000d},
		{0x0000, 0x9102, 0x000e},
		{0x0080, 0x0000, 0x000f},
		{0x1460, 0x2010, 0x0010},
		{0x2040, 0x8107, 0x0011},
		{0x0e30, 0x0102, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x8003, 0x2000, 0x0014},
		{0xf900, 0x0107, 0x0015},
		{0x0002, 0x0102, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x0400, 0x0000, 0x002c},
		{0x0103, 0x0000, 0x0034},
		{0x0000, 0x0000, 0x0038},
		{0x2403, 0x0000, 0x0051},
		{0x4403, 0x0000, 0x0059},
		{0x0000, 0x0000, 0x0078},
		{0x0000, 0x0200, 0x007c},
		{0x3001, 0x0000, 0x009d},
		{0x100d, 0x0000, 0x00a1},
		{0x0000, 0x7001, 0x00a2},
		{0x0020, 0x0100, 0x00a3},
		{0x0400, 0x0000, 0x00c0},
		{0x0303, 0x0000, 0x00c4},
		{0x0001, 0x0000, 0x00c8},
		{0x2c11, 0x0000, 0x00e5},
		{0x6421, 0x0000, 0x00e9},
		{0x0010, 0x0000, 0x0108},
		{0x0000, 0x0000, 0x010c},
		{0x8c03, 0x0000, 0x0129},
		{0x9003, 0x0000, 0x0131},
		{0x0010, 0x0000, 0x014c},
		{0x0000, 0x0000, 0x0150},
		{0x0004, 0x0000, 0x0158},
		{0x1040, 0x2010, 0x016c},
		{0x2040, 0x8107, 0x016d},
		{0x0e30, 0x0102, 0x016e},
		{0x0000, 0x0000, 0x016f},
		{0x8c02, 0x2000, 0x0174},
		{0xf900, 0x0107, 0x0175},
		{0x0002, 0x0102, 0x0176},
		{0x0000, 0x0000, 0x0177},
		{0x800b, 0x2000, 0x0178},
		{0xf900, 0x0107, 0x0179},
		{0x0002, 0x0102, 0x017a},
		{0x0000, 0x0000, 0x017b},
		{0x0020, 0x2010, 0x017c},
		{0x2040, 0x8107, 0x017d},
		{0x0e30, 0x0102, 0x017e},
		{0x0000, 0x0000, 0x017f},
	};

	load_cfg((void*)cin, 0x20228, 354, 4, 0);
	load_data(din_addr[0], 0x28, 4096, 1, 4, 0);
	load_data(din_addr[1], 0x10028, 4096, 1, 4, 0);
	load_data(din_addr[2], 0x13000, 4096, 1, 4, 0);
	load_data(din_addr[3], 0x8000, 4096, 0, 4, 0);
	load_data(din_addr[4], 0x3000, 4096, 1, 4, 0);
	load_data(din_addr[5], 0x18000, 4096, 1, 4, 0);
	load_data(din_addr[6], 0x14000, 4096, 1, 4, 0);
	load_data(din_addr[7], 0x5000, 4096, 0, 4, 0);
	config(0x45, 59, 4, 0);
	execute(0x1d1f, 4, 0);
	store(dout_addr[0], 0x6ef8, 4096, 4, 0);
}
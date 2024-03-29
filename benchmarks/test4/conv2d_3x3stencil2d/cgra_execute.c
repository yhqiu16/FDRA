void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[124][3] __attribute__((aligned(16))) = {
		{0x0001, 0x0c04, 0x0004},
		{0xf040, 0x80f7, 0x0005},
		{0xefe1, 0x0101, 0x0006},
		{0x3021, 0x7800, 0x0008},
		{0x00c0, 0x00f0, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x2842, 0x7800, 0x000c},
		{0x00c0, 0x00f0, 0x000d},
		{0x0000, 0x0100, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x7802, 0x0c00, 0x0010},
		{0xfe80, 0x00f7, 0x0011},
		{0xeffd, 0x0101, 0x0012},
		{0x0000, 0x0c04, 0x0014},
		{0xf040, 0x80f7, 0x0015},
		{0xefe1, 0x0101, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x2840, 0x7800, 0x0018},
		{0x00c0, 0x00f0, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x3000, 0x7800, 0x001c},
		{0x00c0, 0x00f0, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x3801, 0x7800, 0x0020},
		{0x00c0, 0x00f0, 0x0021},
		{0x0000, 0x9f00, 0x0022},
		{0x0001, 0x0000, 0x0023},
		{0x2022, 0x7800, 0x0024},
		{0x00c0, 0x00f0, 0x0025},
		{0x0000, 0x0300, 0x0026},
		{0x4200, 0x0000, 0x0030},
		{0x2000, 0x0002, 0x0034},
		{0x4000, 0x0002, 0x0038},
		{0x1000, 0x0006, 0x003c},
		{0x0000, 0x0006, 0x0040},
		{0x0000, 0x0006, 0x0044},
		{0x0200, 0x0000, 0x0048},
		{0x0011, 0x0000, 0x004c},
		{0x000a, 0x0000, 0x0054},
		{0x0833, 0x0000, 0x0055},
		{0x5001, 0x0000, 0x0059},
		{0x0006, 0x0000, 0x005c},
		{0x0403, 0x0000, 0x005d},
		{0x5001, 0x0000, 0x0061},
		{0x2c83, 0x0000, 0x0065},
		{0x3201, 0x0000, 0x0071},
		{0xfff7, 0xffff, 0x0074},
		{0x0443, 0x0000, 0x0075},
		{0x0000, 0x00c0, 0x0080},
		{0x0402, 0x0080, 0x0084},
		{0x0200, 0x0001, 0x0088},
		{0x0032, 0x00c0, 0x008c},
		{0x0200, 0x0601, 0x0090},
		{0x0300, 0x0000, 0x0094},
		{0x0400, 0x0000, 0x009c},
		{0x3201, 0x0000, 0x00ad},
		{0x8413, 0x0000, 0x00b1},
		{0x6403, 0x0000, 0x00b9},
		{0x0001, 0x0000, 0x00bc},
		{0x0405, 0x0000, 0x00bd},
		{0x2e01, 0x0000, 0x00c5},
		{0x0000, 0x0600, 0x00d4},
		{0x3001, 0x0000, 0x00d8},
		{0x0008, 0x0061, 0x00dc},
		{0x10c0, 0x0683, 0x00e0},
		{0x0200, 0x0003, 0x00e4},
		{0x0000, 0x0044, 0x00e8},
		{0x0020, 0x0000, 0x00ec},
		{0x6981, 0x0000, 0x00fd},
		{0x8601, 0x0000, 0x0101},
		{0x8801, 0x0000, 0x0105},
		{0x4c81, 0x0000, 0x0109},
		{0x8601, 0x0000, 0x010d},
		{0x0340, 0x0000, 0x0124},
		{0x0008, 0x0060, 0x012c},
		{0x2024, 0x0021, 0x0130},
		{0x0000, 0x0020, 0x0138},
		{0x0005, 0x0000, 0x0148},
		{0x1003, 0x0000, 0x0149},
		{0x040d, 0x0000, 0x014d},
		{0x0000, 0x9001, 0x014e},
		{0x000c, 0x00e1, 0x014f},
		{0xfffd, 0xffff, 0x0150},
		{0x1003, 0x0000, 0x0151},
		{0x0002, 0x0000, 0x0154},
		{0x0805, 0x0000, 0x0155},
		{0x0007, 0x0000, 0x0158},
		{0x1013, 0x0000, 0x0159},
		{0xfff8, 0xffff, 0x0160},
		{0x1033, 0x0000, 0x0161},
		{0x0101, 0x0000, 0x0174},
		{0x0001, 0x0000, 0x017c},
		{0x0000, 0x0000, 0x0180},
		{0x0000, 0x0000, 0x0184},
		{0x0000, 0x0000, 0x0188},
		{0x0000, 0x0000, 0x018c},
		{0x2801, 0x7800, 0x0198},
		{0x00c0, 0x00f0, 0x0199},
		{0x0000, 0x0100, 0x019a},
		{0x1000, 0x0c00, 0x019c},
		{0x0040, 0x80f0, 0x019d},
		{0xe001, 0x9501, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x2820, 0x7800, 0x01a0},
		{0x00c0, 0x00f0, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x0002, 0x0c04, 0x01a4},
		{0xf040, 0x80f7, 0x01a5},
		{0xefe1, 0x0101, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x7800, 0x0c00, 0x01a8},
		{0xfe80, 0x00f7, 0x01a9},
		{0xeffd, 0x0101, 0x01aa},
		{0x3041, 0x7800, 0x01ac},
		{0x00c0, 0x00f0, 0x01ad},
		{0x0000, 0x0100, 0x01ae},
		{0x0000, 0x0000, 0x01af},
		{0x6801, 0x0c00, 0x01b0},
		{0xfe80, 0x00f7, 0x01b1},
		{0xeffd, 0x0101, 0x01b2},
		{0x2002, 0x7800, 0x01b4},
		{0x00c0, 0x00f0, 0x01b5},
		{0x0000, 0x0100, 0x01b6},
	};

	load_cfg((void*)cin, 0x24000, 744, 0, 0);
	load_data(din_addr[0], 0xc000, 4096, 1, 0, 0);
	load_data(din_addr[1], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[2], 0xa000, 4096, 1, 0, 0);
	load_data(din_addr[3], 0x12000, 4096, 1, 0, 0);
	load_data(din_addr[4], 0x4000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x1c000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x20000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x10000, 4096, 1, 0, 0);
	load_data(din_addr[8], 0x2000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0x1e000, 36, 1, 0, 0);
	load_data(din_addr[10], 0x22000, 36, 1, 0, 0);
	load_data(din_addr[11], 0x6000, 36, 0, 0, 0);
	load_data(din_addr[12], 0x8000, 4096, 1, 0, 0);
	load_data(din_addr[13], 0x0, 4096, 1, 0, 0);
	load_data(din_addr[14], 0x18000, 4096, 0, 0, 0);
	config(0x0, 124, 0, 0);
	execute(0x3fdff, 0, 0);
	store(dout_addr[0], 0xe000, 3836, 0, 0);
	store(dout_addr[1], 0x14000, 3832, 0, 0);
}

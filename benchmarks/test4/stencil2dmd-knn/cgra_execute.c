void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[156][3] __attribute__((aligned(16))) = {
		{0x780a, 0x0c00, 0x0004},
		{0xfe80, 0x00f7, 0x0005},
		{0xeffd, 0x0101, 0x0006},
		{0x0000, 0x0000, 0x0007},
		{0x0800, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0002, 0x0c04, 0x000c},
		{0xf040, 0x80f7, 0x000d},
		{0xefe1, 0x0101, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x1000, 0x0c00, 0x0010},
		{0x0040, 0x80f0, 0x0011},
		{0xe001, 0x9301, 0x0012},
		{0x0020, 0x0000, 0x0013},
		{0x1408, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0b00, 0x001a},
		{0x0021, 0x0000, 0x001b},
		{0x0801, 0x0c04, 0x001c},
		{0xf040, 0x80f7, 0x001d},
		{0xefe1, 0x0101, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2000, 0x4000, 0x0020},
		{0x0040, 0x0200, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0000, 0x0023},
		{0x6801, 0x0c00, 0x0024},
		{0xfe80, 0x00f7, 0x0025},
		{0xeffd, 0x0101, 0x0026},
		{0x0000, 0x0000, 0x0027},
		{0x0000, 0x0030, 0x0030},
		{0x0000, 0x0002, 0x0034},
		{0x0000, 0x0002, 0x0038},
		{0x1001, 0x0002, 0x003c},
		{0x2000, 0x0011, 0x0040},
		{0x0102, 0x0001, 0x0044},
		{0x8100, 0x0011, 0x0048},
		{0x0000, 0x0000, 0x004c},
		{0x100d, 0x0000, 0x0055},
		{0x0000, 0x9001, 0x0056},
		{0x0041, 0x0010, 0x0057},
		{0x2803, 0x0000, 0x0059},
		{0xf830, 0xffff, 0x005c},
		{0x1001, 0x0000, 0x005d},
		{0x080d, 0x0000, 0x0061},
		{0x0000, 0x8001, 0x0062},
		{0x000c, 0x00e1, 0x0063},
		{0x6831, 0x0000, 0x0065},
		{0x1030, 0x0000, 0x0069},
		{0x4403, 0x0000, 0x006d},
		{0x0004, 0x0000, 0x0070},
		{0x0403, 0x0000, 0x0071},
		{0x0004, 0x0000, 0x0074},
		{0x0403, 0x0000, 0x0075},
		{0x8001, 0x0000, 0x0080},
		{0x3000, 0x0600, 0x0084},
		{0x0000, 0x0043, 0x0088},
		{0x4080, 0x0001, 0x008c},
		{0x0c00, 0x0000, 0x0090},
		{0x0002, 0x0040, 0x0094},
		{0x0300, 0x0083, 0x0098},
		{0x0200, 0x0201, 0x009c},
		{0x5203, 0x0000, 0x00a5},
		{0x8823, 0x0000, 0x00a9},
		{0x3101, 0x0000, 0x00ad},
		{0x5083, 0x0000, 0x00b1},
		{0x000a, 0x0000, 0x00b4},
		{0x8004, 0x0000, 0x00b5},
		{0x2c42, 0x0000, 0x00b9},
		{0x9001, 0x0000, 0x00bd},
		{0x0004, 0x0000, 0x00c0},
		{0x0403, 0x0000, 0x00c1},
		{0x0400, 0x0000, 0x00c5},
		{0x0002, 0x0600, 0x00d0},
		{0x1008, 0x0601, 0x00d4},
		{0x8002, 0x0100, 0x00d8},
		{0x0001, 0x0083, 0x00dc},
		{0x0020, 0x0804, 0x00e0},
		{0x0000, 0x0003, 0x00e4},
		{0x0001, 0x0204, 0x00e8},
		{0x0000, 0x0600, 0x00ec},
		{0x8a03, 0x0000, 0x00f9},
		{0x4483, 0x0000, 0x00fd},
		{0x4803, 0x0000, 0x0101},
		{0x9003, 0x0000, 0x0105},
		{0x6881, 0x0000, 0x0109},
		{0x2403, 0x0000, 0x010d},
		{0x9003, 0x0000, 0x0111},
		{0x1010, 0x0000, 0x0115},
		{0x0100, 0x0600, 0x0120},
		{0x010c, 0x00c0, 0x0124},
		{0xc000, 0x00a0, 0x0128},
		{0x0200, 0x0003, 0x012c},
		{0x0005, 0x0643, 0x0130},
		{0x0000, 0x0081, 0x0134},
		{0x3200, 0x0600, 0x0138},
		{0x0000, 0x0600, 0x013c},
		{0x0000, 0x0000, 0x0140},
		{0x040d, 0x0000, 0x0149},
		{0x0000, 0x9001, 0x014a},
		{0x0041, 0x0010, 0x014b},
		{0x4603, 0x0000, 0x014d},
		{0x0c0d, 0x0000, 0x0151},
		{0x0000, 0x9001, 0x0152},
		{0x0041, 0x0010, 0x0153},
		{0x8442, 0x0000, 0x0155},
		{0x8c13, 0x0000, 0x0159},
		{0x0840, 0x0000, 0x015d},
		{0x0440, 0x0000, 0x0161},
		{0x8c42, 0x0000, 0x0165},
		{0x0300, 0x0000, 0x0170},
		{0x0000, 0x000c, 0x0174},
		{0x0100, 0x0007, 0x0178},
		{0x0000, 0x0003, 0x017c},
		{0x0018, 0x0002, 0x0180},
		{0x0008, 0x0011, 0x0184},
		{0x2000, 0x0021, 0x0188},
		{0x0320, 0x0000, 0x018c},
		{0x1800, 0x4000, 0x0194},
		{0x0040, 0x0200, 0x0195},
		{0x0000, 0xb500, 0x0196},
		{0x0020, 0x0000, 0x0197},
		{0x1000, 0x4000, 0x0198},
		{0x0040, 0x0200, 0x0199},
		{0x0000, 0xb700, 0x019a},
		{0x0000, 0x0000, 0x019b},
		{0x0000, 0x4000, 0x019c},
		{0x0040, 0x0200, 0x019d},
		{0x0000, 0x0100, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x1800, 0x4000, 0x01a0},
		{0x0040, 0x0200, 0x01a1},
		{0x0000, 0xb500, 0x01a2},
		{0x0000, 0x0000, 0x01a3},
		{0x0000, 0x0c04, 0x01a4},
		{0xf040, 0x80f7, 0x01a5},
		{0xefe1, 0x0101, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x0800, 0x4000, 0x01a8},
		{0x0040, 0x0200, 0x01a9},
		{0x0000, 0x0300, 0x01aa},
		{0x0000, 0x0000, 0x01ab},
		{0x1000, 0x0000, 0x01ac},
		{0x0010, 0x0000, 0x01ad},
		{0x0000, 0x0d00, 0x01ae},
		{0x0021, 0x0000, 0x01af},
		{0x6800, 0x0c00, 0x01b0},
		{0xfe80, 0x00f7, 0x01b1},
		{0xeffd, 0x0101, 0x01b2},
		{0x0000, 0x0000, 0x01b3},
		{0x0000, 0x0000, 0x01b4},
		{0x0010, 0x0000, 0x01b5},
		{0x0000, 0x0d00, 0x01b6},
		{0x0001, 0x0000, 0x01b7},
	};

	load_cfg((void*)cin, 0x24000, 936, 2, 2);
	load_data(din_addr[0], 0x8000, 4096, 0, 2, 0);
	load_data(din_addr[1], 0x20000, 256, 1, 2, 0);
	load_data(din_addr[2], 0x1a000, 256, 0, 2, 0);
	load_data(din_addr[3], 0xd020, 256, 1, 2, 0);
	load_data(din_addr[4], 0x10000, 256, 0, 2, 0);
	load_data(din_addr[5], 0x1c000, 256, 1, 2, 0);
	load_data(din_addr[6], 0x2000, 256, 0, 2, 0);
	load_data(din_addr[7], 0x22000, 36, 1, 2, 0);
	load_data(din_addr[8], 0x12000, 36, 1, 2, 0);
	load_data(din_addr[9], 0x6020, 36, 0, 2, 0);
	load_data(din_addr[10], 0x18000, 4096, 1, 2, 0);
	load_data(din_addr[11], 0xa000, 4096, 1, 2, 0);
	load_data(din_addr[12], 0x0, 4096, 0, 2, 0);
	config(0x0, 156, 2, 0);
	execute(0x3ffef, 2, 0);
	store(dout_addr[0], 0x4000, 3832, 2, 0);
	store(dout_addr[1], 0x1e000, 256, 2, 0);
	store(dout_addr[2], 0x14000, 256, 2, 0);
	store(dout_addr[3], 0x16000, 256, 2, 0);
}
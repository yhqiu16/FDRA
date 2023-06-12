void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[115][3] __attribute__((aligned(16))) = {
		{0x8802, 0x2000, 0x0004},
		{0xf900, 0x0107, 0x0005},
		{0x0002, 0x0102, 0x0006},
		{0x9803, 0x0800, 0x0008},
		{0xff00, 0x1fcf, 0x0009},
		{0x0000, 0x0100, 0x000a},
		{0x8003, 0x2000, 0x000c},
		{0xf900, 0x0107, 0x000d},
		{0x0002, 0x0102, 0x000e},
		{0x0000, 0x0000, 0x000f},
		{0x1020, 0x2010, 0x0010},
		{0x2040, 0x8107, 0x0011},
		{0x0e30, 0x0102, 0x0012},
		{0x9002, 0x0800, 0x0014},
		{0xff00, 0x1fcf, 0x0015},
		{0x0000, 0x0100, 0x0016},
		{0x0000, 0x0000, 0x0017},
		{0x1803, 0x0800, 0x0018},
		{0x0040, 0x1fc8, 0x0019},
		{0x0000, 0x9300, 0x001a},
		{0x0001, 0x0000, 0x001b},
		{0x0860, 0x2010, 0x001c},
		{0x2040, 0x8107, 0x001d},
		{0x0e30, 0x0102, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x8006, 0x0bff, 0x0020},
		{0x0140, 0x1fc8, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x8001, 0x2000, 0x0024},
		{0xf900, 0x0107, 0x0025},
		{0x0002, 0x0102, 0x0026},
		{0x0000, 0x0000, 0x0030},
		{0x0100, 0x0000, 0x0034},
		{0x0400, 0x0000, 0x003c},
		{0x2000, 0x0000, 0x0040},
		{0x8081, 0x0004, 0x0044},
		{0x8000, 0x0004, 0x0048},
		{0x0000, 0x0000, 0x004c},
		{0x3103, 0x0000, 0x005d},
		{0x8821, 0x0000, 0x0061},
		{0x4413, 0x0000, 0x0065},
		{0x100d, 0x0000, 0x0069},
		{0x0000, 0x8001, 0x006a},
		{0x4008, 0x00fe, 0x006b},
		{0x8413, 0x0000, 0x006d},
		{0x0000, 0x0600, 0x0080},
		{0x0000, 0x0600, 0x0084},
		{0x0002, 0x0000, 0x0088},
		{0x0800, 0x0003, 0x008c},
		{0x0000, 0x0003, 0x0090},
		{0x0100, 0x0002, 0x0094},
		{0x000c, 0x0600, 0x0098},
		{0x4c11, 0x0000, 0x00ad},
		{0x3011, 0x0000, 0x00b5},
		{0x6411, 0x0000, 0x00bd},
		{0x0000, 0x0600, 0x00d0},
		{0x0330, 0x0200, 0x00d4},
		{0x0000, 0x0040, 0x00dc},
		{0x0802, 0x0000, 0x00e0},
		{0x1040, 0x0004, 0x00e4},
		{0x8000, 0x0020, 0x00e8},
		{0x8403, 0x0000, 0x00fd},
		{0x7011, 0x0000, 0x0101},
		{0x2811, 0x0000, 0x0109},
		{0x4c13, 0x0000, 0x010d},
		{0x0c00, 0x0000, 0x0120},
		{0x0300, 0x0008, 0x0124},
		{0x000c, 0x0000, 0x0128},
		{0x0001, 0x0000, 0x012c},
		{0x0030, 0x0018, 0x0134},
		{0x0000, 0x0021, 0x0138},
		{0x3023, 0x0000, 0x0149},
		{0x040d, 0x0000, 0x014d},
		{0x0000, 0x9001, 0x014e},
		{0x0020, 0x0100, 0x014f},
		{0x8c03, 0x0000, 0x0151},
		{0x8c03, 0x0000, 0x0161},
		{0x0c00, 0x0000, 0x0170},
		{0x0000, 0x0004, 0x0174},
		{0x0020, 0x0000, 0x0178},
		{0x0000, 0x0000, 0x017c},
		{0x0000, 0x0001, 0x0184},
		{0x0010, 0x0000, 0x0188},
		{0x0000, 0x0000, 0x018c},
		{0x1800, 0x2000, 0x0194},
		{0x0040, 0x8100, 0x0195},
		{0x0000, 0x9502, 0x0196},
		{0x0001, 0x0000, 0x0197},
		{0x9000, 0x0800, 0x0198},
		{0xff00, 0x1fcf, 0x0199},
		{0x0000, 0x0100, 0x019a},
		{0x0840, 0x2010, 0x019c},
		{0x2040, 0x8107, 0x019d},
		{0x0e30, 0x0102, 0x019e},
		{0x0000, 0x0000, 0x019f},
		{0x8004, 0x0bff, 0x01a0},
		{0x0140, 0x1fc8, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x8807, 0x0bff, 0x01a4},
		{0x0140, 0x1fc8, 0x01a5},
		{0x0000, 0x0100, 0x01a6},
		{0x0000, 0x0000, 0x01a7},
		{0x9801, 0x0800, 0x01a8},
		{0xff00, 0x1fcf, 0x01a9},
		{0x0000, 0x0100, 0x01aa},
		{0x1000, 0x2010, 0x01ac},
		{0x2040, 0x8107, 0x01ad},
		{0x0e30, 0x0102, 0x01ae},
		{0x0000, 0x0000, 0x01af},
		{0x8005, 0x0bff, 0x01b0},
		{0x0140, 0x1fc8, 0x01b1},
		{0x0000, 0x0100, 0x01b2},
		{0x8800, 0x2000, 0x01b4},
		{0xf900, 0x0107, 0x01b5},
		{0x0002, 0x0102, 0x01b6},
	};

	load_cfg((void*)cin, 0x24000, 690, 0, 0);
	load_data(din_addr[0], 0x14000, 32, 1, 0, 0);
	load_data(din_addr[1], 0x1e000, 32, 1, 0, 0);
	load_data(din_addr[2], 0xc000, 32, 1, 0, 0);
	load_data(din_addr[3], 0x6000, 32, 0, 0, 0);
	load_data(din_addr[4], 0x1a000, 4096, 1, 0, 0);
	load_data(din_addr[5], 0x8000, 4096, 1, 0, 0);
	load_data(din_addr[6], 0x20000, 4096, 1, 0, 0);
	load_data(din_addr[7], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x22000, 4096, 1, 0, 0);
	load_data(din_addr[9], 0x10000, 4096, 1, 0, 0);
	load_data(din_addr[10], 0x2000, 4096, 1, 0, 0);
	load_data(din_addr[11], 0x0, 4096, 0, 0, 0);
	load_data(din_addr[12], 0x1c000, 4096, 1, 0, 0);
	load_data(din_addr[13], 0x4000, 4096, 1, 0, 0);
	load_data(din_addr[14], 0x12000, 4096, 1, 0, 0);
	load_data(din_addr[15], 0xa000, 4096, 0, 0, 0);
	config(0x0, 115, 0, 0);
	execute(0x3ffff, 0, 0);
	store(dout_addr[0], 0xe000, 4080, 0, 0);
	store(dout_addr[1], 0x16000, 4096, 0, 0);
}

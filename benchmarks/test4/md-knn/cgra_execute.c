void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[113][3] __attribute__((aligned(8))) = {
		{0x0400, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0xb300, 0x0006},
		{0x0080, 0x0000, 0x0007},
		{0x1400, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0x0500, 0x000a},
		{0x0000, 0x0000, 0x000b},
		{0x0c00, 0x4000, 0x0010},
		{0x0040, 0x0200, 0x0011},
		{0x0000, 0x0300, 0x0012},
		{0x0000, 0x0000, 0x0013},
		{0x0c00, 0x4000, 0x0018},
		{0x0040, 0x0200, 0x0019},
		{0x0000, 0xb100, 0x001a},
		{0x0080, 0x0000, 0x001b},
		{0x0400, 0x4000, 0x001c},
		{0x0040, 0x0200, 0x001d},
		{0x0000, 0xaf00, 0x001e},
		{0x0080, 0x0000, 0x001f},
		{0x0003, 0x0000, 0x002c},
		{0x0000, 0x0000, 0x0030},
		{0x0000, 0x0004, 0x0038},
		{0x0200, 0x0004, 0x003c},
		{0x4203, 0x0000, 0x0040},
		{0x0003, 0x0000, 0x0044},
		{0x9003, 0x0000, 0x0051},
		{0x7081, 0x0000, 0x0055},
		{0x6c03, 0x0000, 0x0059},
		{0x1000, 0x0000, 0x005d},
		{0x4603, 0x0000, 0x0061},
		{0x0440, 0x0000, 0x0065},
		{0x0c00, 0x0000, 0x0069},
		{0x0000, 0x0040, 0x0074},
		{0x0300, 0x0280, 0x0078},
		{0x00b2, 0x0000, 0x007c},
		{0x5300, 0x0004, 0x0080},
		{0x0201, 0x0400, 0x0084},
		{0x0000, 0x0800, 0x0088},
		{0x0230, 0x0000, 0x008c},
		{0xf830, 0xffff, 0x0094},
		{0x1001, 0x0000, 0x0095},
		{0x100d, 0x0000, 0x0099},
		{0x0000, 0x8001, 0x009a},
		{0x0041, 0x0010, 0x009b},
		{0x8442, 0x0000, 0x009d},
		{0x4803, 0x0000, 0x00a1},
		{0x8442, 0x0000, 0x00a5},
		{0x6603, 0x0000, 0x00a9},
		{0x0c0d, 0x0000, 0x00ad},
		{0x0000, 0x6001, 0x00ae},
		{0x0041, 0x0010, 0x00af},
		{0x040d, 0x0000, 0x00b1},
		{0x0000, 0x6001, 0x00b2},
		{0x0041, 0x0010, 0x00b3},
		{0x0000, 0x0000, 0x00bc},
		{0x4c02, 0x0240, 0x00c0},
		{0x020c, 0x0003, 0x00c4},
		{0x0000, 0x0103, 0x00c8},
		{0x338c, 0x0001, 0x00cc},
		{0x0000, 0x0603, 0x00d0},
		{0x0000, 0x0009, 0x00d4},
		{0x5083, 0x0000, 0x00e1},
		{0x000a, 0x0000, 0x00e4},
		{0x2004, 0x0000, 0x00e5},
		{0x6503, 0x0000, 0x00e9},
		{0x2801, 0x0000, 0x00ed},
		{0x6603, 0x0000, 0x00f1},
		{0x1000, 0x0000, 0x00f5},
		{0x6c03, 0x0000, 0x00f9},
		{0x3000, 0x0040, 0x0104},
		{0x0301, 0x0081, 0x0108},
		{0x0000, 0x00a0, 0x010c},
		{0x0000, 0x0080, 0x0110},
		{0x3080, 0x0020, 0x0114},
		{0x0000, 0x0002, 0x0118},
		{0x0051, 0x0000, 0x011c},
		{0x0820, 0x0000, 0x0125},
		{0x4803, 0x0000, 0x0129},
		{0x0400, 0x0000, 0x012d},
		{0x0004, 0x0000, 0x0130},
		{0x1003, 0x0000, 0x0131},
		{0x0820, 0x0000, 0x0135},
		{0x0004, 0x0000, 0x0138},
		{0x1003, 0x0000, 0x0139},
		{0x8c32, 0x0000, 0x013d},
		{0x0004, 0x0000, 0x0140},
		{0x0c03, 0x0000, 0x0141},
		{0x0100, 0x0000, 0x0154},
		{0x0002, 0x0000, 0x0158},
		{0x0100, 0x0013, 0x015c},
		{0x0020, 0x0001, 0x0160},
		{0x0110, 0x0000, 0x0164},
		{0x0c00, 0x0000, 0x0178},
		{0x0010, 0x0000, 0x0179},
		{0x0000, 0x0b00, 0x017a},
		{0x0001, 0x0000, 0x017b},
		{0x1c00, 0x4000, 0x017c},
		{0x0040, 0x0200, 0x017d},
		{0x0000, 0x0100, 0x017e},
		{0x0000, 0x0000, 0x017f},
		{0x0c00, 0x0000, 0x0180},
		{0x0010, 0x0000, 0x0181},
		{0x0000, 0x0900, 0x0182},
		{0x0001, 0x0000, 0x0183},
		{0x2000, 0x4000, 0x0184},
		{0x0040, 0x0200, 0x0185},
		{0x0000, 0x0100, 0x0186},
		{0x0000, 0x0000, 0x0187},
		{0x1400, 0x0000, 0x0188},
		{0x0010, 0x0000, 0x0189},
		{0x0000, 0x0900, 0x018a},
		{0x0001, 0x0000, 0x018b},
	};

	load_cfg((void*)cin, 0x20000, 678, 6, 2);
	load_data(din_addr[0], 0x18000, 4096, 0, 6, 0);
	load_data(din_addr[1], 0x1b000, 256, 1, 6, 0);
	load_data(din_addr[2], 0x3000, 256, 0, 6, 0);
	load_data(din_addr[3], 0x1d000, 256, 1, 6, 0);
	load_data(din_addr[4], 0x1f000, 256, 0, 6, 0);
	load_data(din_addr[5], 0x13000, 256, 1, 6, 0);
	load_data(din_addr[6], 0x5000, 256, 0, 6, 0);
	config(0x0, 113, 6, 0);
	execute(0xf86b, 6, 0);
	store(dout_addr[0], 0x9000, 256, 6, 0);
	store(dout_addr[1], 0xb000, 256, 6, 0);
	store(dout_addr[2], 0x1000, 256, 6, 0);
}

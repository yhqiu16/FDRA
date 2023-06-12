void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[148][3] __attribute__((aligned(16))) = {
		{0x0000, 0x4000, 0x0004},
		{0x0040, 0x0200, 0x0005},
		{0x0000, 0x0100, 0x0006},
		{0x0800, 0x4000, 0x0008},
		{0x0040, 0x0200, 0x0009},
		{0x0000, 0x0500, 0x000a},
		{0x1000, 0x0000, 0x000c},
		{0x0010, 0x0000, 0x000d},
		{0x0000, 0x0d00, 0x000e},
		{0x0001, 0x0000, 0x000f},
		{0x1800, 0x4000, 0x0010},
		{0x0040, 0x0200, 0x0011},
		{0x0000, 0xbd00, 0x0012},
		{0x0001, 0x0000, 0x0013},
		{0x1800, 0x4000, 0x0014},
		{0x0040, 0x0200, 0x0015},
		{0x0000, 0xb700, 0x0016},
		{0x0020, 0x0000, 0x0017},
		{0x2000, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x3000, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x9100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2800, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x2800, 0x0000, 0x0024},
		{0x0010, 0x0000, 0x0025},
		{0x0000, 0x0100, 0x0026},
		{0x0010, 0x0000, 0x0030},
		{0x000c, 0x0001, 0x0034},
		{0x0000, 0x0008, 0x0038},
		{0x0002, 0x0001, 0x003c},
		{0xc000, 0x0002, 0x0040},
		{0x000c, 0x0008, 0x0044},
		{0x0800, 0x0000, 0x0048},
		{0x0000, 0x0000, 0x004c},
		{0x0c00, 0x0000, 0x0055},
		{0x4442, 0x0000, 0x0059},
		{0x6c03, 0x0000, 0x005d},
		{0x0c40, 0x0000, 0x0061},
		{0x6c83, 0x0000, 0x0065},
		{0x100d, 0x0000, 0x0069},
		{0x0000, 0xa001, 0x006a},
		{0x0041, 0x0010, 0x006b},
		{0x2802, 0x0000, 0x006d},
		{0x0c20, 0x0000, 0x0071},
		{0x8e03, 0x0000, 0x0075},
		{0x0000, 0x0000, 0x007c},
		{0x0000, 0x0640, 0x0080},
		{0x0600, 0x0108, 0x0084},
		{0x0020, 0x0700, 0x0088},
		{0x00b0, 0x0000, 0x008c},
		{0x0000, 0x0060, 0x0090},
		{0x0002, 0x04c0, 0x0094},
		{0x0830, 0x0003, 0x0098},
		{0x0030, 0x0600, 0x009c},
		{0x0020, 0x0000, 0x00a0},
		{0x9001, 0x0000, 0x00a5},
		{0x0004, 0x0000, 0x00a8},
		{0x0c03, 0x0000, 0x00a9},
		{0x2411, 0x0000, 0x00ad},
		{0x6c03, 0x0000, 0x00b1},
		{0xf830, 0xffff, 0x00b4},
		{0x1001, 0x0000, 0x00b5},
		{0x4803, 0x0000, 0x00b9},
		{0x8c01, 0x0000, 0x00bd},
		{0x2e03, 0x0000, 0x00c1},
		{0x0c30, 0x0000, 0x00c5},
		{0x0008, 0x0000, 0x00cc},
		{0x0039, 0x0680, 0x00d0},
		{0x0000, 0x0459, 0x00d4},
		{0x00a0, 0x0691, 0x00d8},
		{0x0800, 0x0000, 0x00dc},
		{0x0101, 0x00e0, 0x00e0},
		{0x0e10, 0x0100, 0x00e4},
		{0x011c, 0x0050, 0x00e8},
		{0x0230, 0x0610, 0x00ec},
		{0x9003, 0x0000, 0x00f5},
		{0x000a, 0x0000, 0x00f8},
		{0x4004, 0x0000, 0x00f9},
		{0x9003, 0x0000, 0x00fd},
		{0x0c10, 0x0000, 0x0101},
		{0x2403, 0x0000, 0x0105},
		{0x8401, 0x0000, 0x0109},
		{0x2403, 0x0000, 0x010d},
		{0x040d, 0x0000, 0x0111},
		{0x0000, 0x8001, 0x0112},
		{0x0041, 0x0010, 0x0113},
		{0x4603, 0x0000, 0x0115},
		{0x0000, 0x0000, 0x011c},
		{0x0005, 0x00e0, 0x0120},
		{0x0200, 0x00c0, 0x0124},
		{0x0015, 0x0680, 0x0128},
		{0x0000, 0x0080, 0x012c},
		{0x0000, 0x0098, 0x0130},
		{0x0004, 0x0111, 0x0134},
		{0x3000, 0x0080, 0x0138},
		{0x5c00, 0x0010, 0x013c},
		{0x9042, 0x0000, 0x0145},
		{0x0004, 0x0000, 0x0148},
		{0x1003, 0x0000, 0x0149},
		{0x8422, 0x0000, 0x014d},
		{0x0004, 0x0000, 0x0150},
		{0x0c03, 0x0000, 0x0151},
		{0x7002, 0x0000, 0x0155},
		{0x6c03, 0x0000, 0x0159},
		{0x080d, 0x0000, 0x015d},
		{0x0000, 0xa001, 0x015e},
		{0x0041, 0x0010, 0x015f},
		{0x4803, 0x0000, 0x0161},
		{0x2c12, 0x0000, 0x0165},
		{0x0009, 0x0000, 0x0170},
		{0x4001, 0x0004, 0x0174},
		{0x0028, 0x000c, 0x0178},
		{0x4040, 0x0008, 0x017c},
		{0x0000, 0x0008, 0x0180},
		{0x0000, 0x0002, 0x0184},
		{0x0310, 0x0000, 0x018c},
		{0x1000, 0x4000, 0x0194},
		{0x0040, 0x0200, 0x0195},
		{0x0000, 0x0100, 0x0196},
		{0x2000, 0x4000, 0x0198},
		{0x0040, 0x0200, 0x0199},
		{0x0000, 0x0100, 0x019a},
		{0x1800, 0x0000, 0x019c},
		{0x0010, 0x0000, 0x019d},
		{0x0000, 0x0900, 0x019e},
		{0x0001, 0x0000, 0x019f},
		{0x2000, 0x0000, 0x01a0},
		{0x0010, 0x0000, 0x01a1},
		{0x0000, 0x0100, 0x01a2},
		{0x1000, 0x0000, 0x01a4},
		{0x0010, 0x0000, 0x01a5},
		{0x0000, 0x0900, 0x01a6},
		{0x0001, 0x0000, 0x01a7},
		{0x2800, 0x0000, 0x01a8},
		{0x0010, 0x0000, 0x01a9},
		{0x0000, 0x0100, 0x01aa},
		{0x2000, 0x0000, 0x01b0},
		{0x0010, 0x0000, 0x01b1},
		{0x0000, 0x0100, 0x01b2},
		{0x0800, 0x4000, 0x01b4},
		{0x0040, 0x0200, 0x01b5},
		{0x0000, 0xb500, 0x01b6},
		{0x0000, 0x0000, 0x01b7},
	};

	load_cfg((void*)cin, 0x24000, 888, 0, 0);
	load_data(din_addr[0], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x1c000, 256, 1, 0, 0);
	load_data(din_addr[2], 0x0, 256, 0, 0, 0);
	load_data(din_addr[3], 0x16000, 256, 1, 0, 0);
	load_data(din_addr[4], 0x14000, 256, 0, 0, 0);
	load_data(din_addr[5], 0x4000, 256, 1, 0, 0);
	load_data(din_addr[6], 0x2000, 256, 0, 0, 0);
	load_data(din_addr[7], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[8], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[9], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[10], 0x1a000, 4096, 0, 0, 0);
	load_data(din_addr[11], 0x12000, 4096, 0, 0, 0);
	load_data(din_addr[12], 0x20000, 4096, 0, 0, 0);
	config(0x0, 148, 0, 0);
	execute(0x37fff, 0, 0);
	store(dout_addr[0], 0xc000, 4096, 0, 0);
	store(dout_addr[1], 0x22000, 256, 0, 0);
	store(dout_addr[2], 0x6000, 256, 0, 0);
	store(dout_addr[3], 0xe000, 256, 0, 0);
}

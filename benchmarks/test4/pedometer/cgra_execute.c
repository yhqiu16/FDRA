void cgra_execute(void** din_addr, void** dout_addr)
{
	static unsigned short cin[43][3] __attribute__((aligned(16))) = {
		{0x3000, 0x0000, 0x0018},
		{0x0010, 0x0000, 0x0019},
		{0x0000, 0x0100, 0x001a},
		{0x2800, 0x0000, 0x001c},
		{0x0010, 0x0000, 0x001d},
		{0x0000, 0x0100, 0x001e},
		{0x0000, 0x0000, 0x001f},
		{0x2000, 0x0000, 0x0020},
		{0x0010, 0x0000, 0x0021},
		{0x0000, 0x0100, 0x0022},
		{0x0000, 0x0004, 0x0040},
		{0x0000, 0x0000, 0x0044},
		{0x1002, 0x0001, 0x0065},
		{0x2000, 0x0000, 0x0088},
		{0x0000, 0x0000, 0x008c},
		{0x4003, 0x0002, 0x00a1},
		{0xc001, 0x0001, 0x00a5},
		{0xa002, 0x0001, 0x00a9},
		{0x9003, 0x0000, 0x00b1},
		{0x0005, 0x0000, 0x00c8},
		{0x00c1, 0x0000, 0x00cc},
		{0x0200, 0x0000, 0x00d0},
		{0x0000, 0x0000, 0x00d4},
		{0x9002, 0x0000, 0x00e9},
		{0x2003, 0x0001, 0x00ed},
		{0x1201, 0x0001, 0x00f5},
		{0x0006, 0x0000, 0x0110},
		{0x4000, 0x0001, 0x0114},
		{0x0000, 0x0000, 0x011c},
		{0x2000, 0x0000, 0x0130},
		{0x0010, 0x0000, 0x0131},
		{0x0000, 0x0100, 0x0132},
		{0x2000, 0x0000, 0x0134},
		{0x0010, 0x0000, 0x0135},
		{0x0000, 0x0100, 0x0136},
		{0x0000, 0x0000, 0x0137},
		{0x2800, 0x0000, 0x0138},
		{0x0010, 0x0000, 0x0139},
		{0x0000, 0x0100, 0x013a},
		{0x3000, 0x0000, 0x0140},
		{0x0010, 0x0000, 0x0141},
		{0x0000, 0x9100, 0x0142},
		{0x0000, 0x0000, 0x0143},
	};

	load_cfg((void*)cin, 0x20000, 258, 0, 0);
	load_data(din_addr[0], 0x10000, 4096, 0, 0, 0);
	load_data(din_addr[1], 0x18000, 4096, 0, 0, 0);
	load_data(din_addr[2], 0x8000, 4096, 0, 0, 0);
	load_data(din_addr[3], 0x1a000, 4096, 0, 0, 0);
	load_data(din_addr[4], 0xa000, 4096, 0, 0, 0);
	load_data(din_addr[5], 0xc000, 4096, 0, 0, 0);
	config(0x0, 43, 0, 0);
	execute(0xb8e0, 0, 0);
	store(dout_addr[0], 0x1c000, 4096, 0, 0);
}

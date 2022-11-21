#include <stdio.h>
#include <stdlib.h>
#include "support.h"

//Define compute data type
#define TYPE int

//Specify row/column sizes
#define row_size 64
#define col_size 64
#define N row_size*col_size

TYPE m1[N];
TYPE m2[N];
TYPE prod[N];
TYPE prod0[N];
void gemm(){
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
            please_map_me();
                k_col = k * col_size;
                mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod0[i_col + j]  = sum;
        }
    }
}

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[22][3] __attribute__((aligned(8))) = {
		{0x0000, 0x0008, 0x0020},
		{0x8202, 0x2060, 0x0021},
		{0x0020, 0x0206, 0x0022},
		{0x0002, 0x0000, 0x0023},
		{0x1000, 0x0000, 0x0028},
		{0x0202, 0x2000, 0x0029},
		{0x0020, 0x0200, 0x002a},
		{0x20e2, 0x0020, 0x002b},
		{0x2000, 0x0000, 0x0030},
		{0x8202, 0x207f, 0x0031},
		{0x0020, 0x0200, 0x0032},
		{0x0002, 0x0000, 0x0033},
		{0x0000, 0x0000, 0x0070},
		{0x0003, 0x0000, 0x0078},
		{0x8803, 0x0000, 0x00b9},
		{0x0000, 0x0020, 0x0108},
		{0x0000, 0x0620, 0x0198},
		{0x0300, 0x0000, 0x0228},
		{0x080f, 0x0000, 0x0271},
		{0x0000, 0x8002, 0x0272},
		{0x2002, 0x0020, 0x0273},
		{0x0000, 0x0000, 0x0274},
	};

	load_cfg(cin, 0x40000, 132, 0, 0);
	load_data(din_addr[0], 0x10000, 16384, 0, 0);
	load_data(din_addr[1], 0x8000, 16384, 0, 0);
	config(0x0, 22, 0, 0);
	execute(0x38, 0, 0);
	store(dout_addr[0], 0x14000, 16384, 0, 0);
}


int main(){
//#ifndef BAREMETAL
//    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
//      perror("mlockall failed");
//      exit(1);
//    }
//#endif

    for (int i=0; i<N; i++){
      m1[i] = i;
      m2[i] = i+1;
    }

    gemm();

    void* cgra_din_addr[2] = {m1, m2};
    void* cgra_dout_addr[1] = {prod};
    cgra_execute(cgra_din_addr, cgra_dout_addr);


//    printf("Here comes the result: \n");
    for (int i=0; i<N; i=i){
        if(prod[i] != prod0[i]){
            printf("%d\n", prod[i], prod0[i]);
        }
    }
    printf("Succeed!\n");

    return 0;
}
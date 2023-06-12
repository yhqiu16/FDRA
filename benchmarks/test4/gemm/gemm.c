// #include "gemm.h"

//Define compute data type
#define TYPE int

//Specify row/column sizes
#define row_size 32
#define col_size 32
#define N row_size*col_size

TYPE m1[row_size][col_size]; 
TYPE m2[col_size][row_size]; 
TYPE prod[row_size][row_size];

void gemm(){
    int i, j, k;
    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<row_size;j++) {
            TYPE sum = 0;
            #pragma unroll 4
            for(k=0;k<col_size;k++) {
#ifdef CGRA_COMPILER
       please_map_me();
#endif
                sum += m1[i][k] * m2[k][j];
            }
            prod[i][j] = sum;
        }
    }
}

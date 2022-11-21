// #include "stencil.h"
//Define input sizes
#define TYPE int
#define col_size 32
#define row_size 32
#define f_size 9

TYPE orig[row_size * col_size]; 
TYPE sol[row_size * col_size]; 
TYPE filter[f_size];

void stencil (){
    int r, c, k1, k2;
    TYPE temp, mul;

    stencil_label1:for (r=0; r<row_size-2; r++) {
        stencil_label2:for (c=0; c<col_size-2; c++) {
            temp = (TYPE)0;
            //please_map_me();
            //#pragma unroll
            for (k1=0;k1<3;k1++){
            
            please_map_me();
            #pragma unroll
                for (k2=0;k2<3;k2++){
                    mul = filter[k1*3 + k2] * orig[(r+k1)*col_size + c+k2];
                    temp += mul;
                }
            }
            sol[(r*col_size) + c] = temp;
        }
    }
}

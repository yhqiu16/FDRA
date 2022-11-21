#include <string.h>
#include <stdio.h>


#define SIZE  20
int A[32], B[32], C[256];
int D[10][16][16];
int E[16][16];

void test(){
   	   for (int i=2;i<9; i=i+2){
   	   	for (int j=15;j>=0;j--){
   	   	    for(int k=0; k<16;k=k+3){
	      		please_map_me();
	      		int tem = A[i*3] + B[j+k];
	      		D[i][j][k] = C[k*k] * E[j][k]+tem;
	      	}
	      	}
	     }
}

int main(){

    for (int i=0;i<32; i++){
      A[i] = i * 2 + 5;
      B[i] = i * 3;
    }
    for (int i=0;i<256; i++){
      C[i] = i;
    }
    for (int i=0;i<256; i++){
        for(int j = 0; j < 16; j++)
          E[i][j] = i+j;
    }
    
   test();

   for (int i=2;i<9; i=i+2){
        for(int j = 0; j < 16; j++)
            for(int k = 0; k < 16; k++)
                printf("%d\n", D[i][j][k]);
    }
   return 0;

}

#include <string.h>
#include <stdio.h>


#define NI 10
#define NJ 16
#define NK 16
#define ASIZE (2*NI)
#define BSIZE (NJ+NK)
#define CSIZE (NJ*NJ)
int A[ASIZE], B[BSIZE];
int C[CSIZE];
int D[NI][NJ][NK];
int E[NJ][NK];

void test(){
  for (int i=2; i<NI; i=i+2){
    for (int j=NJ-1; j>=0; j--){
   	  for(int k=0; k<NK; k=k+3){
   	  	  please_map_me();
   		  int tmp = A[i*2] + B[j+k];
   		  D[i][j][k] = C[j*j] * E[j][k] + tmp;
   		}
   	}
  }
}


int main(){

  for (int i=0; i<ASIZE; i++){
    A[i] = i * 2 + 5;
  }
  for (int i=0; i<BSIZE; i++){
    B[i] = i * 3;
  }
  for (int i=0; i<CSIZE; i++){
    C[i] = i;
  }
  for (int j=0; j<NJ; j++){
    for(int k=0; k<NK; k++)
      E[j][k] = k+j;
  }
    
  test();

  for (int i=2; i<NI; i=i+2){
    for(int j = 0; j < NJ; j++)
      for(int k = 0; k < NK; k++)
        printf("%d", D[i][j][k]);
  }

  return 0;

}

#include <string.h>
#include <stdio.h>


#define SIZE  20
int A[SIZE], B[40][50], C[SIZE];

void array_add(){
   for (int i=0;i<SIZE; i++){
      #ifdef CGRA_COMPILER
      please_map_me();
      #endif
      //C[i] = A[i]+B[i];
      B[i][0] = A[i+1] + A[i-1];
   }
}

int main(){

   int i;
   for (i=0;i<SIZE; i++){
      A[i] = i * 2 + 5;
      B[i][0] = i * 3;
      C[i] = 0;
    }
    
   array_add();

   for (i=0;i<SIZE; i++){
      printf("%d\n", C[i]);
   }
   return 0;

}






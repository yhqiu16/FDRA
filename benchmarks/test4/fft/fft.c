/*
Implementations based on:
V. Volkov and B. Kazian. Fitting fft onto the g80 architecture. 2008.
*/

//#include "fft.h"

//////BEGIN TWIDDLES ////////
#define TYPE int

#define THREADS 64
#define cmplx_M_x(a_x, a_y, b_x, b_y) (a_x*b_x - a_y *b_y)
#define cmplx_M_y(a_x, a_y, b_x, b_y) (a_x*b_y + a_y *b_x)
#define cmplx_MUL_x(a_x, a_y, b_x, b_y ) (a_x*b_x - a_y*b_y)
#define cmplx_MUL_y(a_x, a_y, b_x, b_y ) (a_x*b_y + a_y*b_x)
#define cmplx_mul_x(a_x, a_y, b_x, b_y) (a_x*b_x - a_y*b_y)
#define cmplx_mul_y(a_x, a_y, b_x, b_y) (a_x*b_y + a_y*b_x)
#define cmplx_add_x(a_x, b_x) (a_x + b_x)
#define cmplx_add_y(a_y, b_y) (a_y + b_y)
#define cmplx_sub_x(a_x, b_x) (a_x - b_x)
#define cmplx_sub_y(a_y, b_y) (a_y - b_y)
#define cm_fl_mul_x(a_x, b) (b*a_x)
#define cm_fl_mul_y(a_y, b) (b*a_y)


//void twiddles8(TYPE a_x[8], TYPE a_y[8], int i, int n){
//    int reversed8[8] = {0,4,2,6,1,5,3,7};
//    int j;
//    TYPE phi, tmp, phi_x, phi_y;
//
//    twiddles:for(j=1; j < 8; j++){
//        phi = ((-2*PI*reversed8[j]/n)*i);
//        phi_x = cos(phi);
//        phi_y = sin(phi);
//        tmp = a_x[j];
//        a_x[j] = cmplx_M_x(a_x[j], a_y[j], phi_x, phi_y);
//        a_y[j] = cmplx_M_y(tmp, a_y[j], phi_x, phi_y);
//    }
//}
//////END TWIDDLES ////

#define FF2(a0_x, a0_y, a1_x, a1_y){			\
    TYPE c0_x = a0_x;		\
    TYPE c0_y = a0_y;		\
    a0_x = cmplx_add_x(c0_x, a1_x);	\
    a0_y = cmplx_add_y(c0_y, a1_y);	\
    a1_x = cmplx_sub_x(c0_x, a1_x);	\
    a1_y = cmplx_sub_y(c0_y, a1_y);	\
}

#define FFT4(a0_x, a0_y, a1_x, a1_y, a2_x, a2_y, a3_x, a3_y){           \
    TYPE exp_1_44_x;		\
    TYPE exp_1_44_y;		\
    TYPE tmp;			\
    exp_1_44_x =  0;		\
    exp_1_44_y =  -1;		\
    FF2( a0_x, a0_y, a2_x, a2_y);   \
    FF2( a1_x, a1_y, a3_x, a3_y);   \
    tmp = a3_x;			\
    a3_x = a3_x*exp_1_44_x-a3_y*exp_1_44_y;     	\
    a3_y = tmp*exp_1_44_y - a3_y*exp_1_44_x;    	\
    FF2( a0_x, a0_y, a1_x, a1_y );                  \
    FF2( a2_x, a2_y, a3_x, a3_y );                  \
}

#define N 1024
TYPE XR0[N];
TYPE XR1[N];
TYPE XR2[N];
TYPE XR3[N];
TYPE XI0[N];
TYPE XI1[N];
TYPE XI2[N];
TYPE XI3[N];
TYPE YR0[N];
TYPE YR1[N];
TYPE YR2[N];
TYPE YR3[N];
TYPE YI0[N];
TYPE YI1[N];
TYPE YI2[N];
TYPE YI3[N];

void fft4(){
    int i;
    for(i = 0; i < N; i++){
    please_map_me();
        TYPE a0_x = XR0[i];
        TYPE a0_y = XI0[i];
        TYPE a1_x = XR1[i];
        TYPE a1_y = XI1[i];
        TYPE a2_x = XR2[i];
        TYPE a2_y = XI2[i];
        TYPE a3_x = XR3[i];
        TYPE a3_y = XI3[i];
        FFT4(a0_x, a0_y, a1_x, a1_y, a2_x, a2_y, a3_x, a3_y);
        YR0[i] = a0_x;
        YI0[i] = a0_y;
        YR1[i] = a1_x;
        YI1[i] = a1_y;
        YR2[i] = a2_x;
        YI2[i] = a2_y;
        YR3[i] = a3_x;
        YI3[i] = a3_y;
    }
}



/**
@file main.cpp
 
@brief Pedometer implementation using the sensor Triple Axis Accelerometer Breakout - MMA8452Q
 
*/
/**
@file main.h
@brief Header file containing functions prototypes and global variables.
@brief Implementation of a Pedometer using the accelerometer MMA8452Q, Nokia 5110 display and the mbed LPC1768.
@brief Revision 1.5.
@author Edson Manoel da Silva
@date   May 2015
*/
#define SIZE  1024

int acc_vector[SIZE];/*!< Variable for check if a step was performed */

int acceleration_x[SIZE];
int acceleration_y[SIZE];
int acceleration_z[SIZE];
int acc_avg_x[SIZE];
int acc_avg_y[SIZE];
int acc_avg_z[SIZE];

void pedometer(){
	for(int i=0;i<SIZE;i++){
#ifdef CGRA_COMPILER
       please_map_me();
#endif      
		acc_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
	}
}




int main(){
 for(int i=0;i<SIZE;i++){
    acceleration_x[i] = 3*i;
    acceleration_y[i] = 3*i+1;
    acceleration_z[i] = 3*i+2;
    
    acc_avg_x[i] = i;
    acc_avg_y[i] = i;
    acc_avg_z[i] = i;
 }

pedometer();

return 0;
}

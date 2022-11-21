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
 #define SQRT_MAGIC_F 0x5f3759df 

typedef struct Acceleration Acceleration;
struct Acceleration {
    int x;
    int y;
    int z;
};
//   
/**  
@namespace lcd
@brief GPIO output for Nokia 5110 Display
@brief  VCC (p7),SCE(p8),RST(p9),D/C(p10),MOSI(p11),SCLK(p13),LED(p26)
// */
// N5110 lcd(p7,p8,p9,p10,p11,p13,p26);

// /**  
// @namespace mma8452
// @brief GPIO output for mma8452 accelerometer sensor
// @brief SDA(p28), SCL(p27)
// */
// MMA8452 mma8452(p28,p27);

// /**  
// @namespace leds
// @brief Debug leds
// */
// BusOut leds(LED4,LED3,LED2,LED1);

// /**  
// @namespace buzzer
// @brief PWM for the buzzer
// */
// PwmOut buzzer(p21);

// /**  
// @namespace serial
// @brief Serial interface
// */
// Serial serial(USBTX,USBRX);

// /**  
// @namespace timer1
// @brief First timer used
// */
// Timeout timer1;

// *  
// @namespace timer2
// @brief Second timer used

// Timeout timer2;

// /**  
// @namespace timer3
// @brief Chronometer timer
// */
// Ticker timer3;

// /**  
// @namespace local
// @brief Create Local File System
// */
// LocalFileSystem local("local"); // create local filesystem

// /**  
// @namespace I1
// @brief Create the interrupt for Transient Detection
// @brief I1(p15)
// */
// InterruptIn I1(p15);

// /**  
// @namespace I2
// @brief Create the interrupt for Pulse(Tap) Detection
// @brief I2(p16)
// */
// InterruptIn I2(p16);

/**  
@namespace mma8452
@brief Accleration structure declared in MMA8452 class
*/
Acceleration acceleration;  

/**  
@namespace mma8452
@brief Accleration average structure declared in MMA8452 class
*/
Acceleration acc_avg;

unsigned char second = 0; /*!< second flag set in TimerExpired3 */
unsigned char minute = 0; /*!< minute flag set in TimerExpired3 */
unsigned char hour = 0;/*!< hour flag set in TimerExpired3 */
unsigned char state = 0;/*!< state variable for the FSM */
unsigned char I1_flag = 0;/*!< Interrupt flag set in Transient Detection Interrupt */
unsigned char I2_flag = 0;/*!< Interrupt flag set in Pulse(Tap) Detection Interrupt */
unsigned char timerFlag1 = 0;/*!< Interrupt flag set in Timer1 */
unsigned char timerFlag2 = 0;/*!< Interrupt flag set in Timer2 */
unsigned char aux=0;/*!< Auxiliar for checking if the user stopped using the device */

char Int_SourceSystem =0;/*!< Variable used to read the MMA8452Q Interrupt Source Register */
char Int_SourceTrans=0;/*!< Variable used to clear the MMA8452Q Interrupt Registers */

unsigned char length;/*!< Variable used to check the string length to be printed in the LCD */
char buffer[14];/*!< Buffer used for printing strings on the display */

int step[SIZE]={};/*!< Variable used to ccalculate the steps */
//int step;// = 0;
//int km;// = 0;/*!< Variable used to ccalculate the kilometers */
int km[SIZE]={};
//int km = 0;
//int km_day[30] = {0,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6};
//int km_day[30] = {};/*!< Variable used to ccalculate the kilometers per day */

int acc_vector[SIZE];/*!< Variable for check if a step was performed */
//int acc_vector = 0;
//int sub_x = 0;/*!< Variable used for calibration */
//int sub_y = 0;!< Variable used for calibration 
//int sub_z = 0;/*!< Variable used for calibration */
int i;
int sub_x;
int sub_y;
int sub_z;
int acceleration_x[SIZE];
int acceleration_y[SIZE];
int acceleration_z[SIZE];
int acc_avg_x[SIZE];
int acc_avg_y[SIZE];
int acc_avg_z[SIZE];
/**
Set a flag to alert that a Transient Detection Interrupt has ocurred
*/
void Interrupt();

/**
Set a flag to alert that a Pulse Detection Interrupt has ocurred
*/
void Interrupt2();

/**
Blind the LEDS for state machine error alert
*/
void error();

/**
Set a flag to alert that a Timer1 Interrupt has ocurred
*/
void TimerExpired1();

/**
Set a flag to alert that a Timer2 Interrupt has ocurred
*/
void TimerExpired2();

/**
Performs the calculation for the chronometer time
*/
void TimerExpired3();

/**
Saves the data collected in the stepping count to the flash disk
@param date - the date of the data
@param data1 - steps
@param data2 - Kilometer
*/
void writeDataToFile(char *date,int data1,int data2);
int xx=0;
                    int xhalf ;





// int n = SIZE;
// int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
// int i,j;
// void gemm(){



//    int i,j,k;

//    for (i=0;i<SIZE; i++)
//       for (j=0; j<SIZE; j++)
//         for (k=0;k<SIZE; k++){
//           // #ifdef CGRA_COMPILER
//           //  please_map_me();
//           //  #endif
//            A[i][j] = A[i][j] + B[i][k]* C[k][j];
//                //A[i][j] = A[i][j] + B[i][4*k]* C[4*k][j] + B[i][4*k+1]* C[4*k+1][j]
//                  //   + B[i][4*k+2]* C[4*k+2][j] + B[i][4*k+3]* C[4*k+3][j];
//             }


// }
//__attribute__((annotate("step:1,km:1,acc_vector:1"))) 
void pedometer()
{
    /// Finite State Machine that cotrols the whole system
    //Setting the initial state. Avoiding rubish values
    state = 0;
    I1_flag = 0;
    I2_flag = 0;
    //int step;
    //int km;
    
    // Adressing the rise of pin 15(which is connected to I1 of Sensor)to the interruption routine 1
    // I1.rise(&Interrupt);
    
    // Adressing the rise of pin 16(which is connected to I2 of Sensor)to the interruption routine 2
    // I2.rise(&Interrupt2);  

    // while(1)
    // {    
    //     // Main state machine 
    //     switch (state)
    //     {
    //         // Keep the device sleeping if no interruption is generated
            
    //         // Wait for an user command befor the timer counting ends
            
    //         // Steps counting Screen. Also save data.
    //         case 2:
    //         {
                    // Changes the scale to 2g, the ODR to 800Hz and set the output data to be read from the High Pass Filter
                    // mma8452.transient_counting();                                                
                    // lcd.clear();
                    // buzzer = 0.5;
                    // lcd.printString("Calibrating...",0,0);
                    // wait(5);
                    // Take a average of the 50 values of the device before counting steps. Kind of calibration
                    //acc_avg = mma8452.average();  
                    // acc_avg = 
                    // buzzer = 0;
                    // step= 0; 
		    // km = 0;
                   // km [0]= 0;
                    // leds = 0x04; 
                    aux = 0; 
                    timerFlag2 = 0;
                   // acc_vector =0;
                                        // timer3.attach(&TimerExpired3,1);
                    // While the timer does not end, keep counting
                     // while(!timerFlag2)
                    //
            //          for (i=0;i<100; i++){
            //               #ifdef CGRA_COMPILER
            //                please_map_me();
            //                #endif
            //               step=step+i;
            //    //A[i][j] = A[i][j] + B[i][4*k]* C[4*k][j] + B[i][4*k+1]* C[4*k+1][j]
            //      //   + B[i][4*k+2]* C[4*k+2][j] + B[i][4*k+3]* C[4*k+3][j];
            // }
//int i=10;
//                    for(int j=0;j<16;j++){
                     for(int i=0;i<SIZE;i++)
                     {    
                         
           #ifdef CGRA_COMPILER
           please_map_me();
           #endif
                    //     // acceleration = mma8452.readValues();   // read current values 
                    //     sub_x = acceleration_x[i]- acc_avg_x[i];
                      //   sub_y = acceleration_y[i]- acc_avg_y[i];
                     //   sub_z = acceleration_z[i]-acc_avg_z[i];
                    //     // acc_vector = (pow(sub_x,2.0)+pow(sub_y,2.0)+pow(sub_z,2.0)); 
                       // acc_vector[i] = sub_x * sub_x+  sub_y * sub_y+ sub_z * sub_z; 
                        acc_vector[i] = (acceleration_x[i]- acc_avg_x[i]) * (acceleration_x[i]- acc_avg_x[i])+  (acceleration_y[i]- acc_avg_y[i]) * (acceleration_y[i]- acc_avg_y[i])+ (acceleration_z[i]-acc_avg_z[i]) * (acceleration_z[i]-acc_avg_z[i]) ;
                        

                    //     // acc_vector = sqrt(acc_vector);
      // // #defin3e SQRT_MAGIC_F 0x5f3759d8f 3
                    // //  int  sqrt2(const int x)
                    // // {
                    // //   const int xhalf = 0.5f*x;
                     
                    // //   union // get bits for inting value
                    // //   {
                    // //     int x;
                    // //     int i;
                    // //   } u;
                    // //   u.x = x;
                    // //   u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
                    // //   return x*u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy 
                    // // }

                        
                    //     // // If the acceleration vector is greater than 0.15, add the steps
                //         if(acc_vector[i]  > 15 && i>1)
                //         {
                //              step[i] = step[i-1] + 2;
                //              // step = step + 2;
			    //  // Runing
                //              if (acc_vector[i]  > 100)
                //                 km[i] = km[i-1]+ 2;
				// // km = km + 2;
                //              // Walking
                //              else
                //                km[i] = km[i-1] + 1;    
                //                 // km = km + 1;     
                //         }

                    //     // lcd.clear(); 
                              
                    //     // length = sprintf(buffer,"%6u steps",step); 
                    //     // if (length <= 14)                 
                    //     //     lcd.printString(buffer,0,0);  
                        
                    //     // length = sprintf(buffer,"%6.3f km",km); 
                    //     // if (length <= 14)                 
                    //     //     lcd.printString(buffer,0,1); 
                        
                    //     // length = sprintf(buffer,"%2u:%2u:%2u",hour, minute, second); 
                    //     // if (length <= 14)                 
                    //     //     lcd.printString(buffer,0,2);                             
                              
                    //     // // get current date    
                    //     // time_t seconds = time(NULL);  
                    //     // // Convert date to a string
                    //     // strftime(buffer, 14 , "%a-%d/%m/%Y", localtime(&seconds));
                    //     // lcd.printString(buffer,0,3);
                        
                    //     // // Avoiding counting the same steps twice                                       
                    //     // wait(0.65);
                        
                    //     // // If one stops the activity, starts the timer
                    //     // if ((acc_vector <0.15)&& (aux==0))
                    //     // {
                    //     //     timer2.attach(&TimerExpired2,20.0);
                    //     //     aux = 1;
                    //     // }
                    //     // // If one walks again, reset the timer
                    //     // else if ((acc_vector >0.15)&&(aux == 1))
                    //     // {
                    //     //     timer2.detach();
                    //     //     aux = 0;
                    //     // }

                     }
                     //}
                     //}  
                    // buzzer = 0.5;
                    // wait(3);
                    // buzzer = 0;
                    // Int_SourceTrans = mma8452.readByteFromRegister(TRANSIENT_SRC); 
                    // Int_SourceTrans = mma8452.readByteFromRegister(PULSE_SRC);
                    // Int_SourceSystem = 0;
                    // wait(0.2);
                    // timer3.detach();
                    // second = 0;
                    // minute = 0;
                    // hour = 0;
                    // I1_flag = 0;
                    // timerFlag2 = 0;
                    // // Saving data to local system File
                    // writeDataToFile(buffer,step,km);
                    // // Accumulating the steps per day value
                    // time_t seconds = time(NULL);
                    // strftime(buffer, 3 , "%u", localtime(&seconds));
                    // int value = atoi(buffer);   
                    // km_day[value]=km_day[value]+km;
                    // state = 0;
                    // break;
               // }
               // Graph Km/day Screen 
                      
        // }
    // }  


   //     int i,j,k;

   // for (i=0;i<SIZE; i++)
   //    for (j=0; j<SIZE; j++)
   //      for (k=0;k<SIZE; k++){
   //        // #ifdef CGRA_COMPILER
   //        //  please_map_me();
   //        //  #endif
   //         A[i][j] = A[i][j] + B[i][k]* C[k][j];
   //             //A[i][j] = A[i][j] + B[i][4*k]* C[4*k][j] + B[i][4*k+1]* C[4*k+1][j]
   //               //   + B[i][4*k+2]* C[4*k+2][j] + B[i][4*k+3]* C[4*k+3][j];
   //          }      
}



void Interrupt()
{
    /// Controls the Transient Detection Interrupt flag
    I1_flag = 1;
}

void Interrupt2()
{
    /// Controls the Pulse(Tap)Detection Interrupt flag
    I2_flag = 1;
}

void error() 
{
    /// Error function. In case of error of the state machine
//     while(1) 
//     { 
//         lcd.clear();
//         lcd.printString("FSM Error!",0,0);
//     } 
 }

void TimerExpired1()
{
    /// Timer 1 flag
    timerFlag1 = 1;
}

void TimerExpired2()
{
    /// Timer 2 Flag
    timerFlag2 = 1;
}

void TimerExpired3()
{
    /// Calculates the chronometer time
    second = second + 1;
    if (second > 60)
    {
        second = 0;
        minute = minute + 1;
        if (minute > 60)
        {
            hour = hour + 1;
            minute = 0;
        }
   
    }    
}

void writeDataToFile(char *date,int data1,int data2)
{
    /// Saves the km and steps data to flash disk
    // FILE *fp = fopen("/local/log.txt", "a");
    // // Create the txt file
    // fprintf(fp,"Date: %s\n",date);
    // fprintf(fp,"Steps = %6u\n",data1);
    // fprintf(fp,"Km = %6.3f\n \n",data2);
    // fclose(fp);
}

int main(){
 for(int i=0;i<16;i++){    
    sub_x = 0;
    sub_y = 0;
    sub_z = 0;

    acceleration_x[i] = 3*i;
    acceleration_y[i] = 3*i+1;
    acceleration_z[i] = 3*i+2;
    
    acc_avg_x[i] = i;
    acc_avg_y[i] = i;
    acc_avg_z[i] = i;
 }

pedometer();
//gemm();
return 0;
}

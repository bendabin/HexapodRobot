/*
  
File Name:-  servo_exercise.c
Author:-     Ben Dabin  
Date:-       29/10/13
Version:-    3.0.0

          Program Description:

 This program is the main C code source file for the embedded systems robot.
 The following program deals the walking and steping sequences. It also deals 
 accepting user input coming from a USART 

*/

//Include files
#include <iom1280.h>
#include <intrinsics.h>
#include <stdarg.h>
#include "binary.h"
#include "robot.h"
#include "LCD.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

//Definitions
#define TEST_ANGLE 0
#define ANGLE_STEP 10
#define ANGLE_MAX 130
#define ON_TIME_MAX 12500
#define C_SERVO_MIN 470
#define C_SERVO_MAX 1500
#define C_SERVO_STEP 50
#define RX_complete UCSR1A_Bit7

// Initial sweep angle
#define INITIAL_ANGLE   15
#define INITIAL_ANGLES2 180
// Final sweep angle
#define FINAL_ANGLE 45
#define FINAL_ANGLES2 115
// Start angle for leg extension
#define RETRACT_ANGLE 0
// Final angle for leg extension
#define EXTEND_ANGLE 60

// Small delay between servo action in step (in cycles)
#define STEP_DELAY 1e6
#define INTER_STEP_DELAY 8e4

//define control buttons

#define BUTTON1 PINJ_Bit6
#define BUTTON2 PINJ_Bit5

//Function Declarations 
void setup();
void servo_ex();
void servo_angle(int smin, int smax);
void servo_angle_c(int min, int max);
void servo_drive(char servo, int ocr);
void servo_drive_c(char servo, int on_time, int off_time);
int angle2OCR(int angle);
void angle2count(int angle, int* on_time, int* off_time);
void servo_c_index(char servo, char state);
void send2LCD(char* data, char adress, char length);
void stepLeg1(char STATE);
void stepLeg2(char STATE);
void stepLeg3(char STATE);
void stepLeg4(char STATE);
void stepLeg5(char STATE);
void stepLeg6(char STATE);
void STAND_HEIGHT(int angle);
void reset_position(void);

//****Functions for walking steps
void walk1_STEP(int on_time, int off_time);
void walk2_STEP(int on_time, int off_time);
void walk3_FORWARD(int on_time, int off_time);
void walk4_FORWARD(int on_time, int off_time);
void backwards_STEP1(int on_time, int off_time);
void backwards_STEP2(int on_time, int off_time);

//Global variable declarations
char STATE,  count, RX_value, height_count;
char data[16];

/* setup configuration        */
void setup()
{
      TCCR1A= b10101010;                   //setup 16 bit PWM timer 1
      TCCR1B= b00011011;                   //prescale 8, set top as 
      
      TCCR3A= b10101010;                   //setup 16 bit PWM timer 2
      TCCR3B= b00011011;                   //prescale 8, set top as
      
      TCCR4A= b10101010;                  //setup 16 bit PWM timer 3
      TCCR4B= b00011011;                  //prescale 8, set top as
      
      TCCR5A= b10101010;                  //setup 16 bit PWM timer 4
      TCCR5B= b00011011;                  //prescale 8, set top as
           
      ICR1= PWM_MAX_TIME;                 //setup PWM time to 20ms
      ICR3= PWM_MAX_TIME;                 //setup PWM time
      ICR4= PWM_MAX_TIME;                 //setup PWM time
      ICR5= PWM_MAX_TIME;                 //setup PWM time
      DDRC= 0x00;                         //set port to input
      DDRA= 0xFF;                         //set port to output   
      DDRJ_Bit5= 0;                       //set to input
      DDRJ_Bit6= 0;                       //set to input
      count= 0;
      lcd_int();                          //intialise LCD
      lcd_write_int(0x00);                //clear LCD
      lcd_write_int(0x01);                //select LCD line1
      reset_position();                   //set robot servos in home position
      sprintf(data, "HEXAPOD");           //forward Mode selected
      send2LCD(data,0x01, strlen(data));  //write string to LCD
      sprintf(data, "ROBOT");                 
      send2LCD(data,0xC0, strlen(data));
      __delay_cycles(8e6);
      
      //************SETUP USART**************
      UBRR1L = 51;            //9600 baud
      UCSR1B = b10011000;     //enable USART interrupt
      UCSR1C = b10000110;
      //*************************Setup USART interrupt*********************
      global_int_enable= 1;   //turn on golbal interrupts  
}
/*  
    Main Program starts here
*/  
void main(void)
{ 
   
        char STATE_FLAG= 0;
        int i, angle, on_time, off_time, off_set_angle;
        off_set_angle= 0;
        setup();                            //intialise 
        sprintf(data, "SELECT");            //Display menu function message
        send2LCD(data,0x01, strlen(data));
        sprintf(data, "MODE");
        send2LCD(data,0xC0, strlen(data));
          
        while(1)      
	{                 

    /*  
        This section of code enables the menu selection system which 
        is controlled using BUTTON1 and BUTTON2
    
    */
          
         if(BUTTON1==0)               //if button one selected
         {
                 while(BUTTON1==0);   //debounce switch on release
                 count++;             //increment counter on button press
                 if(count==1)
                 {
                    sprintf(data, "FORWARDS");      //forward Mode selected
                    send2LCD(data,0x01, strlen(data));
                    STATE= 'F';                     //change state to Forward
                 }
                 else if(count== 2)
                 {
                    sprintf(data, "BACKWARDS");   //Backwards Mode selected
                    send2LCD(data,0x01, strlen(data));
                    STATE= 'B';                   //change state to backwards
                 } 
                 else if(count== 3)
                 {
                    sprintf(data, "RIGHT");             //Right Mode selected
                    send2LCD(data,0x01, strlen(data));
                    STATE= 'R';                         //change state to Right
                 }
                 else if(count==4)
                 {
                    sprintf(data, "LEFT");            //Left Mode selected
                    send2LCD(data,0x01, strlen(data));
                    STATE= 'L';                       //change state to Left
                 }
                 else if(count>4)
                 {
                    sprintf(data, "SELECT");          //forward Mode selected
                    send2LCD(data,0x01, strlen(data));
                    sprintf(data, "MODE");
                    send2LCD(data,0xC0, strlen(data));
                    count=0;
                    STATE= 0;                         //change mode state to 0          
                 }                 
                 if(count>0)                          //if count>0 do this
                 {
                    do
                    {
                      if(BUTTON2==0)            //if button selected
                      {
                        while(BUTTON2==0);    
                        STATE_FLAG=1;         //change state to mode enabled
                        
                      }
                      if(BUTTON1 ==0)        //change state to next state
                      {
                        while(BUTTON1==0)
                        STATE_FLAG= 2;  
                      }
                      
                    }while(STATE_FLAG==0);   //loop when state not zero  
                    
                    if(STATE_FLAG== 1)      //if state equal to 1 enable mode
                    {
                        STATE_FLAG=0;
                        sprintf(data, "ENABLED");
                        send2LCD(data,0xC0, strlen(data));
                    }
                    else
                    {
                         STATE_FLAG= 0;
                         sprintf(data, "NEXT");  //forward Mode selected
                         send2LCD(data,0x01, strlen(data));
                         sprintf(data, "MODE");
                         send2LCD(data,0xC0, strlen(data));
                         STATE= 0;
                    }
               }
         }
   /*
          The following code here deals with accepting user input being 
          recieved from an Android Smartphone GUI. This also deals with 
          the Robots menu functions.
                    
   */

         else if(RX_buffer_full)          //wait for smartphone to recieve data
         {  
            while(!RX_buffer_full);
            RX_value= UDR1;
            if(RX_value== 'F')            //If 'F' received Forwards enabled
            {
               while(BUTTON1==0);
               sprintf(data, "FORWARDS"); //forward Mode selected
               send2LCD(data,0x01, strlen(data));
               sprintf(data, "ENABLED");
               send2LCD(data,0xC0, strlen(data));
               STATE= 'F';                //change state to forwards
            }            
            else if(RX_value== 'B')       //If 'B' received Backwards enabled
            {
              sprintf(data, "BACKWARDS"); //forward Mode selected
              send2LCD(data,0x01, strlen(data));
              sprintf(data, "ENABLED");
              send2LCD(data,0xC0, strlen(data));
               STATE= 'B';                //change state to backwards
            }
            else if(RX_value== 'C')       //If 'C' recieved Stop mode enabled
            {
              sprintf(data, "STOP");      //Stop Mode selected
              send2LCD(data,0x01, strlen(data));
              sprintf(data, "ENABLED");
              send2LCD(data,0xC0, strlen(data));
              STATE= 'C';                 //change state to C for 
            }
            else if(RX_value=='R')        //If 'R' received Right mode enabled
            {
              sprintf(data, "RIGHT");     //Right Mode selected
              send2LCD(data,0x01, strlen(data));
              sprintf(data, "ENABLED");
              send2LCD(data,0xC0, strlen(data));
              STATE= 'R';                 //change state to 'R'
            } 
            else if(RX_value=='L')        //If 'L' received Left mode enabled
            {
              sprintf(data, "LEFT");      //Left Mode selected
              send2LCD(data,0x01, strlen(data));
              sprintf(data, "ENABLED");
              send2LCD(data,0xC0, strlen(data));
              STATE= 'L';                 //change state to 'L'
            }  
             else if(RX_value=='O')       //If 'O' recieved Reset mode enabled
            {
              sprintf(data, "RESET");     //Reset Mode selected
              send2LCD(data,0x01, strlen(data));
              sprintf(data, "ENABLED");
              send2LCD(data,0xC0, strlen(data));
              STATE= 'O';                  //change state to 'O'
            }  
            else if(RX_value == 'U')      //If 'U' recieved 
            {                          
              angle= angle + 20;          //increase height of hexapod frame
              STATE= 'U';                 //change state to 'U'
            }
            else if(RX_value == 'D')     //If 'D' recieved
            {
              angle= angle - 20;         //lower height of hexapod frame
              STATE= 'D';                //change state to 'D'
            }   
         }     
                     
   /*
      This part of the code deals with decoding the State machine state  
      selected from either the Android Smartphone or control button option
   */
    
            switch(STATE)           //selects state based on character received
            {                     
                  case  'F'  :            //forwards walk mode                                         
                        
                    if(RX_complete)       //check if USART STATE interrupted
                    {
                      STATE= UDR1;        //change USART to newer state
                      reset_position();   //reset robot leg position  
                      break;              //break out of current mode
                     } 
                      stepLeg3(STATE);    //move leg3
                      __delay_cycles(INTER_STEP_DELAY); //legstep delay
                   if(RX_complete)        //check if USART STATE interrupted
                    {
                      STATE= UDR1;        //change USART to newer state
                      reset_position();   //reset robot leg position
                      break;              //break out of current mode
                    }
                       stepLeg4(STATE);    //move leg4
                     __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                    if(RX_complete)       //check if USART STATE interrupted
                    {
                      STATE= UDR1;        //change USART to newer state
                      reset_position();   //reset robot leg position  
                      break;              //break out of current mode
                    }
                     stepLeg2(STATE);     //move leg2
                    __delay_cycles(INTER_STEP_DELAY); //legstep delay
                      if(RX_complete)      //check if USART STATE interrupted
                    {
                      STATE= UDR1;        //change USART to newer state
                      reset_position();   //reset robot leg position
                      break;              //break out of current mode
                    }
                      stepLeg5(STATE);    //move leg5
                      __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                    if(RX_complete)       //check if USART STATE interrupted
                    {
                      STATE= UDR1;        //change USART to newer state
                      reset_position();   //reset robot leg position
                      break;              //break out of current mode
                    }
                    
                      stepLeg1(STATE);    //move leg1
                      __delay_cycles(INTER_STEP_DELAY); //legstep delay
                     if(RX_complete)      //check if USART STATE interrupted
                     {
                       STATE= UDR1;       //change USART to newer state
                       reset_position();  //reset robot leg position
                       break;             //break out of current mode
                     } 
                     
                      stepLeg6(STATE);     //move leg6
                      __delay_cycles(INTER_STEP_DELAY); //legstep delay
                     if(RX_complete)        //check if USART STATE interrupted
                    {
                        STATE= UDR1;        //change USART to newer state
                        reset_position();   //reset robot leg position
                        break;              //break out of current mode
                     } 
                     
                        break;              //break out of current mode           
                     
                  case 'B'  :               //backwards mode                  
                         
                           if(RX_complete)  //check if USART STATE interrupted
                          {                            
                              STATE= UDR1;      //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;            //break out of current mode
                           } 
                             stepLeg3(STATE);   //move leg 3
                            __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           stepLeg4(STATE);     //move leg 4
                           if(RX_complete)    //check if USART STATE interrupted
                           {                             
                              STATE= UDR1;      //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;            //break out of current mode
                            }                          
                           __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           stepLeg2(STATE);                   //move leg 2
                            if(RX_complete)  //check if USART STATE interrupted            
                           {                             
                               STATE= UDR1;       //change USART to newer state
                               reset_position();  //reset robot leg position
                               break;             //break out of current mode
                            }                 
                           __delay_cycles(INTER_STEP_DELAY); //legstep delay
                           stepLeg5(STATE);      //move leg 5
                           if(RX_complete)    //check if USART STATE interrupted
                           {                              
                              STATE= UDR1;      //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;            //break out of current mode
                            }   
                          __delay_cycles(INTER_STEP_DELAY); //legstep delay                               
                          stepLeg1(STATE);      //move leg 1
                           if(RX_complete)    //check if USART STATE interrupted
                           {
                              
                              STATE= UDR1;      //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;             //break out of current mode
                            }   
                           __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           stepLeg6(STATE);     //move leg 6
                           if(RX_complete)    //check if USART STATE interrupted
                           {
                              
                                STATE= UDR1;      //change USART to newer state
                                reset_position(); //reset robot leg position
                                break;             //break out of current mode
                          }   
                          break;                  
                          
            case 'R' :                            //right mode selected
                          if(RX_complete)    //check if USART STATE interrupted
                          {
                           
                                 STATE= UDR1;     //change USART to newer state
                                 reset_position(); //reset robot leg position
                                 break;           //break out of current mode
                           } 
                             stepLeg3(STATE);     //move leg 3
                            __delay_cycles(INTER_STEP_DELAY); //legstep delay
                           
                           stepLeg4(STATE);     //move leg 4
                           if(RX_complete)    //check if USART STATE interrupted
                           {
                              
                               STATE= UDR1;      //change USART to newer state
                               reset_position(); //reset robot leg position
                               break;           //break out of current mode
                            }                             
                           __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           
                           stepLeg2(STATE);   //move leg 2
                            if(RX_complete)   //check if USART STATE interrupted
                           {
                             
                                STATE= UDR1;      //change USART to newer state
                                reset_position(); //reset robot leg position
                                break;            //break out of current mode
                            }                 
                           __delay_cycles(INTER_STEP_DELAY); //legstep delay
                           
                           stepLeg5(STATE);   //move leg 5
                           if(RX_complete)   //check if USART STATE interrupted
                           {
                              
                                  STATE= UDR1;    //change USART to newer state
                                  reset_position(); //reset robot leg position
                                  break;           //break out of current mode
                            }   
                          __delay_cycles(INTER_STEP_DELAY); //legstep delay
                          
                          stepLeg1(STATE);     //move leg 1
                           if(RX_complete)    //check if USART STATE interrupted
                             {
                              STATE= UDR1;        //change USART to newer state
                              reset_position();   //reset robot leg position
                              break;               //break out of current mode
                            }  
                          __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           
                          
                           stepLeg6(STATE);      //move leg 6
                           if(RX_complete)   //check if USART STATE interrupted
                           {
                             
                              STATE= UDR1;       //change USART to newer state
                              reset_position();  //reset robot leg position
                              break;             //break out of current mode
                            } 
                            break; 
           
            
            case 'L' :                            //right mode selected
                          if(RX_complete)     //check if USART STATE interrupted
                          {
                            
                               STATE= UDR1;      //change USART to newer state
                               reset_position(); //reset robot leg position
                               break;            //break out of current mode
                           } 
                             stepLeg3(STATE);     //move leg 3
                            __delay_cycles(INTER_STEP_DELAY); //legstep delay
                            
                           if(RX_complete)    //check if USART STATE interrupted
                           {
                             
                                STATE= UDR1;      //change USART to newer state
                                reset_position(); //reset robot leg position
                                break;            //break out of current mode
                            } 
                           stepLeg4(STATE);       //move leg 4
                           __delay_cycles(INTER_STEP_DELAY);  //legstep delay
                           
                           
                            if(RX_complete)   //check if USART STATE interrupted
                           {
                              
                              STATE= UDR1;        //change USART to newer state
                              reset_position();  //reset robot leg position
                              break;             //break out of current mode
                            } 
                            stepLeg2(STATE);       //move leg 2
                           __delay_cycles(INTER_STEP_DELAY); //legstep delay
                          
                           if(RX_complete)  //check if USART STATE interrupted
                           {
                              
                              STATE= UDR1;       //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;            //break out of current mode
                            }   
                            stepLeg5(STATE);      //move leg 5
                          __delay_cycles(INTER_STEP_DELAY);   //legstep delay                              
                          
                           if(RX_complete)   //check if USART STATE interrupted
                           {
                             
                              STATE= UDR1;      //change USART to newer state
                              reset_position(); //reset robot leg position
                              break;            //break out of current mode
                            }   
                           stepLeg1(STATE);     //move leg 1
                           __delay_cycles(INTER_STEP_DELAY);   //legstep delay  
                           
                           if(RX_complete)   //check if USART STATE interrupted
                           {
                              STATE= UDR1;    //change USART to newer state
                              break;          //break out of current mode
                            } 
                             stepLeg6(STATE);   //move leg 6
                            __delay_cycles(INTER_STEP_DELAY);   //legstep delay  
                            break;            //break out of current mode
              

            case 'U' :  STAND_HEIGHT(angle);    //increase height of robot
                        break;
                    
                    
            case 'D' : STAND_HEIGHT(angle);     //decrease height of robot
                        break; 
                        
            /*              If stop enabled jump here          */            
                 
            case 'C' :    __delay_cycles(INTER_STEP_DELAY);     //legstep delay  
                          servo_drive(1, angle2OCR(120));//home position 
                          if(RX_complete)    //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode
                          }
                          __delay_cycles(8e4);    //delay                      
                          servo_drive(2, angle2OCR(0)); //home position servo2C
                          if(RX_complete)    //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;            //break out of current mode
                          }                          
                          angle2count((int)65, &on_time, &off_time);
                          servo_drive_c(1, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;           //break out of current mode                  
                          }
                          __delay_cycles(8e4);
                          servo_drive(3, angle2OCR(120));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                  
                          }                          
                           __delay_cycles(8e4);
                          servo_drive(4, angle2OCR(0));
                          if(RX_complete)
                          {
                            STATE= UDR1;  //change USART to newer state
                            break;        //break out of current mode                      
                          }
                          angle2count((int)65, &on_time, &off_time);
                          servo_drive_c(2, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                   
                          }
                         __delay_cycles(8e4);
                          servo_drive(5, angle2OCR(120));
                           __delay_cycles(8e4);
                          servo_drive(6, angle2OCR(0)); 
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                     
                          }
                          angle2count((int)65, &on_time, &off_time);
                          servo_drive_c(3, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                     
                          }
                          __delay_cycles(8e4);                                
                          servo_drive(7, angle2OCR(120));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                     
                          }
                           __delay_cycles(8e4);
                          servo_drive(8, angle2OCR(0));  
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;           //break out of current mode                  
                          }
                          angle2count((int)65, &on_time, &off_time);
                          servo_drive_c(4, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;           //break out of current mode                 
                          }
                          __delay_cycles(8e4);
                          servo_drive(9, angle2OCR(120));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;           //break out of current mode                  
                          }
                           __delay_cycles(8e4);
                          servo_drive(10, angle2OCR(0));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;           //break out of current mode                        
                          }
                          angle2count((int)120, &on_time, &off_time);
                          servo_drive_c(5, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;           //break out of current mode                    
                          }
                          __delay_cycles(8e4);
                          servo_drive(11, angle2OCR(120));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;     //change USART to newer state
                            break;           //break out of current mode                       
                          }
                           __delay_cycles(8e4);
                          servo_drive(12, angle2OCR(0));
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;   //change USART to newer state
                            break;          //break out of current mode                        
                          } 
                          angle2count((int)120, &on_time, &off_time);
                          servo_drive_c(6, on_time, off_time);
                          if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;    //change USART to newer state
                            break;          //break out of current mode                       
                          }
                          __delay_cycles(8e4);
                          break;    
                          
            case 'O' :    if(RX_complete)   //check if USART STATE interrupted
                          {
                            STATE= UDR1;       //change USART to newer state
                            break;             //break out of current mode                  
                          }
                          reset_position();   //reset robot leg position
                          break; 
            }						       
                           
        
        }
   }	  




//// TEST ROUTINES ////
/*

 The following function here was not used in the final program but this was used
 as a servo tester rountine for testing all the A and B servos. As servos were 
 replaced the rountine works by checking through all of the angles for 
 functionality purposes

*/
void servo_angle(int smin, int smax)
{
        int angle[NUM_ANGLES] = {0, 45, 90, 120};
	int ocr_cur;
	int servo, i;
	
	// Test to make sure we pass variables in range
	if(smin < 1)          //define min range of servos
		smin = 1;
	if(smax > 12)
		smax = 12;    //define max range of servos
	
	for(servo=smin; servo <= smax; servo++)
	{
                //increment angles
		for(i=0; i<NUM_ANGLES; i++)    //4 is number of angles to test
		{
			ocr_cur = angle2OCR(angle[i]);
			servo_drive(servo, ocr_cur);
			__delay_cycles(OCR_TEST_DELAY);
		}
                //decrement angles
		for(i=NUM_ANGLES-1; i>=0; i--)
		{			
			ocr_cur = angle2OCR(angle[i]);
			servo_drive(servo, ocr_cur);
			__delay_cycles(OCR_TEST_DELAY);
		} 
	}
}
//the following function below was not used as well but only used for testing
void servo_angle_c(int min, int max)
{
  int servo;
  int on_time, off_time;
  int angle [NUM_ANGLES_C] = {0, 45, 90, 120};

  for(servo=min; servo <= max; servo++)
  {
    for(int i=0; i<NUM_ANGLES_C; i++)
    {
      angle2count(angle[i], &on_time, &off_time);
      servo_drive_c(servo, on_time, off_time);
      __delay_cycles(OCR_TEST_DELAY);
    }
    
  }
}      
 
//// Excercise each servo on the robot. This drives each servo from
////  OCR_MIN to OCR_MAX in steps of OCR_STEP_SIZE

void servo_ex()
{
	int servo, ocr_cur;
	
		for(servo=1; servo <=12; servo++)
		{
			// step up from 0
			ocr_cur = OCR_MIN;
			while(ocr_cur < OCR_MAX)
			{
				servo_drive(servo, ocr_cur);
				ocr_cur += OCR_STEP_SIZE;	
				//delay here
				__delay_cycles(OCR_TEST_DELAY);
			}
			__delay_cycles(OCR_TEST_DELAY);
			//step down from 300
			ocr_cur = OCR_MAX;
			while(ocr_cur > OCR_MIN)
			{
				servo_drive(servo, ocr_cur);
				ocr_cur -= OCR_STEP_SIZE;
				__delay_cycles(OCR_TEST_DELAY);
			}
		}
}

/*
    This part converts and angle value into an on and off time count value,
    This is the most important part of the code used
*/
void angle2count(int angle, int* on_time, int* off_time)
{
  int on, off;
  on = (7 * angle + 470);
  off = ON_TIME_MAX - on;
  *on_time = on;
  *off_time = off;
}
 
//// UTILITY FUNCTIONS ////

int angle2OCR(int angle)
{
	//figure out what OCR value corresponds to this angle (in degrees)
	return (int)(angle * 1.44) + 100;
}
/*                        

     This function deals with the bit PWM software bit banging action of 
     the C servos determined by recieving the on and off times

*/  

void servo_drive_c(char servo, int on_time, int off_time)
{
  int counter, on_count, off_count;
  counter = 15;   //how many pulses
  
  while(counter>0)
  { 
    
    if(RX_complete)         //Check if USART interrupted
    {
      counter=0;          
    }

	switch(servo)           //what servo has been selected?
	{
	       case 1:				        //if leg 1 selected					
				LEG1_C_ON = 1;          //turn leg1 servoC on             
				on_count = on_time; 
				while(on_count>0)  //loop when count is greater than zero
				{
					PORTH_Bit6= 1; //make PORTH bit 6 high
					on_count--;    //decrement on count
				}               
				off_count = off_time; 
				while(off_count>0) //loop when count is greater than zero
				{
					 PORTH_Bit6= 0; //make PORTH bit 6 low
					 off_count--;  //decrement off count
				} 
				break;				
		case 2:			          //if leg 2 selected
				LEG2_C_ON = 1;    //turn leg2 servoC on             
				on_count = on_time;              
				while(on_count>0)       //loop when count is greater than zero
				{
					PORTK_Bit2= 1;    //make PORTK bit 2 high
					on_count--;       //decrement on count    
				}               
				off_count = off_time;   
				while(off_count>0)    //loop when count is greater than zero
				{
					 PORTK_Bit2= 0;  //make PORTK bit 2 low
					 off_count--;    //decrement off count  
				} 
				break;	  
		case 3:				        //if leg 3 selected
				LEG3_C_ON = 1;          //turn leg3 servoC on         
				on_count = on_time;
				while(on_count>0)       //loop when count is greater than zero
				{
					PORTK_Bit0= 1;  //make PORTK bit 0 high
					on_count--;     //decrement on count 
				}               
				off_count = off_time;
				while(off_count>0)      //loop when count is greater than zero
				{
					 PORTK_Bit0= 0;  //make PORTK bit 0 low
					 off_count--;    //decrement off count 
				} 
				break;
		case 4:                                //if leg 4 selected
				LEG4_C_ON = 1;         //turn leg4 servoC on    
				on_count = on_time;
				while(on_count>0)   //loop when count is greater than zero
				{
					PORTK_Bit1= 1;    //make PORTK bit 1 high
					on_count--;       //decrement on count 
				}               
				off_count = off_time;
				while(off_count>0)    //loop when count is greater than zero
				{
					PORTK_Bit1= 0;  //make PORTK bit 1 low
					 off_count--;     //decrement off count
				} 
				break;
		case 5:                                   //if leg 5 selected    
				LEG5_C_ON = 1;           //turn leg5 servoC on  
				on_count = on_time;
				while(on_count>0)  //loop when count is greater than zero
				{
					PORTB_Bit4= 1;  //make PORTB bit 4 high
					on_count--;     //decrement on count
				}               
				off_count = off_time;     
				while(off_count>0)  //loop when count is greater than zero
				{
					PORTB_Bit4= 0; //make PORTB bit 4 low
					 off_count--;    //decrement off count
				} 
				break;
		case 6:					
				LEG6_C_ON = 1;         //if leg 6 selected         
				on_count = on_time;
				while(on_count>0)     //loop when count is greater than zero
				{
					PORTG_Bit5= 1;    //make PORTG bit 5 low
					on_count--;       //decrement on count
				}               
				off_count = off_time;
				while(off_count>0) //loop when count is greater than zero
				{
					PORTG_Bit5= 0;    //make PORTG bit 5 low
					 off_count--;     //decrement off count
				} 
				break;			
		 default:
				break;									 
  }
  counter--;              //decrement master delay counter
  }
  
}

void servo_c_index(char servo, char state)
{
  switch(servo)
  {
  case 1:
    LEG1_C_ON = 1;
    PORTH_Bit6 = state;
    break;
  case 2:
    LEG2_C_ON = 1;
    PORTK_Bit2 = state;
    break;
  case 3:
    LEG3_C_ON = 1;
    PORTK_Bit0 = state;
    break;
  case 4:
    LEG4_C_ON = 1;
    PORTK_Bit1 = state;
    break;
  case 5: 
    LEG5_C_ON = 1;
    PORTB_Bit4 = state;
    break;
  case 6: 
    LEG6_C_ON = 1;
    PORTG_Bit4 = state;
    break;  
  default:
    break;    
    
  }
  
}

void STAND_HEIGHT(int angle)
{ 
  
  for(int i=1; i<=12; i++)
  {
    servo_drive(i, angle2OCR(angle));
    __delay_cycles(8e4);
  }
}

void reset_position(void)
{
  int on_time, off_time;
  
   servo_drive(1, angle2OCR(90));
   __delay_cycles(8e4);
  servo_drive(2, angle2OCR(0));                          
  angle2count((int)95, &on_time, &off_time);
  servo_drive_c(1, on_time, off_time);
  __delay_cycles(8e4);
  servo_drive(3, angle2OCR(90));
   __delay_cycles(8e4);
  servo_drive(4, angle2OCR(0));
  angle2count((int)65, &on_time, &off_time);
  servo_drive_c(2, on_time, off_time);
 __delay_cycles(8e4);
  servo_drive(5, angle2OCR(90));
   __delay_cycles(8e4);
  servo_drive(6, angle2OCR(0)); 
  angle2count((int)35, &on_time, &off_time);
  servo_drive_c(3, on_time, off_time);
  __delay_cycles(8e4);                                
  servo_drive(7, angle2OCR(90));
   __delay_cycles(8e4);
  servo_drive(8, angle2OCR(0));
  angle2count((int)65, &on_time, &off_time);
  servo_drive_c(4, on_time, off_time);
  __delay_cycles(8e4);
  servo_drive(9, angle2OCR(90));
   __delay_cycles(8e4);
  servo_drive(10, angle2OCR(0));
  angle2count((int)120, &on_time, &off_time);
  servo_drive_c(5, on_time, off_time);
  __delay_cycles(8e4);
  servo_drive(11, angle2OCR(90));   
   __delay_cycles(8e4);
  servo_drive(12, angle2OCR(0));       //STOP 
  angle2count((int)120, &on_time, &off_time);
  servo_drive_c(6, on_time, off_time);
  __delay_cycles(8e4);
  
  sprintf(data, "SELECT");  //forward Mode selected
      send2LCD(data,0x01, strlen(data));
      sprintf(data, "MODE");
      send2LCD(data,0xC0, strlen(data));
  
}


/*  
      The following functions below are the leg step mode functions used on the 
      robot. There are 6 of these functions altogether and when called they
      will perform a basic step sequence. Six of these functions form a walk
      sequence, whether this will be backwards, left, right, forwards, etc. 
      The step function utilise all of the A, B and C servos to form a step 
*/
void stepLeg1(char STATE)       //This function deals with one leg step
{

  	int on_time, off_time;
        on_time = 0;
        off_time = 0;
        
  if(STATE == 'F')          //If F recieved do Forwards step
  {  
      angle2count((int)120, &on_time, &off_time);  //convert angle to counts
      servo_drive_c(1, on_time, off_time);         //move servoc leg1C
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(EXTEND_ANGLE));     //extend angle of servo 1A
      servo_drive(2, angle2OCR(EXTEND_ANGLE));     //extend angle or servo 2A
      __delay_cycles(STEP_DELAY);                  //
      angle2count((int)85, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);
      servo_drive(1, angle2OCR(RETRACT_ANGLE));
      servo_drive(2, angle2OCR(RETRACT_ANGLE));
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(35));
      __delay_cycles(STEP_DELAY);  
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time); 
  }
  else if(STATE == 'B')      //If B recieved do Backwards step
  {
      angle2count((int)85, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(EXTEND_ANGLE));
      servo_drive(2, angle2OCR(EXTEND_ANGLE));
      __delay_cycles(STEP_DELAY);
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);
      servo_drive(1, angle2OCR(RETRACT_ANGLE));
      servo_drive(2, angle2OCR(RETRACT_ANGLE));
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(35));
      __delay_cycles(STEP_DELAY);  
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);
  }
  else if(STATE == 'R')     //If R recieved do Reverse step
  {        
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time); 
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(EXTEND_ANGLE));
      servo_drive(2, angle2OCR(EXTEND_ANGLE));
      __delay_cycles(STEP_DELAY);
      angle2count((int)65, &on_time, &off_time);      
      servo_drive_c(1, on_time, off_time);
      servo_drive(1, angle2OCR(RETRACT_ANGLE));
      servo_drive(2, angle2OCR(RETRACT_ANGLE));
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(35));
      __delay_cycles(STEP_DELAY);  
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);        
  }
  else if(STATE == 'L')       //If L recieved do Left step
  {       
      angle2count((int)65, &on_time, &off_time);     
      servo_drive_c(1, on_time, off_time); 
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(EXTEND_ANGLE));
      servo_drive(2, angle2OCR(EXTEND_ANGLE));
      __delay_cycles(STEP_DELAY);
      angle2count((int)120, &on_time, &off_time);        
      servo_drive_c(1, on_time, off_time);
      servo_drive(1, angle2OCR(RETRACT_ANGLE));
      servo_drive(2, angle2OCR(RETRACT_ANGLE));
      __delay_cycles(STEP_DELAY);
      servo_drive(1, angle2OCR(35));
      __delay_cycles(STEP_DELAY);  
      angle2count((int)120, &on_time, &off_time);
      servo_drive_c(1, on_time, off_time);     
    
  }
        
  
}

void stepLeg2(char STATE)           //This function deals with one leg step
{ 
        //servo 3 is upper leg extension
	//servo 4 is lower leg extension
	//servo 2c is sweep angle
	int on_time, off_time;
        on_time = 0;
        off_time = 0;  
  
   if(STATE == 'F')
  {
	
	
	angle2count((int)90, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(3, angle2OCR(EXTEND_ANGLE));
	servo_drive(4, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)65, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
	servo_drive(3, angle2OCR(RETRACT_ANGLE));
	servo_drive(4, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(3, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        angle2count((int)90, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
  }
    else if(STATE == 'B')
  {
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);        
	__delay_cycles(STEP_DELAY);
	servo_drive(3, angle2OCR(EXTEND_ANGLE));
	servo_drive(4, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)90, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
	servo_drive(3, angle2OCR(RETRACT_ANGLE));
	servo_drive(4, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(3, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        
        angle2count((int)90, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
  }
    else if(STATE == 'R')
  {     
        angle2count((int)120, &on_time, &off_time);        
	servo_drive_c(2, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
	servo_drive(3, angle2OCR(EXTEND_ANGLE));
	servo_drive(4, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);
	servo_drive(3, angle2OCR(RETRACT_ANGLE));
	servo_drive(4, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(3, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
	servo_drive_c(2, on_time, off_time);        
  }
  else if(STATE == 'L')
  {       
        angle2count((int)65, &on_time, &off_time);     
        servo_drive_c(2, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
        servo_drive(3, angle2OCR(EXTEND_ANGLE));
        servo_drive(4, angle2OCR(EXTEND_ANGLE));
        __delay_cycles(STEP_DELAY);
        angle2count((int)120, &on_time, &off_time);        
        servo_drive_c(2, on_time, off_time);
        servo_drive(3, angle2OCR(RETRACT_ANGLE));
        servo_drive(4, angle2OCR(RETRACT_ANGLE));
        __delay_cycles(STEP_DELAY);
        servo_drive(3, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
        servo_drive_c(2, on_time, off_time);     
    
  }
}
//// STEP LEG 3
void stepLeg3(char STATE)
{
  
	int on_time, off_time;
        on_time = 0;
        off_time = 0;
        
   if(STATE == 'F')
   {    
	
	angle2count((int)FINAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(5, angle2OCR(EXTEND_ANGLE));
	servo_drive(6, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)INITIAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	servo_drive(5, angle2OCR(RETRACT_ANGLE));
	servo_drive(6, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)FINAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
   }
  else if(STATE == 'B')
  {
    	angle2count((int)INITIAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
        __delay_cycles(STEP_DELAY);
	servo_drive(5, angle2OCR(EXTEND_ANGLE));
	servo_drive(6, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);    
        angle2count((int)FINAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	servo_drive(5, angle2OCR(RETRACT_ANGLE));
	servo_drive(6, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)FINAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
  }
  else if(STATE == 'R')
  {
        angle2count((int)120, &on_time, &off_time);        
	servo_drive_c(3, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
	servo_drive(5, angle2OCR(EXTEND_ANGLE));
	servo_drive(6, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	servo_drive(5, angle2OCR(RETRACT_ANGLE));
	servo_drive(6, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);        
  }
  else if(STATE == 'L')
  {       
        angle2count((int)65, &on_time, &off_time);     
        servo_drive_c(3, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(EXTEND_ANGLE));
        servo_drive(6, angle2OCR(EXTEND_ANGLE));
        __delay_cycles(STEP_DELAY);
        angle2count((int)120, &on_time, &off_time);        
        servo_drive_c(3, on_time, off_time);
        servo_drive(5, angle2OCR(RETRACT_ANGLE));
        servo_drive(6, angle2OCR(RETRACT_ANGLE));
        __delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
        servo_drive_c(3, on_time, off_time);         
  }
    
}


void stepLeg4(char STATE)
{
  	int on_time, off_time;
        on_time = 0;
        off_time = 0;
        
  if(STATE == 'F')
   {    	
	angle2count((int)45, &on_time, &off_time);
	servo_drive_c(4, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(7, angle2OCR(EXTEND_ANGLE));
	servo_drive(8, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)90, &on_time, &off_time);
	servo_drive_c(4, on_time, off_time);
	servo_drive(7, angle2OCR(RETRACT_ANGLE));
	servo_drive(8, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(7, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
       angle2count((int)45, &on_time, &off_time);
      servo_drive_c(4, on_time, off_time);
   }
   else if(STATE == 'B')
  {
       angle2count((int)90, &on_time, &off_time);
       servo_drive_c(4, on_time, off_time);
       __delay_cycles(STEP_DELAY);
       servo_drive(7, angle2OCR(EXTEND_ANGLE));
       servo_drive(8, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
       angle2count((int)45, &on_time, &off_time);
       servo_drive_c(4, on_time, off_time);
       servo_drive(7, angle2OCR(RETRACT_ANGLE));
       servo_drive(8, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(7, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
       angle2count((int)45, &on_time, &off_time);
      servo_drive_c(4, on_time, off_time);
  }
   else if(STATE == 'R')
  {
        angle2count((int)120, &on_time, &off_time);        
	servo_drive_c(4, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
	servo_drive(7, angle2OCR(EXTEND_ANGLE));
	servo_drive(8, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(4, on_time, off_time);
	servo_drive(7, angle2OCR(RETRACT_ANGLE));
	servo_drive(8, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(7, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
	servo_drive_c(4, on_time, off_time);
  }
  else if(STATE == 'L')
  {       
        angle2count((int)65, &on_time, &off_time);     
        servo_drive_c(4, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
        servo_drive(7, angle2OCR(EXTEND_ANGLE));
        servo_drive(8, angle2OCR(EXTEND_ANGLE));
        __delay_cycles(STEP_DELAY);
        angle2count((int)120, &on_time, &off_time);        
        servo_drive_c(4, on_time, off_time);
        servo_drive(7, angle2OCR(RETRACT_ANGLE));
        servo_drive(8, angle2OCR(RETRACT_ANGLE));
        __delay_cycles(STEP_DELAY);
        servo_drive(7, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)65, &on_time, &off_time);
        servo_drive_c(4, on_time, off_time);         
  }
  
}


void stepLeg5(char STATE)
{
	//servo 9 is upper leg extension
	//servo 10 is lower leg extension
	//servo 5c is sweep angle
	int on_time, off_time;
        on_time = 0;
        off_time = 0;
        
   if(STATE == 'F')
   {    	
	angle2count((int)45, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(9, angle2OCR(EXTEND_ANGLE));
	servo_drive(10, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)120, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
	servo_drive(9, angle2OCR(RETRACT_ANGLE ));
	servo_drive(10, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(9, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        angle2count((int)45, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
   }
   else if(STATE == 'B')
  {
    	angle2count((int)90, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(9, angle2OCR(EXTEND_ANGLE));
	servo_drive(10, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2count((int)45, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
	servo_drive(9, angle2OCR(RETRACT_ANGLE));
	servo_drive(10, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(9, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        angle2count((int)45, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
  }
  else if(STATE == 'R')
  {
        angle2count((int)120, &on_time, &off_time);        
	servo_drive_c(5, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
	servo_drive(9, angle2OCR(EXTEND_ANGLE));
	servo_drive(10, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);
	servo_drive(9, angle2OCR(RETRACT_ANGLE));
	servo_drive(10, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(9, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
	servo_drive_c(5, on_time, off_time);         
  }
   else if(STATE == 'L')
  {       
        angle2count((int)65, &on_time, &off_time);     
        servo_drive_c(5, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
        servo_drive(9, angle2OCR(EXTEND_ANGLE));
        servo_drive(10, angle2OCR(EXTEND_ANGLE));
        __delay_cycles(STEP_DELAY);
        angle2count((int)120, &on_time, &off_time);        
        servo_drive_c(5, on_time, off_time);
        servo_drive(9, angle2OCR(RETRACT_ANGLE));
        servo_drive(10, angle2OCR(RETRACT_ANGLE));
        __delay_cycles(STEP_DELAY);
        servo_drive(5, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)65, &on_time, &off_time);
        servo_drive_c(5, on_time, off_time);         
  }
}

void stepLeg6(char STATE)
{
	int on_time, off_time;
        on_time = 0;
        off_time = 0;
        
   if(STATE == 'F')
   {    
	//angle2count((int)65, &on_time, &off_time);
        angle2count((int)45, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(11, angle2OCR(EXTEND_ANGLE));
	servo_drive(12, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	//angle2count((int)120, &on_time, &off_time);
        angle2count((int)90, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
	servo_drive(11, angle2OCR(RETRACT_ANGLE));
	servo_drive(12, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(11, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        angle2count((int)45, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
   }
   else if(STATE == 'B')
  {
    	//angle2count((int)65, &on_time, &off_time);
        angle2count((int)90, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(11, angle2OCR(EXTEND_ANGLE));
	servo_drive(12, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
	//angle2count((int)120, &on_time, &off_time);
         angle2count((int)45, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
	servo_drive(11, angle2OCR(RETRACT_ANGLE));
	servo_drive(12, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(11, angle2OCR(35));
        __delay_cycles(STEP_DELAY);
        angle2count((int)45, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
  }
   else if(STATE == 'R')
  {
        angle2count((int)120, &on_time, &off_time);        
	servo_drive_c(6, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
	servo_drive(11, angle2OCR(EXTEND_ANGLE));
	servo_drive(12, angle2OCR(EXTEND_ANGLE));
	__delay_cycles(STEP_DELAY);
        angle2count((int)65, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);
	servo_drive(11, angle2OCR(RETRACT_ANGLE));
	servo_drive(12, angle2OCR(RETRACT_ANGLE));
	__delay_cycles(STEP_DELAY);
        servo_drive(11, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)120, &on_time, &off_time);
	servo_drive_c(6, on_time, off_time);        
  }
    else if(STATE == 'L')
  {       
        angle2count((int)65, &on_time, &off_time);     
        servo_drive_c(6, on_time, off_time); 
        __delay_cycles(STEP_DELAY);
        servo_drive(11, angle2OCR(EXTEND_ANGLE));
        servo_drive(12, angle2OCR(EXTEND_ANGLE));
        __delay_cycles(STEP_DELAY);
        angle2count((int)120, &on_time, &off_time);        
        servo_drive_c(6, on_time, off_time);
        servo_drive(11, angle2OCR(RETRACT_ANGLE));
        servo_drive(12, angle2OCR(RETRACT_ANGLE));
        __delay_cycles(STEP_DELAY);
        servo_drive(11, angle2OCR(35));
        __delay_cycles(STEP_DELAY);  
        angle2count((int)65, &on_time, &off_time);
        servo_drive_c(6, on_time, off_time);         
  }
}
/*
    The following code controls the angle of the 12, A and B servos and has 
    1 to 12 cases which decide the servo too be performed
*/
void servo_drive(char servo, int ocr) 
{
	switch(servo)                         //what servo will be performed
	{
		case 1:                       
		        LEG1_A_ON= 1;         //servo 1A will be performed
			LEG1_A = ocr;	      //P3
			break;
		case 2:                      
			LEG1_B_ON= 1;         //servo 1B will be performed
			LEG1_B = ocr;	      //P4
			break;
		case 3:                       
			LEG2_A_ON= 1;         //servo 2A will be performed
			LEG2_A = ocr;	      //P7
			break;
		case 4:                       
			LEG2_B_ON= 1;         //servo 2B will be performed
			LEG2_B = ocr;	      //P8
			break;
		case 5:
			LEG3_A_ON= 1;         //servo 3A will be performed
			LEG3_A = ocr;	      //P13
			break;
		case 6:
			LEG3_B_ON= 1;	      //servo 3B will be performed 
			LEG3_B = ocr;	      //P14
			break;
		case 7:
			LEG4_A_ON= 1;	        //servo 4A will be performed
			LEG4_A = ocr;		//P11
			break;
		case 8:
			LEG4_B_ON= 1;	        //servo 4B will be performed
			LEG4_B = ocr;		//P12
			break;
		case 9:
			LEG5_A_ON= 1;	        //servo 5A will be performed
			LEG5_A = ocr;		//P9
			break;
		case 10:
			LEG5_B_ON= 1;	        //servo 5B will be performed
			LEG5_B = ocr;		//P10
			break;
		case 11:
			LEG6_A_ON= 1;	        //servo 6A will be performed
			LEG6_A = ocr;		//P1
			break;
		case 12:
			LEG6_B_ON= 1;	        //servo 6B will be performed
			LEG6_B = ocr;		//P15
			break;
		default:
			//handle errors
			//printf('ERROR: Not a valid leg %d\n', leg_index);
			break;
	}
}	
//**********DIPLAY ON LCD***********************************************

void send2LCD(char* data, char adress, char length)
{
    lcd_write_int(adress); 
    for(int i= 0; i < length; i++)     
    {
              lcd_write_data(*(data+i));
    }
}   


/*
TESTBENCH FUNCTIONS

*/

#include <iom1280.h>
#include <intrinsics.h>
#include <stdarg.h>
#include "binary.h"
#include "robot.h"

#define TEST_ANGLE 0
#define ANGLE_STEP 10
#define ANGLE_MAX 130
#define ON_TIME_MAX 12500
#define C_SERVO_MIN 470
#define C_SERVO_MAX 1500
#define C_SERVO_STEP 50



void setup();
void servo_ex();
void servo_angle(int smin, int smax);
void servo_angle_c(int min, int max);
void servo_drive(char servo, int ocr);
void servo_drive_c(char servo, int on_time, int off_time);
int angle2OCR(int angle);
void LEG2C(int on_time, int off_time);
void angle2count(int angle, int* on_time, int* off_time);
void servo_c_index(char servo, char state);



//****Functions for walking steps
void walk1_STEP(int on_time, int off_time);
void walk2_STEP(int on_time, int off_time);
void walk3_FORWARD(int on_time, int off_time);
void walk4_FORWARD(int on_time, int off_time);
void backwards_STEP1(int on_time, int off_time);
void backwards_STEP2(int on_time, int off_time);

void setup()
{
      TCCR1A= b10101010;
      TCCR1B= b00011011;
      
      TCCR3A= b10101010;
      TCCR3B= b00011011;
      
      TCCR4A= b10101010;
      TCCR4B= b00011011; 
      
      TCCR5A= b10101010;
      TCCR5B= b00011011; 
           
      ICR1= PWM_MAX_TIME;     //setup PWM time to 20ms
      ICR3= PWM_MAX_TIME;     //setup PWM time
      ICR4= PWM_MAX_TIME;     //setup PWM time
      ICR5= PWM_MAX_TIME;     //setup PWM time
      
}
     
	  




void main(void)
{  
   
   
    int i, angle, on_time, off_time;
	setup();
        
       
            
          servo_drive(1, angle2OCR(45));
          servo_drive(3, angle2OCR(45));
          servo_drive(5, angle2OCR(45));
          servo_drive(7, angle2OCR(45));
          servo_drive(9, angle2OCR(45));
          servo_drive(11, angle2OCR(45));          
         
          servo_drive(2, angle2OCR(45));
          servo_drive(4, angle2OCR(45));
          servo_drive(6, angle2OCR(45));
          servo_drive(8, angle2OCR(45));
          servo_drive(10, angle2OCR(45));
          servo_drive(12, angle2OCR(45));
      

     
    while(1)
	{  
	
	
			
                 walk1_STEP(on_time, off_time); 

            angle=45;
            angle2count(angle, &on_time, &off_time);
            servo_drive_c(1, on_time, off_time);
            servo_drive_c(6, on_time, off_time);
          //  servo_drive_c(2, on_time, off_time);
            
            angle=65;
            angle2count(angle, &on_time, &off_time);
            servo_drive_c(4, on_time, off_time);
            servo_drive_c(3, on_time, off_time);
           // servo_drive_c(2, on_time, off_time);
          
            walk1_STEP(on_time, off_time);
            walk2_STEP(on_time, off_time);
           
          
         // while(1);   
          //servo_drive(1, angle2OCR(45));
         // __delay_cycles(8e6);
          
          /*
           
  		for(i=12; i<=12; i++)
		{
			servo_drive(i, angle2OCR(25));
			__delay_cycles(8e6);
		}
             
              //  while(1);
        
                angle = 0;
                while(angle < 120)
                {
                  servo_drive(4, angle2OCR(angle));
                  __delay_cycles(8e6);
                  angle += 24;
                }  
                angle = 0;
                while(angle < 120)
                {
                  servo_drive(12, angle2OCR(angle));
                  __delay_cycles(8e6);
                  angle += 24;
                }  
                
              
				angle = 120;
				while(angle > 0)
				{
				 servo_drive(3, angle2OCR(angle));
				  __delay_cycles(8e6);
				  angle -= 24;
				}
            */
                /*
                angle = 0;
                for(i=0; i<5; i++)
                {
                  servo_drive(11, angle2OCR(angle));
                  __delay_cycles(8e6);
                  angle += 24;
                }  
                */
	}	  
}

/// IDEAS FOR WALK ROUTINE ////

void main()
{	
	stand();

	while(1)
	{
		if(ctrl_walk_fwd)
			walk_fwd();
		if(ctrl_walk_bkwd)
			walk_bkwd();
	}
}

void walk_fwd()
{
	stepLeg3();
	stepLeg6();
	//small delay
	stepLeg4();
	stepLeg1();
	//small delay

}

void stepLeg1()
{
	// LEG1 has servos 1,2,3
	// servo 3 determines the sweep angle
	// servo 2 determines the lower leg extension
	// servo 1 determines the upper leg extension
	
	
}


void stepLeg3()
{
	//servo 5 is upper leg extension
	//servo 6 is lower leg extension
	//servo 3c is sweep angle
	int on_time, off_time;
	
	angle2count(FINAL_ANGLE &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	__delay_cycles(STEP_DELAY);
	servo_drive(5, angle2OCR(EXTENSION_ANGLE));
	servo_drive(6, angle2OCR(EXTENSION_ANGLE));
	__delay_cycles(STEP_DELAY);
	angle2_count(INITIAL_ANGLE, &on_time, &off_time);
	servo_drive_c(3, on_time, off_time);
	servo_drive(5, angle2OCR(START_ANGLE));
	servo_drive(6, angle2OCR(START_ANGLE));
	__delay_cycles(STEP_DELAY);
}








//// TEST ROUTINES ////

//// Drive each servo to a set of particular angles. The angles to be used are
//// specified in the array angle. Each servo should move to the first angle, 
//// second, and so on in sequence.
void servo_angle(int smin, int smax)
{
    //int angle[NUM_ANGLES] = {0, 45, 90, 120};
    int angle[NUM_ANGLES] = {0, 45, 90, 120};
	int ocr_cur;
	int servo, i;
	
	// Test to make sure we pass variables in range
	if(smin < 1)
		smin = 1;
	if(smax > 12)
		smax = 12;
	
	for(servo=smin; servo <= smax; servo++)
	{
                //increment angles
		for(i=0; i<NUM_ANGLES; i++)			//4 is number of angles to test
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

void servo_angle_c(int min, int max)
{
  int servo;
  int on_time, off_time;
//int angle [NUM_ANGLES_C] = {0, 45, 90, 120};
  int angle [NUM_ANGLES_C] = {0, 45, 90, 120};
/*  
  if(min < 1)
    min = 1;
  if(min > 6)
    min = 6;
  */
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

void LEG2C(int on_time, int off_time)
{
  
  // Proposed model is (7*angle + 470)
  // on_time = 7 * angle + 470;   //NB: angle is in degrees
  // off_time = MAX_ON_TIME - on_time;
  int count1, count2, counter;
  counter= 50;
  
  LEG2_C_ON=1;
  
   while(counter>0)
    {     
                        
      count1= on_time;
      while(count1>0)
      {
           PORTK_Bit2= 1;
          count1--; 
      }               
      count2= off_time;
      while(count2>0)
      {
           PORTK_Bit2=0;
           count2--;
      }       
       counter--;
    }
}


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
void servo_drive_c(char servo, int on_time, int off_time)
{
  int counter, on_count, off_count;
  counter = 90;   //how many pulses
  
  while(counter>0)
  {     
                      
    on_count = on_time;
    while(on_count>0)
    {
        servo_c_index(servo, 1);
        on_count--; 
    }               
    off_count = off_time;
    while(off_count>0)
    {
         servo_c_index(servo, 0);
         off_count--;
    }       
     counter--;
  }
  
}
*/
void servo_drive_c(char servo, int on_time, int off_time)
{
  int counter, on_count, off_count;
  counter = 15;   //how many pulses
  
  while(counter>0)
  { 

	switch(servo)
	{
	       case 1:									
				LEG1_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTH_Bit6= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					 PORTH_Bit6= 0;
					 off_count--;
				} 
				break;				
		case 2:			
				LEG2_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTK_Bit2= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					 PORTK_Bit2= 0;
					 off_count--;
				} 
				break;	
		case 3:					
				LEG3_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTK_Bit0= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					 PORTK_Bit0= 0;
					 off_count--;
				} 
				break;
		case 4:
				LEG4_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTK_Bit1= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					PORTK_Bit1= 0;
					 off_count--;
				} 
				break;
		case 5:
				LEG5_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTB_Bit4= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					PORTB_Bit4= 0;
					 off_count--;
				} 
				break;
		case 6:					
				LEG6_C_ON = 1;                 
				on_count = on_time;
				while(on_count>0)
				{
					PORTG_Bit5= 1;
					on_count--; 
				}               
				off_count = off_time;
				while(off_count>0)
				{
					PORTG_Bit5= 0;
					 off_count--;
				} 
				break;			
		 default:
				break;									 
  }
  counter--;
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




void walk1_STEP(int on_time, int off_time)
{ 
  
  
  
       
}







void walk2_STEP(int on_time, int off_time)
{
int angle;
       //after standing up 
      servo_drive(1, angle2OCR(45)); 
      servo_drive(2, angle2OCR(45));
      servo_drive(7,angle2OCR(90));
      servo_drive(8, angle2OCR(90));           
     __delay_cycles(4e6);    
      servo_drive(1, angle2OCR(90)); 
      servo_drive(2, angle2OCR(90));
      servo_drive(7, angle2OCR(45));
      servo_drive(8, angle2OCR(45)); 
      __delay_cycles(4e6);
}

void backwards_STEP1(int on_time, int off_time)
{
      //after standing up 
    
      servo_drive(7, angle2OCR(0));
      servo_drive(8, angle2OCR(0));   
      servo_drive(5, angle2OCR(45)); 
      servo_drive(6, angle2OCR(45));      
      __delay_cycles(8e6);
      servo_drive(7, angle2OCR(45));
      servo_drive(8, angle2OCR(45)); 
      servo_drive(5, angle2OCR(0)); 
      servo_drive(6, angle2OCR(0));
      __delay_cycles(8e6);

}

void backwards_STEP2(int on_time, int off_time)
{  
       //after standing up       
      servo_drive(11,angle2OCR(0));
      servo_drive(12, angle2OCR(0));
      servo_drive(5, angle2OCR(45)); 
      servo_drive(6, angle2OCR(45));      
      __delay_cycles(2e6);          
      servo_drive(11, angle2OCR(45));
      servo_drive(12, angle2OCR(45)); 
      servo_drive(5, angle2OCR(0)); 
      servo_drive(6, angle2OCR(0));      
      __delay_cycles(2e6);
  
}









void walk3_FORWARD(int on_time, int off_time)
{

      int angle= 65;
      angle2count(angle, &on_time, &off_time);
      servo_drive_c(2, on_time, off_time);
      servo_drive(3, angle2OCR(45));
      servo_drive(4, angle2OCR(0));
      angle= 25;
      angle2count(angle, &on_time, &off_time);
      servo_drive_c(2, on_time, off_time);
      servo_drive(3, angle2OCR(45));
      servo_drive(4, angle2OCR(45));

}
void walk4_FORWARD(int on_time, int off_time)
{
      int angle= 65;               
      angle2count(angle, &on_time, &off_time);                
      servo_drive_c(5, on_time, off_time);
      servo_drive(9, angle2OCR(45));
      servo_drive(10, angle2OCR(0));
      angle= 25;
      angle2count(angle, &on_time, &off_time);
      servo_drive_c(5, on_time, off_time);
      servo_drive(9, angle2OCR(45));
      servo_drive(10,angle2OCR(45)); 
  
}


void servo_drive(char servo, int ocr)
{
/*  
      LEG1_A_ON = 0;
      LEG1_B_ON = 0;
      LEG1_C_ON = 0;  
      LEG2_A_ON = 0;
      LEG2_B_ON = 0;
      LEG2_C_ON = 0;  
      LEG3_A_ON = 0;
      LEG3_B_ON = 0;
      LEG3_C_ON = 0;   
     LEG4_A_ON = 0;      
     LEG4_B_ON = 0;
     LEG4_C_ON = 0;  
      LEG5_A_ON = 0;        
      LEG5_B_ON = 0;
      LEG5_C_ON = 0;   
     LEG6_A_ON = 0;
    LEG6_B_ON = 0;
     LEG6_C_ON = 0;
  */
	switch(servo)
	{
		case 1:
		    LEG1_A_ON= 1;
			LEG1_A = ocr;		//P3
			break;
		case 2:
			LEG1_B_ON= 1;
			LEG1_B = ocr;		//P4
			break;
		case 3:
			LEG2_A_ON= 1;
			LEG2_A = ocr;		//P7
			break;
		case 4:
			LEG2_B_ON= 1;
			LEG2_B = ocr;		//P8
			break;
		case 5:
			LEG3_A_ON= 1;
			LEG3_A = ocr;		//P13
			break;
		case 6:
			LEG3_B_ON= 1;	
			LEG3_B = ocr;		//P14
			break;
		case 7:
			LEG4_A_ON= 1;	
			LEG4_A = ocr;		//P11
			break;
		case 8:
			LEG4_B_ON= 1;	
			LEG4_B = ocr;		//P12
			break;
		case 9:
			LEG5_A_ON= 1;	
			LEG5_A = ocr;		//P9
			break;
		case 10:
			LEG5_B_ON= 1;	
			LEG5_B = ocr;		//P10
			break;
		case 11:
			LEG6_A_ON= 1;	
			LEG6_A = ocr;		//P1
			break;
		case 12:
			LEG6_B_ON= 1;	
			LEG6_B = ocr;		//P15
			break;
		default:
			//handle errors
			//printf('ERROR: Not a valid leg %d\n', leg_index);
			break;
	}
}	

   
/*
TESTBENCH FUNCTIONS

*/

#define PWM_MAX_TIME 2500
#define OCR_MAX 300
#define OCR_STEP_SIZE 30
#define OCR_TEST_DELAY 4e6
#include "robot.h"

void setup();
void servo_ex(char servo, int ocr);

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
      
      //DDRK_Bit0= 1;
      
      LEG1_A_ON= 1;
      LEG1_B_ON= 1;
      LEG1_C_ON= 1;
      
      LEG2_A_ON= 1;
      LEG2_B_ON= 1;
      LEG2_C_ON= 1;
      
      LEG3_A_ON= 1;
      LEG3_B_ON= 1;
      LEG3_C_ON= 1;
      
      LEG4_A_ON= 1;      
      LEG4_B_ON= 1;
      LEG4_C_ON= 1;
      
      LEG5_A_ON= 1;        
      LEG5_B_ON= 1;
      LEG5_C_ON= 1;
      
      LEG6_A_ON= 1;
      LEG6_B_ON= 1;
      LEG6_C_ON= 1;
      
      ICR1= PWM_MAX_TIME;     //setup PWM time to 20ms
      ICR3= PWM_MAX_TIME;     //setup PWM time
      ICR4= PWM_MAX_TIME;     //setup PWM time
      ICR5= PWM_MAX_TIME;     //setup PWM time
}
     
void servo_ex(char servo, int ocr)
{
	switch(servo)
	{
		case 1:
			LEG1_A = count_a;		//P3
			break;
		case 2:
			LEG1_B = count_b;		//P4
			break;
		case 3:
			LEG2_A = count_a;		//P7
			break;
		case 4:
			LEG2_B = count_b;		//P8
			break;
		case 5:
			LEG3_A = count_a;		//P13
			break;
		case 6:
			LEG3_B = count_b;		//P14
			break;
		case 7:
			LEG4_A = count_a;		//P11
			break;
		case 8:
			LEG4_B = count_b;		//P12
			break;
		case 9:
			LEG5_A = count_a;		//P9
			break;
		case 10:
			LEG5_B = count_b;		//P10
			break;
		case 11:
			LEG6_A = count_a;		//P1
			break;
		case 12:
			LEG6_B = count_b;		//P15
			break;
		default:
			//handle errors
			//printf('ERROR: Not a valid leg %d\n', leg_index);
			break;
	}
}	 
	  /*
      //enable the TIMSK0 interrupt
      TIMSK0= b00000010;      //enable output compare interrupt

      TCCR0A= b00000000;
      TCCR0B= b00000010;      //prescale8
      OCR0A= 10;             //interrupt every 100us
      
      //setup timer2 PWM
      TCCR2A= b00000010;
      TCCR2B=  b00000010; 
        TIMSK2= b00000010;
        OCR2A= 10;
     
      global_int_enable= 1;
      //SETUP USART
	  */
	  
	  
	  
	  
	  
	  
	  
	  
void main(void)
{
	int servo, k, ocr_cur;
	
	setup();
	while(1)
	{
		//go through each leg
		for(servo=1; servo>=12; servo++)
		{
			// step up from 0
			ocr_cur = 0;
			while(ocr_cur < OCR_MAX)
			{
				servo_ex(servo, ocr_cur);
				ocr_cur += OCR_STEP_SIZE;	
				//delay here
				__delay_cycles(OCR_TEST_DELAY);
			}
			__delay_cycles(OCR_TEST_DELAY * 2);
			//step down from 300
			ocr_cur = OCR_MAX;
			while(ocr_cur > 0)
			{
				servo_ex(servo, ocr_cur);
				ocr_cur -= OCR_STEP_SIZE;
				__delay_cycles(OCR_TEST_DELAY);
			}
		}
	}	  
}



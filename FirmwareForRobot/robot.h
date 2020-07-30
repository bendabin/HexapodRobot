/* ROBOT.H
Header file for hexapos robot constants

*/

#ifndef __ROBOT_H
#define __ROBOT_H



#define PWM_MAX_TIME 2500   //Maximum PWM time (ICR value)
#define OCR_MIN 100         //Minimum OCR value
#define OCR_MAX 300         //Maximum OCR value
#define OCR_STEP_SIZE 20    //How much to step OCR by in test
#define OCR_TEST_DELAY 8e6  //Number of cycles to wait between tests
#define NUM_ANGLES 4       //number of angles to test
#define NUM_ANGLES_C 4 

#define global_int_enable SREG_Bit7

#define LEG1_A_ON DDRB_Bit5       //P3
#define LEG1_B_ON DDRB_Bit6       //P4
#define LEG1_C_ON DDRH_Bit6       //P6

#define LEG2_A_ON DDRE_Bit3       //P7
#define LEG2_B_ON DDRE_Bit4       //P8
#define LEG2_C_ON DDRK_Bit2       //P18

#define LEG3_A_ON DDRL_Bit3       //P13 
#define LEG3_B_ON DDRL_Bit4       //P14
#define LEG3_C_ON DDRK_Bit0       //P16

#define LEG4_A_ON DDRH_Bit4       //P11
#define LEG4_B_ON DDRH_Bit5       //P12
#define LEG4_C_ON DDRK_Bit1       //P17

#define LEG5_A_ON DDRE_Bit5       //P9
#define LEG5_B_ON DDRH_Bit3       //P10
#define LEG5_C_ON DDRB_Bit4       //P5

#define LEG6_A_ON DDRB_Bit7      //P15
#define LEG6_B_ON DDRL_Bit5      //P15   
#define LEG6_C_ON DDRG_Bit5      //P2

//defines for PWM
#define LEG1_A OCR1A 
#define LEG1_B OCR1B

#define LEG2_A OCR3A
#define LEG2_B OCR3B

#define LEG3_A OCR5A
#define LEG3_B OCR5B

#define LEG4_A OCR4B
#define LEG4_B OCR4C

#define LEG5_A OCR3C
#define LEG5_B OCR4A

#define LEG6_A OCR1C        //P1
#define LEG6_B OCR5C        //P15

//***********defines for PWM state LEGC***************************
#define LEG1_ST 0
#define LEG2_ST 2
#define LEG3_ST 3
#define LEG4_ST 4
#define LEG5_ST 5
#define LEG6_ST 6

#define DEFAULT 1
#define FORWARD 2
#define BACKWARD 3
//#define DEFAULT 4

//**************Define USART stuff*******************************

#define RX_buffer_full    UCSR1A_Bit7
#define TX_Buffer_empty   UCSR1A_Bit5

//side 1

#define LEG1_ON_RIGHT  590  
#define LEG1_OFF_RIGHT 5310
#define LEG1_ON_LEFT  295
#define LEG1_OFF_LEFT 5605

#define LEG2_ON_RIGHT  516  
#define LEG2_OFF_RIGHT 5384
#define LEG2_ON_LEFT  295   
#define LEG2_OFF_LEFT 5605


#define LEG3_ON_RIGHT  516
#define LEG3_OFF_RIGHT 5384
#define LEG3_ON_LEFT 	251		
#define LEG3_OFF_LEFT	5649

//side 2

#define  LEG4_ON_LEFT 207            
#define  LEG4_OFF_LEFT 5693

#define  LEG4_ON_RIGHT   590         
#define  LEG4_OFF_RIGHT  5310
   
#define  LEG5_ON_LEFT   294         
#define  LEG5_OFF_LEFT  5606     
#define  LEG5_ON_RIGHT   590         
#define  LEG5_OFF_RIGHT  5310

#define LEG6_ON_LEFT   406
#define LEG6_OFF_LEFT  5494
#define LEG6_ON_RIGHT  590
#define LEG6_OFF_RIGHT  5310

#define global_int_enable SREG_Bit7




#endif //__ROBOT_H
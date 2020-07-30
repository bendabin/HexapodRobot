/*
  
File Name:-  LCD.c
Author:-     Ben Dabin  
Date:-       29/10/13
Version:-    3.0.0

          Program Description:

  The following program is about an LCD driver that deals with displaying 
  user information.

*/

#include <iom1280.h>
#include <intrinsics.h>
#include "LCD.h"
#include "robot.h"

#define E PORTA_Bit0
#define RW PORTA_Bit1
#define RS PORTA_Bit2


void lcd_write(char data);
void check_busy(void);



//*****************Initialisation Function**********************
void lcd_int(void)
{
  __delay_cycles(120000);   //wait 15ms
  lcd_write(0x30);          //0b00110000
   __delay_cycles(40000);   //wait 5ms
   lcd_write(0x30);
   __delay_cycles(800);     //wait 100us
  lcd_write(0x30);   
  __delay_cycles(800);     //wait 100us
  lcd_write_int(0x3C);
  lcd_write_int(0x08);
  lcd_write_int(0x0C);
  lcd_write_int(0x02); //0b00000111
  
   
}

//*****************Write Function************************************
void lcd_write(char data)
{
  DDRC= 0xFF;
  RS = 0;
  RW = 0;
  E = 0;
 // __delay_cycles(8);  
  PORTC= data;     //transmit LSB data
  E= 1;
  __delay_cycles(8);
  E= 0;
  __delay_cycles(8);  
  DDRC=0x00;
}

//**********************check busy**********************************
void check_busy(void)
{  
  DDRC= 0x00; //change PE0 to PE3 as input
  RS= 0;
  RW= 1;
  E= 0;
  __delay_cycles(8);  
  E= 1;
  __delay_cycles(8);
  while(PINC_Bit7);
  E=0;
}
//**********************write data function**********************************

void lcd_write_int(char data)
{
  check_busy();
  lcd_write(data);
}
//**********************Check Function**********************************

void lcd_write_data(char data)
{
  check_busy();
  DDRC= 0xFF;
  RS = 1;
  RW = 0;
  E = 0;
 // __delay_cycles(8);
  PORTC = data;  
  E= 1;
  __delay_cycles(8);
  E= 0;
  __delay_cycles(8); 
  DDRC=0x00;
}
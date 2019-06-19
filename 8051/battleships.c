//-----------------------------------------------------------------------------//
//User_Interface_Functions.c
//----------------------------------------------------------------------------
//  Copyright (C) 2016 BRAUDE COLLEGE  
//  Electronics & Elictrical Engineering Department
//  All rights reserved.
//  Owner		:  Dr. Fadil Tareef
//  FILE NAME   :  User_Interface_Functions.c 
//  DATE         :  23 DEC 2016
//  TARGET MCU   :  C8051F020
//  DESCRIPTION  :  This file contains LCD and KEPAD Interface Functions Implementation.
// 					P1 is used for the control signals, P1.0 = E, P1.1 = RW, P1.2 = RS, output only
// 					P2 is used for data: P2.7 is read to get the status of the LCD (BF)
// 					P2 must be configured as bidirectional (open-drain) 
//					and set to FF (or at least 80) before reading the status of P2.7
//					P3 is used for KEPAD interface.
//					P3.0-P3.3 connected to the COLUMNS of the KEPAD, input pins.
//					P3.4-P3.7 connected to the ROWS of the KEPAD, output pins.
//				  
// 	NOTES: This file used in the LCD.c file.
//		   This file updated for Keil C compiler and C8051F020.
//-----------------------------------------------------------------------------

//#include "C8051F020.h"                  // Include register definition file.
#include "battleships.h"
//----------------------------- related Functions --------------------------------

void delay(int secs)
{
	int pu = secs/
	
	TL0=0x0E6;
	TH0=0x0BC;	//48358 IN DECIMAL
	TR0 = 1;	//START COUNTING
	while(!TF0);	//DELAY UNTIL OF
}


void Reset_isr() interrupt 0
{
	LCD_BF(); // wait untill the LCD is no longer busy
	LCD_INIT();// initialize the LCD to 8 bit mode
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x06); // curser moves from left to right
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x02);// move the cursor home
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("BattleShips!");
	LCD_BF();// wait untill the LCD is no longer busy
	


}
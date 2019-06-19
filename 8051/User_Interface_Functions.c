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
#include "User_Interface_def.h"
//----------------------------- LCD related Functions --------------------------------

//------------------------------------------------------------------------------------
// LCD_INIT FUNCTION
//------------------------------------------------------------------------------------
//
void LCD_INIT(void)
{
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RS_MASK; // RS = 0
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RW_MASK; // RW = 0
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~E_MASK; // E = 0
	large_delay(200); 		// 16ms delay
	LCD_DAT_PORT = 0x38; 	// set 8-bit mode
	small_delay(1); 		//RPK
	pulse_E();
	large_delay(50); 		// 4.1ms delay [50]
	LCD_DAT_PORT = 0x38; 	// set 8-bit mode
	small_delay(1); 		//RPK
	pulse_E();
	large_delay(3); 		// 1.5ms delay [2]
	LCD_DAT_PORT = 0x38; 	// set 8-bit mode
	small_delay(1); 			//RPK
	pulse_E();
	large_delay(3);			// 1.5ms delay [2]
	LCD_CMD(0x06); 			// curser moves right
	LCD_CLRS();
	LCD_CMD(0x0E); 			// display and curser on
}

//------------------------------------------------------------------------------------
// LCD_BF FUNCTION
//------------------------------------------------------------------------------------
// wait for the busy bit to drop
void LCD_BF(void)
{
	LCD_DAT_PORT = 0xFF; 	// Set to FF to enable input on P6
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RS_MASK; // RS = 0
	LCD_CTRL_PORT = LCD_CTRL_PORT | RW_MASK; // RW = 1
	small_delay(3); 		// [1 was original delay value]
	LCD_CTRL_PORT = LCD_CTRL_PORT | E_MASK; // E = 1
	do
	{ 						// wait for busy flag to drop
		small_delay(2); 	// [1 was original delay value]
	} while ((LCD_DAT_PORT & 0x80) != 0);
}

//------------------------------------------------------------------------------------
// LCD_DATA FUNCTION
//------------------------------------------------------------------------------------
// write a character to the LCD screen
void LCD_DAT(char LCDDATA)
{
	LCD_BF();
	LCD_CTRL_PORT = LCD_CTRL_PORT | RS_MASK; // RS = 1
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RW_MASK; // RW = 0
	LCD_DAT_PORT = LCDDATA;
	small_delay(1); //RPK
	pulse_E();
}

//------------------------------------------------------------------------------------
// LCD_MSG FUNCTION
//------------------------------------------------------------------------------------
// write a string to the LCD screen
void LCD_MSG(char MSG[])
{
	int i;
	i=0;
	while(MSG[i])
	{
		LCD_DAT(MSG[i]);
		i++;
	}
}

//------------------------------------------------------------------------------------
// LCD_CMD FUNCTION
//------------------------------------------------------------------------------------
// write a command to the LCD controller
void LCD_CMD(char CMD)
{
	LCD_BF();
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RS_MASK; // RS = 0
	LCD_CTRL_PORT = LCD_CTRL_PORT & ~RW_MASK; // RW = 0
	LCD_DAT_PORT = CMD;
	small_delay(1); //RPK
	pulse_E();
}

//------------------------------------------------------------------------------------
// LCD_CLRS FUNCTION
//------------------------------------------------------------------------------------
void LCD_CLRS(void)
{
	LCD_CMD(0x01); //-- clear LCD display
	LCD_CMD(0x80); //-- cursor go to 0x00
}

//------------------------------------------------------------------------------------
// LCD_GOTO FUNCTION
//------------------------------------------------------------------------------------
void LCD_GOTO(char ADDR)
{
	LCD_CMD(ADDR | 0x80);
}

//------------------------------------------------------------------------------------
// LCD_CURS FUNCTION
//------------------------------------------------------------------------------------
/*void LCD_CURS(__bit on) // 1 displays cursor, 0 hides it
{
	if (on)
		LCD_CMD(0x0E);
	else
		LCD_CMD(0x0C);
}
*/
//----------------------------- KEPAD related Functions --------------------------------

//------------------------------------------------------------------------------------
// KEPAD_INIT FUNCTION
//------------------------------------------------------------------------------------
//
void KEPAD_INIT(void)
{
	KEPAD_PORT = 0xFF;		//define P3.0-P3.3 as input
}

//------------------------------------------------------------------------------------
// KEY_RELEASE FUNCTION
//------------------------------------------------------------------------------------
//
void KEY_RELEASE(void)
{
	unsigned char col; 
	R1=0; R2=0; R3=0; R4=0;			//ground all rows 
	do
	{
		col = KEPAD_PORT & 0x0F;	//read the columns
	}	while(col!=0x0F);			//check until all keys released
}

//------------------------------------------------------------------------------------
// PRESSED_KEY FUNCTION
//------------------------------------------------------------------------------------
//
void PRESSED_KEY(void)
{
	unsigned char col; 
	//KEPAD_PORT = KEPAD_PORT & 0x0F;	//ground all rows at once
	R1=0; R2=0; R3=0; R4=0;
	do
	{
		do
		{
			small_delay(200);		//call delay
			col = KEPAD_PORT&0x0F;	//read the columns
		}	while(col==0x0F);	//check until a key pressed
		small_delay(50);		//call delay
		col = KEPAD_PORT&0x0F;	//read the columns
	}	while(col==0x0F);	//check until a key pressed
}

//------------------------------------------------------------------------------------
// GET_KEY FUNCTION
//------------------------------------------------------------------------------------
//
char GET_KEY(void)
{
	unsigned char col; 
	
	//KEPAD_PORT &= 0xEF;		//ground row 0 only
	R1=0; R2=1; R3=1; R4=1;
	if(C1==0)	//'1' is pressed
	{
		small_delay(50);		//call delay
		while(C1==0);
		return 0;
	}
	if(C2==0)	//'2' is pressed
	{
		small_delay(50);		//call delay
		while(C2==0);
		return 1;
	}
	if(C3==0)	//'3' is pressed
	{
		small_delay(50);		//call delay
		while(C3==0);
		return 2;
	}
		if(C4==0)	//'A' is pressed
	{
		small_delay(50);		//call delay
		while(C4==0);
		return 3;
	}
		
		
	//KEPAD_PORT &= 0xDF;		//ground row 1 only
	R1=1; R2=0; R3=1; R4=1;
	if(C1==0)	//'4' is pressed
	{
		small_delay(50);		//call delay
		while(C1==0);
		return 4;
	}
	if(C2==0)	//'5' is pressed
	{
		small_delay(50);		//call delay
		while(C2==0);
		return 5;
	}
	if(C3==0)	//'6' is pressed
	{
		small_delay(50);		//call delay
		while(C3==0);
		return 6;
	}
		if(C4==0)	//'B' is pressed
	{
		small_delay(50);		//call delay
		while(C4==0);
		return 7;
	}
	
	//KEPAD_PORT &= 0xBF;		//ground row 0 only
	R1=1; R2=1; R3=0; R4=1;
	if(C1==0)	//'7' is pressed
	{
		small_delay(50);		//call delay
		while(C1==0);
		return 8;
	}
	if(C2==0)	//'8' is pressed
	{
		small_delay(50);		//call delay
		while(C2==0);
		return 9;
	}
	if(C3==0)	//'9' is pressed
	{
		small_delay(50);		//call delay
		while(C3==0);
		return 10;
	}
		if(C4==0)	//'C' is pressed
	{
		small_delay(50);		//call delay
		while(C4==0);
		return 11;
	}
	
	//KEPAD_PORT &= 0x7F;		//ground row 0 only
	R1=1; R2=1; R3=1; R4=0;
	if(C1==0)	//'*' is pressed
	{
		small_delay(50);		//call delay
		while(C1==0);
		return 12;
	}
	if(C2==0)	//'0' is pressed
	{
		small_delay(50);		//call delay
		while(C2==0);
		return 13;
	}
	if(C3==0)	//'#' is pressed
	{
		small_delay(50);		//call delay
		while(C3==0);
		return 14;
	}
	if(C4==0)	//'D' is pressed
	{
		small_delay(50);		//call delay
		while(C4==0);
		return 15;
	}
	return 0;
}
//------------------------------------------------------------------------------------
// ASCII Conversion
//------------------------------------------------------------------------------------

char ASCII_CONV(char d)
{
	char ASCII_Table[]="123A456B789C*0#D";
	return ASCII_Table[d];
}

//------------------------------------------------------------------------------------
// delay routines
//------------------------------------------------------------------------------------
// delay using spin wait
void small_delay(char d)
{

	while (d--);
}

void large_delay(char d)
{
	while (d--)
	small_delay(255);
}

void huge_delay(char d)
{
	while (d--)
	large_delay(255);
}

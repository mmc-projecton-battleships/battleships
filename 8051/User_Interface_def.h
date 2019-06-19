//-----------------------------------------------------------------------------//
//User_Interface_def.h
//----------------------------------------------------------------------------
//  Copyright (C) 2016 BRAUDE COLLEGE  
//  Electronics & Elictrical Engineering Department
//  All rights reserved.
//  Owner		:  Dr. Fadil Tareef
//  FILE NAME   :   
//  DATE         :  23 DEC 2016
//  TARGET MCU   :  C8051F020
//  DESCRIPTION  :  This file contains LCD and KEPAD Interface Definition,
//					and functions prototype.
// 					P1 is used for the LCD control signals, P1.0 = E, P1.1 = RW, P1.2 = RS, output only
// 					P2 is used for LCD data: P2.7 is read to get the status of the LCD (BF)
// 					P2 must be configured as bidirectional (open-drain) 
//					and set to FF (or at least 80) before reading the status of P2.7
//					P3 is used for KEPAD interface.
//					P3.0-P3.3 connected to the COLUMNS of the KEPAD, input pins.
//					P3.4-P3.7 connected to the ROWS of the KEPAD, output pins.
//				  
// 	NOTES: This file used in the LCD.c file.
//		   This file updated for Keil C compiler and C8051F020.
//-----------------------------------------------------------------------------

#include <C8051F020.h>                  // Include register definition file.
//---------------------------------------------------------------------------
// Global Defines
//---------------------------------------------------------------------------
#define	KEPAD_PORT		P3		//KEPAD 
#define ROWS_MASK		0xF0	//mask for pins P3.4-P3.7
#define COLUMNS_MASK	0x0F	//mask for pins P3.0-P3.3
#define	LCD_DAT_PORT	P2		//LCD is in 8 bit mode
								//PORT 2 is the DATA port
#define LCD_CTRL_PORT 	P1 		// 3 control pins on P1
#define E_MASK 			0x01	// for assessing LCD_CTRL_PORT
#define RW_MASK 		0x02
#define RS_MASK 		0x04
//sbit 	RS=P1^2;			//Register Set command line 
//sbit	R_W=P1^1;			//Read/Write command line
//sbit	E=P1^0;				//Enable command line
//sbit	BF=P2^7;			//Busy Flag	
//Keypad Connections
sbit C1 = P3^0;
sbit C2 = P3^1;
sbit C3 = P3^2;
sbit C4 = P3^3;
sbit R1 = P3^4;
sbit R2 = P3^5;
sbit R3 = P3^6;
sbit R4 = P3^7;
//End Keypad Connections	
//---------------------------------------------------------------------------
// Global MACROS
//---------------------------------------------------------------------------
#define pulse_E();\
		small_delay(5);\
		LCD_CTRL_PORT = LCD_CTRL_PORT | E_MASK;\
		small_delay(5);\
		LCD_CTRL_PORT = LCD_CTRL_PORT & ~E_MASK;
		
//extern char ADDR, LCDDATA, CMD, d, MSG[];
//**********************************************
//------------ Function prototypes -------------
//**********************************************
void	LCD_BF (void);		// wait until the LCD is no longer busy
void	LCD_INIT (void);	// initialize the LCD to 8 bit mode
void	LCD_CLRS (void);	// clear display
void	LCD_GOTO (char ADDR);	// move to address addr
//void	LCD_CURS (__bit on);// 1 displays cursor, 0 hides it
void 	LCD_HOME (void); 	// home cursor
void	LCD_DAT (char LCDDATA);		// write data to the LCD controller
void	LCD_CMD  (char CMD);	// write a command to the LCD controller
void	LCD_MSG  (char MSG[]);	// send string to LCD at current cursor location
void 	KEPAD_INIT(void);
void 	KEY_RELEASE(void);
void 	PRESSED_KEY(void);
char 	GET_KEY(void);
char  	ASCII_CONV(char d);
void 	small_delay (char d); // 8 bit, about 0.34us per count @22.1MHz
void 	large_delay (char d); // 16 bit, about 82us per count @22.1MHz
void 	huge_delay  (char d); // 24 bit, about 22ms per count @22.1MHz
//-----------------------------------------------------------------------------


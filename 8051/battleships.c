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
#include "battleships.h"
//----------------------------- related Functions --------------------------------
char first = 't';
void main()
{

	if (first=='f') send_char('r');//tell the ARM to Reset.
	first='f';
	Init_Device();
	Init_LCD();
	Init_map();
	Main_loop();
}
void Main_loop()
{
	while(1)
	{
		switch(screen_num)
		{
			case 0:
				//start
				start_screen();
				break;
			case 1:
				//chose difficulty level
				switch_difficulty();
				break;
			case 2:
				counting_screen();
				break;
			case 3:
				//show time and missiles left
				screen_data();				
				break;
			case 4:
				//upper half of map
				screen_map_one();
				break;
			case 5:
				//lower half of map
				screen_map_two();
				break;
			case 6:
				//end
				screen_end(w);
				break;
			default://bug - > print that there is a problem. ask the player to reset the game. "Error screen".
				break;
		}
	}
}


void screen_data()
{
	char key=0;
	while(1)
	{
		get_data();//get time and missiles left from ARM
		if(w!=0)//if we lost\won
		{
		screen_num=6;//go to end
		return;
		}
		print_current_status();//print time and missiles left
		key=GET_KEY();
		large_delay(130);
		if(key==5)//if user asks to get down to the upper half of the map
		{
			screen_num=4;
			return;
		}
	}	
}

void print_current_status()
{
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x02);// move the cursor home
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("Time left: ");
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG(game_timer);
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_GOTO(0x40);
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("Misses left: ");
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_DAT((miss_cnt/10)+'0');
	LCD_BF();
	LCD_DAT((miss_cnt%10)+'0');
	LCD_BF();// wait untill the LCD is no longer busy
}

void get_data()
{
	char s[3];
	short i;
	//send 'd', wait for input 4 times
	check_input_uart();
	check_end();
		if(w!=0)
			return;
	send_char('d');//ask for data
	for(i=0;i<3;i++)//get data
	{
		wait_for_input();
		check_end();
		if(w!=0)
			return;
		s[i]=recieved_note;
	}
	//set time and missiles left
	recieved_note=0;
	game_timer[0]=(s[0]/10) + '0';
	game_timer[1]=(s[0]%10) + '0';
	game_timer[2]=':';
	game_timer[3]=(s[1]/10) + '0';
	game_timer[4]=(s[1]%10) + '0';
	miss_cnt = s[2];
}

//check if there is avaible data.
void check_input_uart()
{
	if (!RI0)
		return;
	recieved_note = SBUF0;
	RI0=0;
}
//wait for data.
void wait_for_input()
{
	while(!RI0);
	recieved_note = SBUF0;
	RI0=0;
}
//send data to ARM.
void send_char(char c)
{
	TI0 = 0;
	SBUF0=	c;
	while(!TI0);
	TI0= 0;
}
//wait for "secs" seconds.
void delay(int secs)
{
	short j=0;
	short i=0;
	for (;j<secs;j++)
	{
		for (;i<28;i++)
		{
			TR0 = 1;	//START COUNTING
			while(!TF0);	//DELAY
			TF0=0;
		}
	}
}

void Init_LCD()
{
	red=0;
	green=0;
	yellow=0;
	blue=0;
	LCD_BF(); // wait untill the LCD is no longer busy
	LCD_INIT();// initialize the LCD to 8 bit mode
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x06); // curser moves from left to right
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x02);// move the cursor home
	LCD_BF();// wait untill the LCD is no longer busy
	
}

void start_screen()
{
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("BattleShips!");
	LCD_BF();// wait untill the LCD is no longer busy
	delay(2);
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	screen_num=1;
}


void switch_difficulty()
{
	char key;
	LCD_CLRS(); // clears the display
	LCD_CMD(0x02);// move the cursor home
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("Please choose");
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_GOTO(0x40);
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("difficulty:");
	LCD_BF();// wait untill the LCD is no longer busy
	PRESSED_KEY();	//wait until pressing
	key = GET_KEY();	//save the key pressed
	KEY_RELEASE();		//wait until releasing
	LCD_DAT(ASCII_CONV(key));
	LCD_BF();// wait untill the LCD is no longer busy
	screen_num=2;
	set_difficulty(key+1);
	delay(2);
}

void set_difficulty(char difficulty)
{
	//difficulty level range : 1-3 (on keyboard). 
	if (difficulty>3)
	{
		delay(1);
		screen_num=1;
		LCD_CLRS(); // clears the display
		LCD_CMD(0x02);// move the cursor home
		LCD_BF();// wait untill the LCD is no longer busy
		LCD_MSG("range: 1-3");
		LCD_BF();// wait untill the LCD is no longer busy
		delay(1);
		return;
	}
	send_char(difficulty);//let the arm decide what difficulty parameters should be.
}

void counting_screen()
{
	wait_for_input();
	recieved_note=0;
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("3");
	LCD_BF();// wait untill the LCD is no longer busy
	delay(1);
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("2");
	LCD_BF();// wait untill the LCD is no longer busy
	delay(1);
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("1");
	LCD_BF();// wait untill the LCD is no longer busy
	delay(1);
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("GO!");
	LCD_BF();// wait untill the LCD is no longer busy
	delay(1);
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	send_char('s');//indicate the ARM the game starts now.
	screen_num=3;//change to screen_data.
}
//upper half of the map
void screen_map_one()
{
	char key=0; //used to read input from the user keyboard.
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_GOTO(0x00);
	LCD_BF();
	print_map(screen_num);
	//---------> move cursor to right place
	if(cursor>15)
		LCD_GOTO(cursor + 0x30);
	else
		LCD_GOTO(cursor);
	LCD_BF();// wait untill the LCD is no longer busy
	while(1) // kind of a main loop
	{
		key = GET_KEY();
		if(key!=0) 
		{
			red=0;
			green=0;
			yellow=0;
			blue=0;
			switch(key)
			{
			case 1://move cursor up. may chagnge the screen.
				if (cursor<=15)
				{
					screen_num=3;//change to screen_data.
					return;
				}
				cursor-=16;//go to upper line
				LCD_GOTO(cursor);
				break;
			case 4://move cursor left.
				if (cursor==0 || cursor==16)//can't go behind the screeen.
				{
					cursor+=15;
					LCD_BF();
					LCD_GOTO(cursor+3*(cursor/16));
					break;
				}
				cursor--;
				LCD_BF();
				//------------>move left
				if(cursor>15)
					LCD_GOTO(cursor + 0x30);
				else
					LCD_GOTO(cursor);
				break;
			case 5://move cursor down. may change screen.
				if (cursor>=16)
				{
					cursor-=16;
					screen_num=5;//go to bottom half of the map.
					return;
				}
				cursor+=16;
				LCD_BF();
				LCD_GOTO(cursor + 0x30);
				break;
			case 6://move cursor right.
				if (cursor==15 || cursor==31)//can't go behind the screeen.
				{
					cursor-=15;
					LCD_BF();
					LCD_GOTO(cursor+3*(cursor/16));
					break;
				}
				cursor++;
				LCD_BF();
				//------------>move right
				if(cursor>15)
					LCD_GOTO(cursor + 0x30);
				else
					LCD_GOTO(cursor);
				break;
			}
		}
		key=0;
		//if a win or lose message came
		check_input_uart();
		check_end();
		if(w!=0)
			return;
		//hit
		if(SW4 == 0)
		{
			if(map[cursor/16][cursor%16] =='X'||map[cursor/16][cursor%16]=='O') return;
			//independently because long binary sentence arn't working well.
			if(map[cursor/16][cursor%16]=='S')return;
			red=0;
			green=0;
			yellow=0;
			blue=0;
			send_char((char)cursor+1);//send to ARM the hit location.
			wait_for_input();//wait for ARM to respond.
			if(recieved_note == 'h')//if hit
			{
				green=1;
				recieved_note=0;
				map[cursor/16][cursor%16] = 'X';
				LCD_BF();
				LCD_DAT('X');
				LCD_BF();
				LCD_CMD(0x10);
			}
			else if(recieved_note == 'm')//if miss
			{
				red=1;
				recieved_note=0;
				map[cursor/16][cursor%16] = 'O';
				LCD_BF();
				LCD_DAT('O');
				LCD_BF();
				LCD_CMD(0x10);
			}
			else if(recieved_note == 'p')//if falles ship
			{
				red=1;
				green=1;
				yellow=1;
				blue=1;
				recieved_note=0;
				update_fallen_ship();
				return;
			}
			check_input_uart();
			check_end();
			if(w!=0)
				return;
			while(SW4 == 0);//wait untill switch4 in released.
		}
	}
}
//lower half of the map
void screen_map_two()
{
	char key=0; //used to read input from the user keyboard.
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_GOTO(0x00);
	LCD_BF();
	print_map(screen_num);
	//---------> move cursor to right place
	if(cursor>15)
		LCD_GOTO(cursor + 0x30);
	else
		LCD_GOTO(cursor);
	LCD_BF();// wait untill the LCD is no longer busy
	while(1) // kind of a main loop
	{
		key = GET_KEY();
		if(key!=0) 
		{
			red=0;
			green=0;
			yellow=0;
			blue=0;
			switch(key)
			{
			case 1://move cursor up. may chagnge the screen.
				if (cursor<=15)
				{
					screen_num=4;//change to screen_data.
					cursor+=16;
					return;
				}
				cursor-=16;//go to upper line
				LCD_GOTO(cursor);
				break;
			case 4://move cursor left.
				if (cursor==0 || cursor==16)//can't go behind the screeen.
				{
					cursor+=15;
					LCD_BF();
					LCD_GOTO(cursor+3*(cursor/16));
					break;
				}
				cursor--;
				LCD_BF();
				//------------>move left
				if(cursor>15)
					LCD_GOTO(cursor + 0x30);
				else
					LCD_GOTO(cursor);
				break;
			case 5://move cursor down. may change screen.
				if (cursor>=16)
				{
					return;
				}
				cursor+=16;
				LCD_BF();
				LCD_GOTO(cursor + 0x30);
				break;
			case 6://move cursor right.
				if (cursor==15 || cursor==31)//can't go behind the screeen.
				{
					cursor-=15;
					LCD_BF();
					LCD_GOTO(cursor+3*(cursor/16));
					break;
				}
				cursor++;
				LCD_BF();
				//------------>move right
				if(cursor>15)
					LCD_GOTO(cursor + 0x30);
				else
					LCD_GOTO(cursor);
				break;
			}
		}
		key=0;
		check_input_uart();//if a win or lose message came
		check_end();
		if(w!=0)
			return;
		//hit
		if(SW4 == 0)
		{
			if(map[(cursor/16)+2][cursor%16] =='X'||map[(cursor/16)+2][cursor%16]=='O') return;
			if(map[(cursor/16)+2][cursor%16]=='S')return;
			red=0;
			green=0;
			yellow=0;
			blue=0;
			send_char((char)cursor+33);//send to ARM the hit location. +32 offset indicate 2' screen
			wait_for_input();//wait for ARM to respond.
			if(recieved_note == 'h')//if hit
			{
				green=1;
				recieved_note=0;
				map[cursor/16+2][cursor%16] = 'X';
				LCD_BF();
				LCD_DAT('X');
				LCD_BF();
				LCD_CMD(0x10);
			}
			else if(recieved_note == 'm')//if miss
			{
				red=1;
				recieved_note=0;
				map[cursor/16+2][cursor%16] = 'O';
				LCD_BF();
				LCD_DAT('O');
				LCD_BF();
				LCD_CMD(0x10);
			}
			else if(recieved_note == 'p')//falles ship
			{
				red=1;
				green=1;
				yellow=1;
				blue=1;
				recieved_note=0;
				update_fallen_ship();
				return;
			}
			check_input_uart();
			check_end();
			if(w!=0)
				return;
			while(SW4 == 0);
		}
	}
}
//get 3 pos of the falles ship from the uart.
void update_fallen_ship()
{
	char pos[3];
	int i=0;
	for(;i<3;i++)//get cordinates of the fallen ship
	{
		wait_for_input();
		check_end();
		if(w!=0)
			return;
		pos[i]=recieved_note;
	}
	//update on map
	recieved_note=0;
	map[pos[0]/16][pos[0]%16] = 'S';//'S' = fallen ship
	if(pos[1]!='e')
		map[pos[1]/16][pos[1]%16] = 'S';
	if(pos[2]!='e')
		map[pos[2]/16][pos[2]%16] = 'S';
}


void check_end()
{
	if(recieved_note =='w'||recieved_note=='l')
	{
		w=recieved_note;
		screen_num=6;
		return;
	}
}

//fill map with blanks .
void Init_map()
{
	short i=0;
	short j=0;
	for(j=0;j<2;j++)
	{
		for(i=0;i<16;i++)
		{
			map[j][i]= '-';//represent "unchecked"  = '-' in uppper half map
			map[j+2][i]='+';//represent "unchecked"  = '+' in bottom half map
		}
	}
	
	
}
//print the map by the right screen. 
//screen 2 = upper half. 
//sceren 3 = bottom half.
void print_map(int screen)
{
	short i;
	if (screen==4 || screen==5)
	{
		
			for (i=0;i<16;i++)
			{
				LCD_BF();
				LCD_DAT(map[(0+2*(screen/5))][i]);
				LCD_BF();
			}
			LCD_GOTO(0x40);
			for (i=0;i<16;i++)
			{
				LCD_BF();
				LCD_DAT(map[1+2*(screen/5)][i]);
				LCD_BF();
			}
	}
}


void screen_end(char win)
{
	char* message;
	if (win=='w')
	{
		message = "WINNER!";
	}
	else if(win=='l')
	{
		message = "LOOSER!";
	}
	else
	{
		message = "ERROR!";
	}
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CMD(0x02);// move the cursor home
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG(message);
	LCD_BF();// wait untill the LCD is no longer busy
	delay(4);//wait 4 seconds.
	LCD_CLRS();
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_MSG("please press");
	LCD_BF();
	LCD_GOTO(0x40);
	LCD_BF();
	LCD_MSG("reset button!.");
	LCD_BF();// wait untill the LCD is no longer busy
	while(1);
}
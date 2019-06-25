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

void main()
{
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
				start_screen();
				switch_difficulty();
				counting_screen();
				break;
			case 1:
				screen_data();
				break;
			case 2:
				screen_map_one();
				break;
			case 3:
				screen_map_two();
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
		//get_data();
		print_current_status();
		key=GET_KEY();
		if(key==5)
		{
			screen_num=2;
			return;
		}
	}	
}

void print_current_status()
{
	char miss;
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
	miss=miss_cnt/10;
	LCD_DAT(miss+'0');
	LCD_BF();
	miss=miss_cnt%10;
	LCD_DAT(miss+'0');
	LCD_BF();// wait untill the LCD is no longer busy
}

void get_data()
{
	char s[3];
	int i;
	//send 'd', wait for input 4 times
	send_char('d');
	for(i=0;i<3;i++)
	{
		wait_for_input();
		s[i]=recieved_note;
	}
	recieved_note=0;
	game_timer[0]=(s[0]/10) + '0';
	game_timer[1]=(s[0]%10) + '0';
	game_timer[2]=':';
	game_timer[3]=(s[1]/10) + '0';
	game_timer[4]=(s[1]%10) + '0';
	miss_cnt = (s[2]/10)*10 + (s[2]%10);
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
	//#devnote: add :check if the arm got the tarsmission ?
}
//wait for "secs" seconds.
//#devnote: check with oscilator if the freq is legit
void delay(int secs)
{
	int j=0;
	int i=0;
	for (;j<secs;j++)
	{
		for (;i<28;i++)
		{
			TR0 = 1;	//START COUNTING
			while(!TF0);	//DELAY
		}
	}
}

void Init_LCD()
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
	set_difficulty(key+1);
	delay(2);
	//#devnote: add :check if difficulty
}

void set_difficulty(char difficulty)
{
	char key;
	if (difficulty>3)
	{
		LCD_BF();
		LCD_MSG("input field:");
		LCD_BF();
		LCD_GOTO(0x40);
		LCD_BF();
		LCD_MSG("1-3");
		delay(2);
		while(difficulty>3)
		{
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
			difficulty = GET_KEY();	//save the key pressed
			KEY_RELEASE();		//wait until releasing
			LCD_DAT(ASCII_CONV(key));
			delay(2);
		}
		LCD_CLRS(); // clears the display
	}
	send_char(difficulty);//let the arm decide what difficulty parameters should be.
}

void counting_screen()
{
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
	screen_num=1;//change to screen_data.
}

void Reset_isr() interrupt 1
{
	game_timer[0]='0';
	game_timer[1]='0';
	game_timer[3]='0';
	game_timer[4]='0';
	miss_cnt=0;
	TI0 = 0;
	send_char('r'); //sending "Reset" to ARM.
	Init_LCD();
	KEPAD_INIT();	// initialize the keypad
	screen_num=0;
	recieved_note=0;
	cursor =0;
	Init_map();
	Main_loop();//#devnote: when will the interrupt end ? there is a bug !.
}

void screen_map_one()
{
	char key=0; //used to read input from the user keyboard.
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	//---------> move cursor to right place
	if(cursor>15)
		LCD_GOTO(cursor + 0x30);
	else
		LCD_GOTO(cursor);
	LCD_BF();// wait untill the LCD is no longer busy
	print_map(2);
	while(1) // kind of a main loop
	{
		key = GET_KEY();
		if(key!=0) 
		{
			switch(key)
			{
			case 1://move cursor up. may chagnge the screen.
				if (cursor<=15)
				{
					screen_num=1;//change to screen_data.
					return;
				}
				cursor-=16;//go to upper line
				LCD_GOTO(cursor);
				break;
			case 4://move cursor left.
				if (cursor==0 || cursor==16)//can't go behind the screeen.
					break;
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
					screen_num=3;//go to bottom half of the map.
					return;
				}
				cursor+=16;
				LCD_BF();
				LCD_GOTO(cursor + 0x30);
				break;
			case 6://move cursor right.
				if (cursor==15 || cursor==31)//can't go behind the screeen.
					break;
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
		//hit
		if(SW4 == 0)
		{
			send_char((char)cursor);//send to ARM the hit location.
			wait_for_input();//wait for ARM to respond.
			if(recieved_note == 'h')//if hit
			{
				recieved_note=0;
				map[cursor/16][cursor%16] = 'X';
				LCD_BF();
				LCD_DAT('X');
				LCD_BF();
			}
			else if(recieved_note == 'm')//if miss
			{
				recieved_note=0;
				map[cursor/16][cursor%16] = 'O';
				LCD_BF();
				LCD_DAT('O');
				LCD_BF();
			}
			else if(recieved_note == 'p')
			{
				recieved_note=0;
				update_fallen_ship();
				return;
			}
		}
	}
}

void screen_map_two()
{
	char key=0; //used to read input from the user keyboard.
	LCD_BF();// wait untill the LCD is no longer busy
	LCD_CLRS(); // clears the display
	LCD_BF();// wait untill the LCD is no longer busy
	//---------> move cursor to right place
	if(cursor>15)
		LCD_GOTO(cursor + 0x30);
	else
		LCD_GOTO(cursor);
	LCD_BF();// wait untill the LCD is no longer busy
	print_map(3);
	while(1) // kind of a main loop
	{
		key = GET_KEY();
		if(key!=0) 
		{
			switch(key)
			{
			case 1://move cursor up. may chagnge the screen.
				if (cursor<=15)
				{
					screen_num=2;//change to screen_data.
					return;
				}
				cursor-=16;//go to upper line
				LCD_GOTO(cursor);
				break;
			case 4://move cursor left.
				if (cursor==0 || cursor==16)//can't go behind the screeen.
					break;
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
					break;
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
		//hit
		if(SW4 == 0)
		{
			send_char((char)cursor+128);//send to ARM the hit location. +128 offset indicate 2' screen
			wait_for_input();//wait for ARM to respond.
			if(recieved_note == 'h')//if hit
			{
				recieved_note=0;
				map[cursor/16][cursor%16] = 'X';
				LCD_BF();
				LCD_DAT('X');
				LCD_BF();
			}
			else if(recieved_note == 'm')//if miss
			{
				recieved_note=0;
				map[cursor/16][cursor%16] = 'O';
				LCD_BF();
				LCD_DAT('O');
				LCD_BF();
			}
			else if(recieved_note == 'p')
			{
				recieved_note=0;
				update_fallen_ship();
				return;
			}
		}
	}
}
//get 3 pos of the falles ship from the uart.
void update_fallen_ship()
{
	char pos[3];
	wait_for_input();
	pos[0]=recieved_note;
	wait_for_input();
	pos[1]=recieved_note;
	wait_for_input();
	pos[2]=recieved_note;
	recieved_note=0;
	map[(pos[0]/128)+(pos[0]%128)/16][(pos[0]%128)%16] = 95;// 95 = /
	if(pos[1]!='e')
		map[(pos[1]/128)+(pos[1]%128)/16][(pos[1]%128)%16] = 95;
	if(pos[2]!='e')
		map[(pos[2]/128)+(pos[2]%128)/16][(pos[2]%128)%16] = 95;
}



//fill map with blank boxes.
void Init_map()
{
	int i=0;
	int j=0;
	for(j=0;j<4;j++)
	{
		for(i=0;i<16;i++)
		{
			map[j][i]= 219;//219 represent unchecked box
		}
	}	
	
}
//print the map by the right screen. 
//screen 2 = upper half. 
//sceren 3 = bottom half.
void print_map(int screen)
{
	int i;
	if (screen==2 || screen==3)
	{
		
			for (i=0;i<16;i++)
			{
				LCD_BF();
				LCD_DAT(map[(0+2*(screen/3))][i]);
				LCD_BF();
			}
			LCD_GOTO(0x40);
			for (i=0;i<16;i++)
			{
				LCD_BF();
				LCD_DAT(map[1+2*(screen/3)][i]);
				LCD_BF();
			}
	}
}


void end() interrupt 0
{
	wait_for_input();
	screen_end(recieved_note);
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include <stdlib.h>
#include <time.h>

#define COM_PORT 3 // gpioPortD (USART location #1: PD0 and PD1)
#define UART_TX_pin 0

//functions signatures
void update_time(char* timer);						//reduce timer by one second
void Init();										//initialization of the device
void send_data(char data);							//sends "data" to 8051
void check_input();									//checks if there is an input in the UART buffer
void make_ships();									//making ships
void screen_pre_start();							//includes all needed operations to be executed before the game starts
void screen_game();									//respond to user requests(get data, check hit, reset),check if the game has ended.
bool check_vio_bounds(char pos, int size, char dir);//return true if the new ship doesn't go outside the map boundaries.
bool check_other_ships(char pos, int size,char dir);//return true if there are no ships in the area of the new ship.
char valid_ship(char pos, int size, char dir);		//direction : 0=down, 1=up, 2=right, 3=left. checks if the inserted ship is legal.
void main_loop();									//main routine of the game
void set_difficulty(char level);					//set difficulty level parameters
void wait_for_input();								//wait for an input from from 8051
void Init_Ships();									//set all the ships to 'e'= empty slot
void check_win();									//checks if the user  has won.
void eog();											//end of game. send result to 8051 and wait for reset.
void TIMER0_IRQHandler(void);						//OF interrupt of timer 0. call update_time and set OF flag off.

//variables
char timer[5];										//game timer (for example 15:00)
char missiles;										//missiles left
short ships_count=0;								//how many ships are created
char ships[8][3];									//2'd array of ships positions(position legal range is from 0 to 63).
char input=0;										// input from 8051 will be stored here
short screen_num=0;									//index of current screen
char win=0;											//win\lose indication


int main(void)
{
  /* Chip errata */
  CHIP_Init();
  Init();
  srand(time(NULL));								//true random generator
  input=0;
  main_loop();
}



void main_loop()
{
	 while(1)
	  {
		  switch(screen_num)
		  {
		  case -1:									//after the user requested to reset the game
			  Init();
			  screen_num++;
			  break;
		  case 0:									//do all needed before the game starts
			  screen_pre_start();
			  break;
		  case 1:									//game
			  screen_game();
			  break;
		  case 2:									//end of game
			  eog();
			  break;
		  }
	  }
}
void Init_Ships()
{
	//###set all ships to be "empty" (zero state)
	for (int i=0; i<14;i++)
		for(int j=0;j<3;j++)
			ships[i][j]='e';						//'e' == empty slot.
}
//###make all needed operations before the game starts
void screen_pre_start()
{
	wait_for_input();								//waiting for difficulty level.
	if(input=='r')									//if the user requested to reset
	{
		screen_num=-1;
		return;
	}
	else if(input>3||input<0)						//illegal difficulty level
		GPIO->P[4].DOUT |=0x00000004;				// light led
	set_difficulty(input);							//sets difficulty level
	make_ships();									//make ships
	send_data('s');									//tell 8051 ARM is ready
	input=0;
	wait_for_input();								//wait for the game to start.
	while(input!='s')
	{
		if(input=='r')								//if the user requested to reset
		{
			screen_num=-1;
			return;
		}
		input=0;
		wait_for_input();							//wait for the game to start.
	}

	input=0;
	screen_num=1;									//screen game
	TIMER0->CMD = 0x00000001;						// start timer
}
//###respond to user requests (get data, check hit, reset) and check if the game has ended.
void screen_game()
{
	check_input();
	switch (input)
	{
		case 'd':									//user asked for data
			send_data((timer[0]-'0')*10+timer[1]-'0');
			send_data((timer[3]-'0')*10+timer[4]-'0');
			send_data(missiles);
			break;
		case 'r':									//user asked to reset
				screen_num=-1;
				return;
				break;
		default:									//user pressed "hit". input = hit location +1
			if(input>64 || input<1)
				break;
			missiles--;								//one missiles less
			input--;								//location fix
			short c_fallen =0; 						// check fallen ship
			char hit = 'm';
			for (int i=0;i<ships_count;i++)
			{
				c_fallen=0;
				for(int j=0;j<3;j++)
				{
					if(ships[i][j]>=128||ships[i][j]=='e')
						c_fallen++;					//if this place was already hit
					if(ships[i][j]==input)			//means there is a hit
					{
						hit='h';
						ships[i][j]+=128;			//mark this place as hit
						c_fallen++;
					}
				}
				if(c_fallen==3)						//this ship is fully fallen
				{
					hit ='p';						//tell 8051 there is a fallen ship
					send_data(hit);
					for(int j=0;j<3;j++)			//send 3 coordinates of this ship
					{
						if (ships[i][j]>=128)
							send_data(ships[i][j]-128);
						else
							send_data(ships[i][j]);
						ships[i][j]='x';			//means ship was sent. this is a fallen ship.
					}
					break;
				}
				if(hit=='h')						//tell 8051 there is a hit
				{
					send_data(hit);
					break;
				}
			}
			if (hit=='m')							//tell 8051 that is a miss
			{
				send_data(hit);//send miss
			}

			if(missiles==0)							//if the user is out of missiles
			{
				win='l';							//lose indication
				screen_num=2;						//end of game
				return;
			}
	}
	input=0;
	check_win();
	if(win!=0)
	{
		screen_num=2;								//end of game
		return;
	}
}
//check if all the ships have fallen. if they are, the user win the game.
void check_win()
{
	for(int i=0;i<ships_count;i++)
		for(int j=0;j<3;j++)
		{
			if(ships[i][j]!='X'&&ships[i][j]!='e'&&ships[i][j]!='x')
				return;								//if the ship has a position which wasn't hit.
		}
	win='w';										//if all ships are fallen
}
//###make ships and make sure they are legal according to the game rules.
void make_ships()
{

char direction;//direction : 0=down, 1=up, 2=right, 3=left.
char pos;
//## make 2 ships in size of 3
for (int i=0; i<2;i++)
	{
		pos=rand()%64;
		direction=rand()%3;
		direction = valid_ship(pos,3,direction);	//get a good direction from this position. 0 means no good direction
		while(direction==0)							//make the ship valid, try other positions.
		{
			pos=rand()%64;
			direction=rand()%3;
			direction =valid_ship(pos,3,direction);
		}
		direction--;								//fix
		switch(direction)							//set ship coordinates
		{
		case 0:
			ships[i][0]=pos;
			ships[i][1]=pos+16;
			ships[i][2]=pos+32;
			break;
		case 1:
			ships[i][0]=pos;
			ships[i][1]=pos-16;
			ships[i][2]=pos-32;
			break;
		case 2:
			ships[i][0]=pos;
			ships[i][1]=pos+1;
			ships[i][2]=pos+2;
			break;
		case 3:
			ships[i][0]=pos;
			ships[i][1]=pos-1;
			ships[i][2]=pos-2;
			break;
		}
		ships_count++;								//one more ship was created
	}
//###make ships in size of 2 chars
for (int i=0; i<2;i++)
	{
		pos=rand()%64;
		direction=rand()%3;
		direction = valid_ship(pos,2,direction);	//get a good direction from this position. 0 means no good direction
		while(direction==0)							//make the ship valid, try other positions.
		{
			pos=rand()%64;
			direction=rand()%3;
			direction =valid_ship(pos,2,direction);
		}
		direction--;								//fix
		switch(direction)							//set ship coordinates
				{
				case 0:
					ships[i+2][0]=pos;
					ships[i+2][1]=pos+16;
					break;
				case 1:
					ships[i+2][0]=pos;
					ships[i+2][1]=pos-16;
					break;
				case 2:
					ships[i+2][0]=pos;
					ships[i+2][1]=pos+1;
					break;
				case 3:
					ships[i+2][0]=pos;
					ships[i+2][1]=pos-1;
					break;
				}
		ships_count++;								//one more ship was created
	}
//##make ships in size of 1 chars
for (int i=0; i<4;i++)
	{
		pos=rand()%64;
		direction=rand()%3;
		direction = valid_ship(pos,1,direction) ;
		while(direction==0)							//make the ship valid
		{
			pos=rand()%64;
			direction=rand()%3;
			direction =valid_ship(pos,1,direction);
		}
		direction--;
		ships[i+4][0]=pos;
		ships_count++;
	}
}
//###return true if the new ship doesn't go outside the map boundaries.
bool check_vio_bounds(char pos, int size, char dir)//direction : 0=down, 1=up, 2=right, 3=left.
{
	if (dir==0)								//if down and not violating boundaries
	{
		if((size ==2 && pos/16==3)||(size ==3 && pos/16>=2))
			return false;
		return true;
	}
	else if (dir==1)								//if up and not violating boundaries
	{
		if((size ==2 && pos/16==0)||(size ==3 && pos/16<=1))
			return false;
		return true;
	}
	else if (dir==2)								//if right and not violating boundaries
	{
		if((size ==2 && pos%16==15)||(size ==3 && pos%16>=14))
			return false;
		return true;
	}
	else 								//if left and not violating boundaries
	{
		if((size ==2 && pos%16==0)||(size ==3 && pos%16<=1))
			return false;
		return true;
	}
	return true;
}
//###return true if there are no ships in the area of the new ship.
bool check_other_ships(char pos, int size, char dir)//direction : 0=down, 1=up, 2=right, 3=left.
{
	short down=1;
	short right=1;
	if (dir==0)
		down=1;
	else if(dir==1)
		down=-1;
	else if(dir==2)
		right=1;
	else if(dir==3)
		right=-1;
	///### make sure there are no ships around the desired area of this new ship
	for(int index=0;index<ships_count;index++)
		for(int spos=0;spos<3;spos++)
			for (int i =-1;i<(dir<=1)*size+2;i++)
				for (int j =-1;j<(dir>1)*size+2;j++)
				{
					if (ships[index][spos]==((pos/16)+down*(i))*16 + (pos%16)+right*j)
						return false;
				}

	return true;
}
//###check if the ship is valid. if not , try other directions.
char valid_ship(char pos, int size, char dir)		//direction : 0=down, 1=up, 2=right, 3=left.
{
	//try all directions
	for (int i=0; i<4; i++)
	{
		if(check_vio_bounds(pos,size,dir))			//ensure safe bounds in this direction
			if(check_other_ships(pos,size,dir))		//check if there are no ships around
				return dir+1;
		dir++;
		if(dir==4)
			dir=0;
	}
	//if no direction fits
	return 0;
}

//updating "timer"
void update_time(char* timer)
{
	if (timer[2] != ':')
		return;
	timer[4]--;
	if (timer[4]== '0'-1)
	{
		timer[3]--;
		timer[4]='9';
		if (timer[3]== '0'-1)
			{
				timer[1]--;
				timer[3]='5';

			if (timer[1]== '0'-1)
				{
					timer[0]--;
					timer[1]='9';
					if (timer[0]=='0'-1)
					{
						//exit game
						win='l';//lose
						screen_num=2;
					}
				}
			}
	}
}
//initialize the program.
void Init()
{
	GPIO->P[1].MODEH|=0x00000020;					// define as input
	GPIO->P[4].MODEL|=0x00000400;					//led for debug.
	Init_Ships();
	CMU->CTRL |= (1 << 14);							// Set HF clock divider to /2 to keep core frequency <32MHz
	CMU->OSCENCMD |= 0x4;							// Enable XTAL Oscillator
	while(! (CMU->STATUS & 0x8) );					// Wait for XTAL osc to stabilize
	CMU->CMD = 0x2;									// Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock should be 24MHz
	CMU->HFPERCLKEN0 |=0x00002022;					//enable GPIO clk & USART1 clk & timer0 clk
	TIMER0->CTRL|=0x09000000;						// prescaler = 1024
	TIMER0->CMD =0x00000002;						// stop timer
	TIMER0->CNT = 0x00000000;						// set value to 0
	TIMER0->TOP= 0xB71B;							//timer counts until B71B
	NVIC_EnableIRQ(TIMER0_IRQn);					// enable timer0 interrupt
	TIMER0->IEN |= 0x00000001;						// enable OF
	GPIO->P[COM_PORT].MODEL = (1 << 4) | (4 << 0);	// Configure PD0 as digital output and PD1 as input
	GPIO->P[COM_PORT].DOUTSET = (1 << UART_TX_pin);	// Initialize PD0 high since UART TX idles high (otherwise glitches can occur)
	USART1->CLKDIV = (48 << 6);						//set CLKDIV to get baud rate of 115200
	USART1->CMD = 0xC05;							// clear all USART flags
	USART1->IFC = 0x1FF9;							// clear all USART interrupt flags
	USART1->ROUTE = 0x103;							//enable pins Tx, Rx
	check_input();
	input=0;
	win=0;
	ships_count=0;
}
//send one char via UART
void send_data(char data)
{
	while( !(USART1->STATUS & (1 << 6)) );	// wait for TX buffer to empty
	USART1->TXDATA= data;
}

void check_input()
{
	if(USART1->STATUS & (1 << 7))			// if RX buffer contains valid data
	{
		   input = USART1->RXDATA;			// store the data
	}
}
void wait_for_input()
{
	while(!(USART1->STATUS & (1 << 7)));	// if RX buffer contains valid data
    input = USART1->RXDATA;					// store the data
}

//timer0 OF interrupt
void TIMER0_IRQHandler(void)
{
	TIMER0->IFC |= 0x00000001;				//turn off OF flag
	update_time(timer);						//one second less
}
void set_difficulty(char level)
{
	switch(level)							//set timer and missiles according to difficulty level
	{
		case 1:
			timer[0]='1';
			timer[1]='0';
			timer[2]=':';
			timer[3]='0';
			timer[4]='0';
			missiles=45;
			break;
		case 2:
			timer[0]='0';
			timer[1]='6';
			timer[2]=':';
			timer[3]='0';
			timer[4]='0';
			missiles=34;
			break;
		case 3:
			timer[0]='0';
			timer[1]='2';
			timer[2]=':';
			timer[3]='3';
			timer[4]='0';
			missiles=22;
			break;
	}
}

//###send the result of the game and wait for reset instruction from 8051
void eog()
{
	  TIMER0->CMD =0x00000002;			// stop timer
	  GPIO->P[4].DOUT |=0x00000004;		// light led
	  while(input!='e')					//tell 8051 the game was ended
	  {
		  send_data(win);
		  check_input();				//note that 8051 got the message
	  }
	  input=0;
	  wait_for_input();					//wait until user request to reset the game
	  GPIO->P[4].DOUT &=~0x00000004;	//turn off led
	  screen_num=-1;					//do INIT again
}

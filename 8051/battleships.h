//#include <C8051F020.h>				// Include register definition file.


char game_timer[5]; 	  				//time left
int miss_cnt=0;							//mistakes left
char recieved_note=0; 					// revieced note from UART.
char map[4][16]; 						// blank map of ships. updated by ARM.
int screen_num=0;						// represent no. of screen. no=0 => start screen
int cursor =0;							//position of the cursor in game time

//sbit SW1 = P0^4;						//reset button
//sbit SW4 = P0^7;						//hit button
//--------------------------------------//
//---------------Functions--------------//
//--------------------------------------//
void Init_map();						//initialize the "map" array.
void Init_Device(void);					//divice initialization (config wizzard)
void Init_LCD();						//initialize the lcd for a new game
void start_screen();					//presenting primary game screen
void switch_difficulty();				//asking the player for difficulty level and set it.
void set_difficulty(char difficulty);	//set global vars according to the difficulty
void counting_screen();					//counting back from 3 to 1 and game starts!
void Reset_isr(void);					//main function - initialization of a new game
void Main_loop();						//Main loop of the whole program.
void update_data();						//updateing current time and num of mistakes.
void delay(int secs);					//#devnote: need to be fixed. there is a new oscilator frequency.
void check_input_uart();				// check if there is input from the ARM
void send_char(char c);					//send char 'c' to ARM
void print_map(int screen);				//print map relevent to screen
void screen_map_one();					//screen map upper half.
void wait_for_input();					//wait for input in uart0.
void screen_data();						//get data from arm an printing
void print_current_status();			//print the time and left mistakes on the lcd.
void get_data();						//update game_timer and miss_cnt
void screen_map_two();			   		//screen map bottom half.
void screen_end(char win);				//ending screen.
void end();								//external interrupt 0  --> end of game.

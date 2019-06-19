#include <C8051F020.h>                  // Include register definition file.


sbit SW1 = P0^4;
sbit SW2 = P0^5;



void Init_Device(void);					//init
void Init_LCD();
void start_screen();
int switch_difficulty();
void counting_screen();
void Reset_isr() interrupt 0;			//initialize new game
void Hit() interrupt 2;					//take the current location of the cursor and send it to the arm
void Write_hit();						//print on the lcd "X" in the hit location
void Write_miss();						//print on the lcd "O" in the miss location
char* Get_time();						//returning string of the current time left.
char* Get_mistakes();					//returning string containing number of misses
void Lose();							//print on the lcd lose announce
void Win();								//print on the lcd win announce
void Show_screen(int screen_num);		//print on the lcd "screen_num"-th screen
void Main_loop();
void update_data();						//updateing current time and num of mistakes.
void delay(int secs);


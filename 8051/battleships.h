//#include <C8051F020.h>                  // Include register definition file.


sbit SW1 = P0^4;
sbit SW2 = P0^5;

char* game_timer;
int miss_cnt;

void Init_Device(void);					//divice initialization (config wizzard)
void Init_LCD();						//initialize the lcd for a new game
void start_screen();					//presenting primary game screen
void switch_difficulty();				//asking the player for difficulty level and set it.
void set_difficulty(char difficulty);	//set global vars according to the difficulty
void counting_screen();					//counting back from 3 to 1 and game starts!
void Reset_isr(void);			//main function - initialization of a new game
void Hit(void);					//take the current location of the cursor and send it to the arm
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
void check_input();						// check if there is input from the ARM
void send_char(char c);						//send char 'c' to ARM
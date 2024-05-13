//--------------------------------------------------------
// Application demonstrator: SNAKE game
//--------------------------------------------------------


#include "EDK_CM0.h" 
#include "core_cm0.h"
#include "edk_driver.h"
#include "edk_api.h"

#include <stdio.h>

//Maximum snake length
//#define N 100							

//Game region
#define left_boundary 5
#define right_boundary 96
#define top_boundary 5
#define bottom_boundary 116
#define boundary_thick 1

// Paddle Size and Movement Speed
#define paddle_width 2
#define paddle_height 10
#define paddle_speed 2

// Ball size and movement speed
#define ball_size 1
#define ball_speed 1

// Global variables
static int score1;
static int score2;
static int paddle1_pos;
static int paddle2_pos;
static int ball_x;
static int ball_y;
static int ball_speed_x;
static int ball_speed_y;
static int ball_dir_x;
static int ball_dir_y;
static int pause;
static int gamespeed;


// Structure define
// Target cordinates and variable to indicate whether target is hit
struct target{
	int x;
	int y;
	int reach;
	}target;


//---------------------------------------------
// Game
//---------------------------------------------

void Game_Init(void)
{	
	//Draw a game region
	clear_screen();
	
	rectangle(left_boundary,top_boundary,right_boundary,top_boundary+boundary_thick,BLUE);//draws top boundary
	rectangle(left_boundary,top_boundary,left_boundary+boundary_thick,bottom_boundary,BLUE);//draws left boundary
	rectangle(left_boundary,bottom_boundary,right_boundary,bottom_boundary+boundary_thick,BLUE);//draws bottom boundary
	rectangle(right_boundary,top_boundary,right_boundary+boundary_thick,bottom_boundary+boundary_thick,BLUE);//draws right boundary

	//Initialise data	
		score1 = 0;
		score2 = 0;
    paddle1_pos = (bottom_boundary - top_boundary) / 2;
    paddle2_pos = (bottom_boundary - top_boundary) / 2;
    ball_x = (left_boundary + right_boundary) / 2;
    ball_y = (top_boundary + bottom_boundary) / 2;
    ball_speed_x = ball_speed;
    ball_speed_y = ball_speed;
    ball_dir_x = 1;
    ball_dir_y = 1;		
		gamespeed = 6;
	
	//Initialise timer (load value, prescaler value, mode value)
	timer_init((Timer_Load_Value_For_One_Sec/gamespeed),Timer_Prescaler,1);	
	timer_enable();
	
	target.reach=1;
	

	//By default set pause is 0
	pause=0;
	
 // Print instructions on text console of VGA
    printf("\n------- Pong Game --------");
    printf("\nCentre btn ..... hard reset");
    printf("\nKeyboard r ..... soft reset");
    printf("\n   Player 1: Paddle 1   ");//player 1
    printf("\nKeyboard w ........ move up");//player 1
    printf("\nKeyboard s ...... move down");//player 1
    printf("\n   Player 2: Paddle 2   ");//player 2
    printf("\nKeyboard i ........ move up");//player 2
    printf("\nKeyboard k ...... move down");//player 2
    printf("\n---------------------------");
    printf("\nTo run the game, make sure:");
    printf("\n*UART terminal is activated");
    printf("\n*UART baud rate is 19200bps");
    printf("\n*Keyboard is in lower case");
    printf("\n---------------------------");
    printf("\nPress any key to start\n");

	while(KBHIT()==0); //wait till keyboard press is detected
		
	NVIC_EnableIRQ(Timer_IRQn);//start timing
	NVIC_EnableIRQ(UART_IRQn);	
}

void Game_Close(void)
{
	clear_screen();
	
	// Reset scores to 0
	score1 = 0;
	score2 = 0;
	
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");//flush screen
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	
	NVIC_DisableIRQ(Timer_IRQn);			
	NVIC_DisableIRQ(UART_IRQn);	
}

//Generate a random target using system tick as food
void target_gen(void)
{
	target.x = (char)random(left_boundary+boundary_thick+1,right_boundary-2);
	target.x = target.x-target.x%2;
	delay(111*target.x);
	target.y = (char)random(top_boundary+boundary_thick+1,bottom_boundary-2);
	target.y = target.y-target.y%2;
	target.reach=0;	
}
	
int GameOver(void)
{
	char key;
	
	NVIC_DisableIRQ(UART_IRQn);
	NVIC_DisableIRQ(Timer_IRQn);
	
	printf("\nGAME OVER !!!\n");
	printf("\nPress 'q' to quit");
	printf("\nPress 'r' to replay\n");
	
	while(1)
		{
			while(KBHIT()==0);
		
			key = UartGetc();
			
			if (key == RESET)
				return 1;
			
			else if (key == QUIT)
				return 0;
			
			else
				printf("\nInvalid Input! Try Again!");
		}		
}


//---------------------------------------------
// UART ISR -- used to input commands
//---------------------------------------------
void UART_ISR(void)
{	
	char key = 0;
	int paddle1_has_moved = 0;
	int paddle2_has_moved = 0;
	int paddle1_dir = 0;
	int paddle2_dir = 0;

	key = UartGetc();	
	
	
	if (paddle1_has_moved == 1)
    {
        if (key == UP)
            paddle1_dir = -1; // Move paddle 1 up
        else if (key == DOWN)
            paddle1_dir = 1; // Move paddle 1 down
    }
		
	if (paddle2_has_moved == 1)
    {
        if (key == UP2)
            paddle2_dir = -1; // Move paddle 1 up
        else if (key == DOWN2)
            paddle2_dir = 1; // Move paddle 1 down
    }

	if(key==PAUSE)
	{
		if(pause==0)
		{
			pause=1;
			NVIC_DisableIRQ(Timer_IRQn);	
		}
		else
		{
			pause =0;
			NVIC_EnableIRQ(Timer_IRQn);
			NVIC_EnableIRQ(Timer_IRQn);
				
		}
	}
		
	paddle1_has_moved=0;
	paddle2_has_moved=0;		
}
 

//---------------------------------------------
// TIMER ISR -- used to move the snake
//---------------------------------------------


void Timer_ISR(void)
{
	

	//Display the total distance that the snake has moved
	Display_Int_Times();
		
	//Clear timer irq
	timer_irq_clear();
}	

//---------------------------------------------
// Main Function
//---------------------------------------------
int main(void){

	//Initialise the system
	SoC_init();
	//Initialise the game
	Game_Init();
	
	//Go to sleep mode and wait for interrupts
	while(1)
		__WFI();	
}





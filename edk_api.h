//--------------------------------------------------------
// Application Programming Interface (API) header file
//--------------------------------------------------------


//--------------------------------------------------------
// User parameter define
//--------------------------------------------------------

#define System_Tick_Frequency 					50000000
#define Timer_Prescaler 								16							//Timer Prescaler, options are: 16, 1 
#define Timer_Load_Value_For_One_Sec 								(System_Tick_Frequency/Timer_Prescaler)

//--------------------------------------------------------
// Constant parameters
//--------------------------------------------------------

//Image region size
#define DISPLAY_WIDTH 100
#define DISPLAY_HEIGHT 120

//VGA color
#define RED 	0xE0
#define GREEN 0x1C
#define BLUE 0x03
#define WHITE 0xFF
#define BLACK 0x00

//Command from the keyboard
#define DOWN 0x73
#define UP 0x77

//Command from the keyboard for Paddle 2
#define DOWN2 0x6A
#define UP2 0x69

//Common command from keyboard
#define RESET 0x72
#define QUIT 0x71
#define PAUSE 0x20

//--------------------------------------------------------
// API functions declaration
//--------------------------------------------------------

void SoC_init(void);					//SoC initialization

void Display_Int_2_scores ( int score1, int score2);					//Display score using 7-segment - digit 1-2 for one player - digit 3-4 for another player

void Display_Int_Times(void);				//Display the number of interrupts on the 7-segment display

void rectangle(int x1,int y1,int x2,int y2, int color);		// Draw a rectangle on the screen

void clear_screen(void);			// Clean up the screen

int read_switch(void);				// Read 8-bits from 8-bits switches

void write_LED (int data);		// Write 8-bits to 8-bits LEDs

void delay(int value);				// Delay

char random (char min, char max);	// A simple random generator using system tick

int KBHIT(void);							//Wait for keyboard hit

extern unsigned char UartPutc(unsigned char my_ch);		//Retarget functions

extern unsigned char VGAPutc(unsigned char my_ch);		//Retarget functions

extern unsigned char UartGetc(void);									//Retarget functions
 
//--------------------------------------------------------
// API global variables
//--------------------------------------------------------
 
static char dig1,dig2,dig3,dig4;


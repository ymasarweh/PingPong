//--------------------------------------------------------
// Application demonstrator: PONG game
//--------------------------------------------------------

#include "EDK_CM0.h" 
#include "core_cm0.h"
#include "edk_driver.h"
#include "edk_api.h"
#include <stdio.h>

// Game region
#define left_boundary 4
#define right_boundary 96
#define top_boundary 5
#define bottom_boundary 116
#define boundary_thick 1

// Paddle size and movement speed
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
static int pause;
static int gamespeed;
static int speed_table[10] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5}; // Speed table

// Structure definitions
struct Ball {
    int x;
    int y;
    int dir_x;
    int dir_y;
    int speed_x;
    int speed_y;
} ball;

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

// Function to generate the ball and launch it in a random direction
void generate_ball(void) {
    // Set the ball position to the center of the game region
    ball.x = (left_boundary + right_boundary) / 2;
    ball.y = (top_boundary + bottom_boundary) / 2;

    // Generate random direction for the ball
    ball.dir_x = (random(0, 1) == 0) ? -1 : 1;  // Randomly choose between -1 (left) and 1 (right)
    ball.dir_y = (random(0, 1) == 0) ? -1 : 1;  // Randomly choose between -1 (up) and 1 (down)

    // Set the ball speed
    ball.speed_x = ball_speed;
    ball.speed_y = ball_speed;
}

void draw_paddle(int x, int y, int height, int width, int color) {
    rectangle(x, y, x + width, y + height, color);
}

void draw_ball(int x, int y, int size, int color) {
    rectangle(x, y, x + size, y + size, color);
}

void draw_score(int score1, int score2) {
    printf("\nPlayer 1: %d\n", score1);
    printf("Player 2: %d\n", score2);
		Display_Int_2_scores(score1,score2);
}

void Game_Init(void)
{
    // Draw a game region
    clear_screen();
    
    rectangle(left_boundary, top_boundary, right_boundary, top_boundary + boundary_thick, BLUE); // draws top boundary
    rectangle(left_boundary, top_boundary, left_boundary + boundary_thick, bottom_boundary, BLUE); // draws left boundary
    rectangle(left_boundary, bottom_boundary, right_boundary, bottom_boundary + boundary_thick, BLUE); // draws bottom boundary
    rectangle(right_boundary, top_boundary, right_boundary + boundary_thick, bottom_boundary + boundary_thick, BLUE); // draws right boundary

    // Initialize data    
    score1 = 0;
    score2 = 0;
    paddle1_pos = (bottom_boundary - top_boundary) / 2;
    paddle2_pos = (bottom_boundary - top_boundary) / 2;
    gamespeed = 6;
  
    // Initialize timer (load value, prescaler value, mode value)
    timer_init((Timer_Load_Value_For_One_Sec / gamespeed), Timer_Prescaler, 1);
    timer_enable();
 
    // By default, set pause to 0
    pause = 0;

    // Print instructions on text console of VGA
    printf("\n------- Pong Game --------");
    printf("\nCentre btn ..... hard reset");
    printf("\nKeyboard r ..... soft reset");
    printf("\n   Player 1: Paddle 1   ");//player 1
    printf("\nKeyboard w ........ move up");//player 1
    printf("\nKeyboard s ...... move down");//player 1
    printf("\n   Player 2: Paddle 2   ");//player 2
    printf("\nKeyboard i ........ move up");//player 2
    printf("\nKeyboard j ...... move down");//player 2
    printf("\n---------------------------");
    printf("\nTo run the game, make sure:");
    printf("\n*UART terminal is activated");
    printf("\n*UART baud rate is 19200bps");
    printf("\n*Keyboard is in lower case");
    printf("\n---------------------------");
    printf("\nPress any key to start\n");

    while (KBHIT() == 0); //wait till keyboard press is detected

    NVIC_EnableIRQ(Timer_IRQn); //start timing
    NVIC_EnableIRQ(UART_IRQn);

    // Call the generate_ball() function to generate and launch the ball
    generate_ball();
    
    // Draw initial game state
    draw_paddle(left_boundary + boundary_thick, paddle1_pos, paddle_height, paddle_width, GREEN); // Adjusted left paddle position
    draw_paddle(right_boundary - paddle_width, paddle2_pos, paddle_height, paddle_width, GREEN); // Adjusted right paddle position
    draw_ball(ball.x, ball.y, ball_size, RED);
    
    // Draw initial score
    draw_score(score1, score2);
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
        while(KBHIT() == 0);
        
        key = UartGetc();
        
        if (key == 'r')
            return 1;
        
        else if (key == 'q')
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
    key = UartGetc();    

    if (key == 'w' && paddle1_pos > top_boundary + boundary_thick)
    {
        // Erase previous paddle
        draw_paddle(left_boundary + boundary_thick, paddle1_pos, paddle_height, paddle_width, BLACK); // Adjusted left paddle position
        paddle1_pos -= paddle_speed;
        // Draw new paddle
        draw_paddle(left_boundary + boundary_thick, paddle1_pos, paddle_height, paddle_width, GREEN); // Adjusted left paddle position
    }
    else if (key == 's' && paddle1_pos < bottom_boundary - paddle_height - boundary_thick)
    {
        // Erase previous paddle
        draw_paddle(left_boundary + boundary_thick, paddle1_pos, paddle_height, paddle_width, BLACK); // Adjusted left paddle position
        paddle1_pos += paddle_speed;
        // Draw new paddle
        draw_paddle(left_boundary + boundary_thick, paddle1_pos, paddle_height, paddle_width, GREEN); // Adjusted left paddle position
    }
    else if (key == 'i' && paddle2_pos > top_boundary + boundary_thick)
    {
        // Erase previous paddle
        draw_paddle(right_boundary - paddle_width, paddle2_pos, paddle_height, paddle_width, BLACK); // Adjusted right paddle position
        paddle2_pos -= paddle_speed;
        // Draw new paddle
        draw_paddle(right_boundary - paddle_width, paddle2_pos, paddle_height, paddle_width, GREEN); // Adjusted right paddle position
    }
    else if (key == 'j' && paddle2_pos < bottom_boundary - paddle_height - boundary_thick)
    {
        // Erase previous paddle
        draw_paddle(right_boundary - paddle_width, paddle2_pos, paddle_height, paddle_width, BLACK); // Adjusted right paddle position
        paddle2_pos += paddle_speed;
        // Draw new paddle
        draw_paddle(right_boundary - paddle_width, paddle2_pos, paddle_height, paddle_width, GREEN); // Adjusted right paddle position
    }
    else if (key == 'p')
    {
        if (pause == 0)
        {
            pause = 1;
            NVIC_DisableIRQ(Timer_IRQn);    
        }
        else
        {
            pause = 0;
            NVIC_EnableIRQ(Timer_IRQn);                
        }
    }
}

//---------------------------------------------
// TIMER ISR -- used to move the ball
//---------------------------------------------
void Timer_ISR(void)
{
    static int hit_count = 0; // Counter to track the number of paddle hits

    // Erase previous ball
    draw_ball(ball.x, ball.y, ball_size, BLACK);
    
    // Move ball
    ball.x += ball.dir_x * ball.speed_x;
    ball.y += ball.dir_y * ball.speed_y;

    // Collision detection with boundaries
    if (ball.y <= top_boundary + boundary_thick || ball.y >= bottom_boundary - boundary_thick)
    {
        ball.dir_y = -ball.dir_y;
    }

    // Collision detection with paddles
    else if (ball.x <= left_boundary + boundary_thick + paddle_width && ball.y >= paddle1_pos && ball.y <= paddle1_pos + paddle_height)
    {
        ball.dir_x = -ball.dir_x;

        // Increment hit count and update ball speed
        if (hit_count < 9) { // Ensure the hit count does not exceed the size of the speed table
            hit_count++;
            ball.speed_x = speed_table[hit_count];
            ball.speed_y = speed_table[hit_count];
        }
    }
    else if (ball.x >= right_boundary - paddle_width && ball.y >= paddle2_pos && ball.y <= paddle2_pos + paddle_height)
    {
        ball.dir_x = -ball.dir_x;

        // Increment hit count and update ball speed
        if (hit_count < 9) { // Ensure the hit count does not exceed the size of the speed table
            hit_count++;
            ball.speed_x = speed_table[hit_count];
            ball.speed_y = speed_table[hit_count];
        }
    }
    else if (ball.x <= left_boundary || ball.x >= right_boundary)
    {
        // Update score and reset ball
        if (ball.x <= left_boundary)
            score2++;
        else
            score1++;
        
        // Clear previous score and update it
        draw_score(score1, score2);

        // Reset hit count when a point is scored
        hit_count = 0;
        
        if (score1 == 10 || score2 == 10) {
            if (score1 == 10)
                printf("Player 1 wins!\n");
            else
                printf("Player 2 wins!\n");
            if (GameOver()) {
                Game_Init();
            } else {
                Game_Close();
                while (1);
            }
        } else {
            generate_ball();
        }
    }

    // Draw new ball
    draw_ball(ball.x, ball.y, ball_size, RED);

    // Clear timer irq
    timer_irq_clear();
}

//---------------------------------------------
// Main Function
//---------------------------------------------
int main(void)
{
    // Initialise the system
    SoC_init();
    // Initialise the game
    Game_Init();
    
    // Go to sleep mode and wait for interrupts
    while(1)
        __WFI();    
}

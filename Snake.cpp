#define UNICODE

#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>

int screen_width = 120;
int screen_height = 30; 
int FoodX = rand() % screen_width;
int FoodY = rand() % screen_height; 

using namespace std; 
// A struct allows you to group more than one variable to represent an object. In this instance the object is the snake. 
struct Snakesegments
{
    int x; // Stores x position of the snake
    int y; //; Stores y position of the snake
};

int main()
{
    // Initialise the screen as a wide char vairable. Allows storage of 65536 UNICODE values whereas char only allows 256 characters corresponding to ISO Latin tables. 
    // UNICODE is an interantional encoding standard for use with different languages.
    wchar_t *screen = new wchar_t[screen_width*screen_height];
    // Print empty space to the screen console for the desired screen height and width 
    for (int i = 0; i < screen_width*screen_height; i++) screen[i] = L' ';
    // Sets hconsole as the handle. CreateConsoleScreenBuffer creates the handles. CreateConsoleScreenBuffer( Desired Access, Share Mode (can be zero), Security Attributes, Type of Buffer to Create, Reserved - Should be NULL)
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	// Sets hconsole as the active screen buffer
    SetConsoleActiveScreenBuffer(hConsole);
    // Don't fully understand this inclusion. Think it's the number of bytes expected to be written to screen, but not sure why this is zero. 
	DWORD dwBytesWritten = 0;

    
  

    while(1)
    { // When bDead is true, this loop prevents the app from exiting the game loop. Same as having a menu loop
    // Game Loop (Always follows format: Timing and User Input, Game Logic, Display to the user)
    // Store each segment of the snakes body in a list. Lists are sequence containers that allow constant time insert and erase operations anywhere within the sequence, and iteration in both directions.
    // {{}} gives default starting position of the snake
    std::list<Snakesegments> snake = {{60,15}, {61,15}, {62,15}, {63,15}, {64,15}, {65,15}, {66,15}, {67,15}, {68,15}, {69,15}, {70,15}}; 

    // Create Game-State Variables
    
           // initialise x and y variables for food position
    int Score = 0;           // Score Variable
    int SnakeDirection = 3;  // 0 = North, 1 = East, 2 = South, 3 = West. Snake begins facing west
    bool bDead = false;      // End of game condition
    bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;
    while(!bDead)
    {

        // Timing & User Input
        //Sleep(150);

        auto t1 = chrono::system_clock::now();
		while ((chrono::system_clock::now() - t1) < ((SnakeDirection % 2 == 1) ? 80ms : 200ms))
		{ 

            bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;                    // If key if pressed, bkeyleft does not equal zero. ie - true
            bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;                   // If key is pressed, bkeyright is true. 

            if(bKeyLeft && !bKeyLeftOld)                                                             // Using bool operator for the old input can prevent latching, thus the snake doesn't coil when the user holds the key
            {
               SnakeDirection--;   
                if(SnakeDirection == -1) SnakeDirection = 3;                                         // Ensures the 0 - 4 NESW coordinate system provides a continuous circle
            }

            if(bKeyRight && !bKeyRightOld)
            {
                SnakeDirection++; 
                if(SnakeDirection == 4) SnakeDirection = 0; 
            }

            bKeyRightOld = bKeyRight;
	        bKeyLeftOld = bKeyLeft;
        }

        




        // Game Logic
        
        // Update Snake Position, place a new head at the front of the list the right direction
			switch (SnakeDirection)
			{
			case 0: // UP
				snake.push_front({ snake.front().x, snake.front().y - 1 });         // Creates a new snake head in the desired direction
				break;
			case 1: // RIGHT
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2: // DOWN
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3: // LEFT
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			}
            snake.pop_back();                                                        // Take the end off the snake each iteration, as each iteration the direction switch adds an element onto the snake



            // Collision Detection with console boundaries
            if (snake.front().x < 0 || snake.front().x >= screen_width) bDead = true;           // If snake goes beyond the screen, it's dead
            if (snake.front().y < 3 || snake.front().y >= screen_height) bDead = true;          // Same here, but y < 3 due to the banner occupying top screen position

            // Collision Detection with food
            if (snake.front().x == FoodX && snake.front().y == FoodY)                         // If snake coordinates == food coordinates...
			{
				Score++;                                                                       // Then add 1 onto the score
				while (screen[FoodY * screen_width + FoodX] != L' ')                          // Then generate new food. Keep generating new food while the food is Not in empty space....
				{                                                                               // ie - if the new food coordinates are in the snake, pick a new location
					FoodX = rand() % screen_width;
					FoodY = (rand() % (screen_height-3))+3;
				}

                
				for (int i = 0; i < 5; i++)                                                     // Push back the body of the snake 5 times. 
					snake.push_back({ snake.back().x, snake.back().y });
			}
            
            // Collision Detection with itself
			for (std::list<Snakesegments>::iterator i = snake.begin(); i != snake.end(); i++)        // I assume this iterator is the way to work through a linked list
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)   // All snake segments are checked with the snakes head. If they are equal, bDead = true
					bDead = true;                                                               // The snakes head is not checked for collision with the snakes head, as this will end game instantly. 



        // Display
        for (int i = 0; i < screen_width*screen_height; i++) screen[i] = L' ';                  // clear the screen 

        for (int i = 0; i < screen_width; i++)                                                  // Draw a banner at the top of the screen
        {
            screen[i] = L'=';    
            screen[2 * screen_width + i] = L'='; 
        }
        
        wsprintf(&screen[screen_width + 5], L"Created with help from Javidx                          SNAKE                                          Score = %d",  Score);      // Display information at the top of the screen

        // Auto deduces the type of the intialiser (which is a list) from the snake input. Therefore, s is a counter that is in itself a list that allows auto iteration across the snake's body
        for (auto s: snake) screen[s.y * screen_width + s.x] = bDead? L'+' : L'0';          // if the snake is dead, print + where the snake's body is. Otherwise print - indicating the snake is alive.     

        // draw snakes head
        screen[snake.front().y * screen_width + snake.front().x] = bDead? L'X' : L'@';      // at the front of the list, print X if dead, @ if not            

        // draw food 
        screen[FoodY * screen_width + FoodX] = L'%'; 
       


        if (bDead)
		wsprintf(&screen[15 * screen_width + 40], L"GAME OVER:  PRESS 'SPACE' TO PLAY AGAIN    ");
        // WriteConsoleOutputCharacter displays the frame to the user. WriteConsoleOutputCharacter( Handle, Characters to be written to the screen, no. characters to be written, coordinate to begin writing to screen, pointer variable that receives the characters actually written)
        WriteConsoleOutputCharacter(hConsole, screen, screen_width*screen_height, {0,0}, &dwBytesWritten);
    }
    //Holds up the program until the space bar is pressed
    while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
    }
     







    return 0; 
}
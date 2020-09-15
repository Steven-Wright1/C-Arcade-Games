#define UNICODE 

#include <iostream>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <ctime>
#include <fstream>
#include <conio.h>
#include <cmath>

int nScreenWidth = 120;			    // Console Screen Size X (columns)
int nScreenHeight = 30;			    // Console Screen Size Y (rows)
std:: wstring tetromino[7];         // Tetromino Storage
int nFieldWidth = 12;               // Playing Field Size X (columns)
int nFieldHeight = 18;              // Playing Field Size Y (rows)
unsigned char *pField = nullptr;    // Dynamically Store Playing Field Array - Unsigned Chars so 0 = empty space, 1 = tetromino piece etc. 
int nSpeed = 20;                    // Tetromino Speed 
int best_score;                     // High Score
bool Pause = true;                  // Pause Switch 
int Piece;                   // A    B    C     D    E    F      ELSE 
static const double case0[] = {0.4, 0.6, 0.8, 0.85, 0.9, 0.95};                     // Probability of Next Piece/Tetromino Being Pieces A, B, C, D, E, F, G  for Easy Difficulty
static const double case1[] = {0.143, 0.286, 0.429, 0.572, 0.715, 0.858};           // "" Medium Difficulty
static const double case2[] = {0.05, 0.1, 0.2, 0.4, 0.6, 0.8};                      // "" Hard Difficulty
const double *PieceProb = NULL;                                                     // Dynamically Store Probability Arrays
int nScore;                                                                         // Score Storing Variable


// Save High Score to File if nScore > (best_score)
void save_high_score(int nScore)        
{
std::ifstream input("high_score.txt");
input >> best_score; 

std::ofstream output("high_score.txt");
    if (nScore > best_score)
        {
        output << nScore; 
        }
        else 
            {
            output << best_score; 
            }
}

// Index Manipualtion for Piece Rotation
int Rotate(int px, int py, int r)
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0 degrees			// 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
									//12 13 14 15

	case 1: // 90 degrees			//12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
									//15 11  7  3

	case 2: // 180 degrees			//15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
									// 3  2  1  0

	case 3: // 270 degrees			// 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}

// Collision Testing 
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	// Iterate Across Piece
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into Piece
			int pi = Rotate(px, py, nRotation);

			// Get Index into Field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			// Check That Test is in Bounds of Playing Field. 
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					// In Bounds so do Collision Check
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0) 
						return false; // Fail on First Hit
				}
			}
		}

	return true;
}

int main()
{
    // Seed Time-Dependant Psuedo-Random Number for 'Random' Piece Selection
    srand(time(NULL));


    int Menu_Selection;     // Store Menu Selection
    int Difficulty;         // Store Difficulty Selection
    

    // Print a TETRIS Banner to the User
    std::cout << "#######      #######      #######      ###         ##      ###### " << std::endl; 
    std::cout << "   #         #               #         #  #        ##      ##     " << std::endl; 
    std::cout << "   #         #               #         #  #                 ###   " << std::endl; 
    std::cout << "   #         #######         #         ###         ##         ### " << std::endl; 
    std::cout << "   #         #               #         #  #        ##          ###" <<  std::endl; 
    std::cout << "   #         #               #         #   #       ##           ##" <<  std::endl; 
    std::cout << "   #         #######         #         #    #      ##      #######" <<  std::endl; 

    // Display Instructions
    std::cout << "Instructions" << std::endl << "Arrow Keys - Move Piece" << std::endl << "Z - Rotate Piece" << std::endl << "P - Pause Game" << std::endl << "R - Restart Game" << std::endl; 
   
    // Unless User Selects Quit, Continue to Print Menu After Game
    do {
    
    std::cout << "Press 1 to begin a new game. Press 0 to quit \n";
    std::cin >> Menu_Selection;
    
        // Present Menu
        switch(Menu_Selection) 
        {

        default: 
        std::cout << "Please press either 1 or 0."; 
        break; 

        case 0: 
        std::cout << "See you soon!";
        return 0;

        case 1: 
        // Present Difficult Options if User Wants to Play
        std::cout << "Please Select Difficulty" << std::endl << "1. Easy" << std::endl << "2. Medium" << std::endl << "3. Hard" << std::endl;
        std::cin >> Difficulty; 
            
            // Initialise Relevant Tetromino Piece Selection Probabilities
            switch(Difficulty) 
            {
                case 1: 
                PieceProb = case0;
                break; 

                case 2:
                PieceProb = case1; 
                break; 
        
                case 3:
                PieceProb = case2; 
                break;
            }
 
        // Create Screen Buffer
	    wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	    for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
	    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	    SetConsoleActiveScreenBuffer(hConsole);
	    DWORD dwBytesWritten = 0;

        // Create Tetromino Pieces.
	    tetromino[0].append(L"..X...X...X...X."); // A
	    tetromino[1].append(L"..X..XX...X....."); // B
	    tetromino[2].append(L".....XX..XX....."); // C
	    tetromino[3].append(L"..X..XX..X......"); // D
	    tetromino[4].append(L".X...XX...X....."); // E
	    tetromino[5].append(L".X...X...XX....."); // F
	    tetromino[6].append(L"..X...X..XX....."); // G
        tetromino[7].append(L"................"); // 'Empty' Field to Display Next Piece On Top

        // Create Play Field Buffer
	    pField = new unsigned char[nFieldWidth*nFieldHeight]; 
	    for (int x = 0; x < nFieldWidth; x++)     // Board Boundary
		    for (int y = 0; y < nFieldHeight; y++)
			    pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0; // If Board Boundary, Assign 9. Otherwise, Assign 0. 

    


	    bool bKey[5];                               // Store Keyboard Input in a True/False Array
        int nCurrentPiece = rand() % 7;             // 'Randomise' Current Piece
        int nNextPiece = rand() % 7;                // 'Randomise' Next Piece
	    int nCurrentRotation = 0;                   // Initial Rotation = 0
	    int nCurrentX = nFieldWidth / 2;            // Piece Initial X POS is Halfway Across Pfield
	    int nCurrentY = 0;                          // Piece Initial Y POS is Top of Pfield        
	    int nSpeedCount = 0;                        // Count Game-Loop Iterations
	    bool bForceDown = false;                    // Downwards Forcing Variable
	    bool bRotateHold = true;                    // Flag if User is Holding Down Rotate Button (Z)   
	    int nPieceCount = 1;                        // Piece Counter
	    std::vector<int> vlines;                    // Storage for Lines Created in Game
	    bool bGameOver = false;                     // Flag for Game Over
        int Level = 0;                              // Level Storage Variable

            // Main Game Loop
	        while (!bGameOver) 
	        {
            save_high_score(nScore);

            //Apply Difficulty Probabilities to Piece Selection
            double i = (rand()%100)/100.00;
		    if     (i <= PieceProb[0])  {Piece = 0;}
		    else if(i <= PieceProb[1])  {Piece = 1;}
		    else if(i <= PieceProb[2])  {Piece = 2;}
		    else if(i <= PieceProb[3])  {Piece = 3;}
            else if(i <= PieceProb[4])  {Piece = 4;}
            else if(i <= PieceProb[5])  {Piece = 5;}
		    else                        {Piece = 6;}
       
            // Resets Next Piece Visualisation Array
		    for (int px = 0; px < 4; px++)
			    for (int py = 0; py < 4; py++)
                    if (tetromino[7][Rotate(px, py, nCurrentRotation)] != L'X')     
					screen[(py + 9)*nScreenWidth + (px + 50)] = '.';


		    // Game Timing =======================
		    Sleep(50);                               // Small Step = 1 Game Tick
		    nSpeedCount++;                           // Increase Speed Counter With Each Iteration
		    bForceDown = (nSpeedCount == nSpeed);    // Force the Piece Down if the Speed Counter = Current Speed
  
		    //User Input ========================
            // Store Allowed Keyboard Responses in Boolean bKey Array
		    for (int k = 0; k < 6; k++)								// R   L   D  ZPR
			    bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28ZPR"[k]))) != 0;
		
		    // Handle player movement (If Movement Key is Pressed and the Piece Fits Given the Movement, Translate the Piece)
		    nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;      
		    nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;		
		    nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
		
            // Rotate Piece if Z is Pressed. Only Perform 1 Rotation
		    if (bKey[3])
		    {
			    nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			    bRotateHold = false;
		    }
		        else
		        {
			        bRotateHold = true;
		        }

            //If P is Pressed, Pause Switch is Activated. Set Game Speed to 0 and Cancel Out User Inputs 
            if (bKey[4]) Pause = !Pause;
                if (Pause == false) 
                {
                    nSpeedCount = 0; 
                    nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? -1 : 0;
		            nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? -1 : 0;		
		            nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? -1 : 0; 
                }
        
    
		        // If the Piece is Forced Down, Conduct These Checks 
		        if (bForceDown)
		        {

                int i = 0; 
                // Increase Game Speed Every 10 Pieces. Game Speed Increase is Proportional to Difficulty
                switch(nPieceCount)
                {
                    case 1 : nSpeed = 20; 
                    break;
                    case 10: nSpeed = 20 - Difficulty; 
                    break;
                    case 20: nSpeed = 19 - Difficulty;
                    break; 
                    case 30: nSpeed = 17 - Difficulty;
                    break; 
                    case 40: nSpeed = 16 - Difficulty;
                    break; 
                    case 50: nSpeed = 15 - Difficulty;
                    break; 
                    case 60: nSpeed = 14 - Difficulty;
                    break; 
                    case 70: nSpeed = 13 - Difficulty;
                    break; 
                    case 80: nSpeed = 12 - Difficulty;
                    break; 
                    case 90: nSpeed = 11 - Difficulty;
                    break; 
                    case 100: nSpeed = 10 - Difficulty;
                    break; 
                    case 110: nSpeed = 9 - Difficulty;
                    break; 
                    case 120: nSpeed = 8 - Difficulty;
                    break;
                }

            
		        // Test if the Piece Can be Moved Down
			    if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				    nCurrentY++;                            // If so, Move the Piece Down
			    else
			    {
				// Otherwise Lock the Piece in Place, Test for Lines, Output the Score Update and Select the New Piece
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                //Check for Lines
                //Check the rows of the tetromino
			    for (int py = 0; py < 4; py++)
                //Translate the Piece into the Field Array
                if (nCurrentY + py < nFieldHeight - 1)
                {
                    bool bline = true; 
                    for (int px = 1; px < nFieldWidth - 1; px++)
                    bline &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                    if (bline)
						{
							// Remove Line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
                            //Where a line exists, set the blocks to an = sign
							pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            //If a line exists, push back the current row
                            vlines.push_back(nCurrentY + py);
						}	
                
                }

                // Provide Difficult Dependant Score Updates. Multiple Lines Gain Higher Bonuses
                if (Difficulty == 1)
                 {
                    nScore += (21 - nSpeed) * 25; 
                    if(!vlines.empty())	nScore += (1 << vlines.size()) * 50;
                 }
                    else if (Difficulty == 2) 
                    {
                        nScore += (21 - nSpeed) * 50;
                        if(!vlines.empty())	nScore += (1 << vlines.size()) * 100;
                    }
                        else if (Difficulty == 3) 
                        {
                            nScore += (21 - nSpeed) * 100;
                            if(!vlines.empty())	nScore += (1 << vlines.size()) * 150;
                        }
            
                // Update the Piece Count
                nPieceCount++;
                // Calculate the Level
				Level = ceil(nPieceCount/10.0); 
                 

				
				// Pick New Piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = nNextPiece;
                nNextPiece = Piece;
				
				// If Piece Does Not Fit Straight Away, Game Over!
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
            
            // Reset Speed Counter
             nSpeedCount = 0;
            
		}
       
		// Display ======================

	    //Draw Tetris Title Page on Screen Buffer. Also Output the Score, Piece Count, Level and High Score
        swprintf (screen, L"#######      #######      #######      ###         ##      ######                                                          #         #               #         #  #        ##      ##                                                              #         #               #         #  #                 ###                                                            #         #######         #         ###         ##         ###                                                          #         #               #         #  #        ##          ###                                                         #         #               #         #   #       ##           ##                                                         #         #######         #         #    #      ##      #######                                                       Score: %d     piece count: %d     Level: %d                                          High Score: %d                                                                    Next Piece:" , nScore, nPieceCount, Level, best_score);
    
       
                                                
		// Draw the Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 9)*nScreenWidth + (x + 25)] = L" ABCDEFG=["[pField[y*nFieldWidth + x]];


        // If User Presses the R Key, Reset the Score, Game Speed, Piece Count and the Playing Field
        if (bKey[5])
        {
            nScore = 0;     
            nSpeed = 20;
            nPieceCount = 0; 
            for (int x = 1; x < nFieldWidth-1; x++)
			    for (int y = 1; y < nFieldHeight-1; y++)
                {
			pField[y*nFieldWidth + x] = 0;
            screen[(y + 9)*nScreenWidth + (x + 25)] = L" ABCDEFG=["[pField[y*nFieldWidth + x]];
                }
        }

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 9)*nScreenWidth + (nCurrentX + px + 25)] = nCurrentPiece + 65;
	
        // Draw Next Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
            if (tetromino[nNextPiece][Rotate(px, py, 0)] == L'X')
					screen[(py + 9)*nScreenWidth + (px + 50)] = nNextPiece + 65;


        //Animate piece deletion 
        if(!vlines.empty())
        {
            // Display Frame 
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			// Add in a small delay to show the user the = symbols inferring line deletion
            Sleep(400); 

                for (auto &v : vlines)
				    for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}

			vlines.clear();
        }
		
		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}


    // Close Screen Buffer Once Game Over
	CloseHandle(hConsole);

    // Display on Console to User the Score, and Skill Level
	std::cout << "Game Over!! Score:" << nScore << std::endl;
    if(nScore <= 24000)                    std::cout << "Better Luck Next Time. Skill Level: Novice!" << '\n';
    if(nScore > 24000 && nScore <= 40000 ) std::cout << "Good Job! Skill Level: Amateur" << '\n';
    if(nScore > 40000 && nScore <= 75000 ) std::cout << "Well Done! Skill Level: " << '\n';
    if(nScore > 75000)                     std::cout << "Congratulations! You're a pro!";
    }

    

       } while (Menu_Selection != 0);
    
	
	return 0;

}

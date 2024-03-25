#include "FEHLCD.h"
#define YMAX 239
#include "FEHLCD.h"
#include "FEHRandom.h"

#define PIECE 0
#define CROWN 1
#define CPU 2
#define PLAYER 3
#define TRASH 4



void DitherRect(int, int, int, int, int, int) ;
void DrawSprite(int, int, int, int) ;



/* One instance of the Piece class is used for each checkers piece.
                                        Author: Ezekiel Wessendarp */
class Piece {
    public :
        /* Constructor takes coordinates and team affiliation inputs
                                        Author: Ezekiel Wessendarp */
        Piece(int x = -1, int y = -1, int affiliation = -1) : piece_x(x), piece_y(y), color(affiliation) {}
        void DisplayPiece(void) ;



        // Getter functions to return private member variables
        
        int GetX() {return piece_x ;}   // Author:
        int GetY() {return piece_y ;}   // Ezekiel
        int GetColor() {return color ;} // Wessendarp
        int GetKing() {return king ;}   // ^



        // Setter functions to modify private member variables

        /* This probably sucks and should probably be deleted at the end, but I've kept it in case we find a
        use. It moves a Piece's private coordinate variables by intervals of 30 pixels. It is called in my
        TestingLogic() function, so don't compile without deleting that call.
                                                                                Author: Ezekiel Wessendarp */
        void MoveBySquareOffset(int x_offset, int y_offset) {
            piece_x += 30 * x_offset ;
            piece_y += 30 * y_offset ;
        }
        /* This function is just a setter for the private coordinate variables.
                                                    Author: Ezekiel Wessendarp */
        void MoveToCoord(int x, int y) {
            piece_x = x ;
            piece_y = y ;
        }
        /* Just a one-way setter to king a Piece. One-way because you shouldn't need to un-king.
                                                                    Author: Ezekiel Wessendarp */
        void King(void) {king = true ;}



    private :
        // X and Y coordinates of the board square for the Piece
        int piece_x, piece_y ;
        // Boolean value to describe if Piece is kinged
        bool king = false ;
        // Color/team affiliation
        int color ;
} ;

/* Function to display an individual Piece. Board::DisplayBoard() heavily depends on this function.
                                                                        Author: Ezekiel Wessendarp */
void Piece::DisplayPiece(void) {
    // Set Piece color based on its affiliation
    LCD.SetDrawColor(color) ;

    // Only display a Piece that has acceptable coordinates
    if (piece_x >= 0 && piece_y >= 0) {
        DrawSprite(piece_x + 3, piece_y + 3, PIECE, color) ;
        /*LCD.FillCircle(piece_x + 15, piece_y + 15, 12) ;
        LCD.SetDrawColor(GRAY) ;
        LCD.DrawCircle(piece_x + 15, piece_y + 15, 12) ; // The "+ 15"s are here because the circle draws at
        LCD.DrawCircle(piece_x + 15, piece_y + 15, 8) ;  // the center of the square and not the top left */

        // Draw a crown on kinged pieces
        if (king == true) {
                DrawSprite(piece_x + 10, piece_y + 12, CROWN, GRAY) ;
        }
    }

    
}

/* The board handles interactions between Pieces and tracks their locations.
                                                Author: Ezekiel Wessendarp */
class Board {
    public :
        // Constructor takes starting x position for board
        Board(int offset) : x_pos(offset) { }
        void DisplayBoard(void) ;
        void DisplaySquare(Piece *) ;
        Piece* SelectPiece() ;
        void SelectDest(Piece *) ;

        // Sidebar functions
        void DisplaySidebar() ;
        void RefreshSidebar() ;

        // Getter functions to return private member variables

        // Returns the board's x offset
        int GetPos() {return x_pos ;} // Author: Ezekiel Wessendarp
        int GetSidebarPos() {return sidebar_x ;} // ^
        // Returns the pointer to the piece at r, c
        Piece* GetBoardState(int r, int c) {return boardState[r][c] ;} // Author: Ezekiel Wessendarp

        // Setter functions to modify private member variables

        // Changes one value in the Piece pointer array
        void SetBoardState(int r, int c, Piece *piece) {boardState[r][c] = piece ;} // Author: Ezekiel Wessendarp
        void SetTurn() {p1Turn = !p1Turn ;}



    private :
        /*        x_pos
           =  0 for left side
           = 40 for the middle
           = 80 for right side
        */
        int x_pos ;
        // This is the x-value for the left side of the sidebar, initialized by Board::DisplaySidebar()
        int sidebar_x ;
        // Tracks whose turn it is
        bool p1Turn = true ;
        // 2D Piece pointer array that contains pointers to every active Piece
        Piece *boardState[8][8] = {{nullptr}} ;
} ;

/* This function draws the checkers board and then calls the DisplayPiece function for every active Piece.
                                                                                Author: Ezekiel Wessendarp */
void Board::DisplayBoard(void) {
    int x, y, r, c ;
    bool swaps = false ;

    LCD.Clear(LCD.Scarlet) ;

    // Row loop to draw the checkerboard
    for (y = 0 ; y < 239 ; y += 30) {

        // Nested column loop to draw the checkerboard
        for (x = x_pos ; x < 239 + x_pos ; x += 30) {

            // Alternate colors for each square
            if (swaps == false) {
                swaps = true ;
                LCD.SetDrawColor(WHITE) ;
            }
            else if (swaps == true) {
                swaps = false ;
                LCD.SetDrawColor(SKYBLUE) ;
            }
            



            LCD.FillRectangle(x, y, 30, 30) ;
        }

        // From the end of one row to the start of the next, the color stays the same
        swaps = !swaps ;
    }


    
    // Call Piece::DisplayPiece() for every Piece that is present in the board's Piece pointer array
    for (r = 0 ; r < 8 ; r ++) {

        for (c = 0 ; c < 8 ; c++) {

            // If statement ensures that the pointer is non-null before attempting to display the Piece
            if (boardState[r][c]) {
                boardState[r][c] -> DisplayPiece() ;
            }
        }
    }

    // Clears touch buffer in case the user has clicked/tapped while the figure was being drawn
    LCD.ClearBuffer() ;
}

// Redraws a board square where a piece is leaving from
void Board::DisplaySquare(Piece *piece) {
    LCD.SetDrawColor(SKYBLUE) ;
    LCD.FillRectangle(piece -> GetX(), piece -> GetY(), 30, 30) ;
}

// Displays the sidebar with its icons
void Board::DisplaySidebar() {
    // Sidebar is only supported for two Board positions
    switch (x_pos) {
        case 0 :
            sidebar_x = 240 ;
            break ;
        case 80 :
            sidebar_x = 0 ;
            break ;
        default :
            LCD.WriteLine("ERROR: Sidebar does not support") ;
            LCD.WriteLine("this value for x_pos") ;
    }

    // Call DitherRect() function to create a blue-gray sidebar
    DitherRect(sidebar_x, 0, 80, 240, NAVY, GRAY) ;

    // Calls DrawSprite() functions
    DrawSprite(sidebar_x + 25, 15, CPU, BLACK) ;
    DrawSprite(sidebar_x + 25, 195, PLAYER, BLACK) ;
    DrawSprite(sidebar_x + 10, 113, TRASH, GRAY) ;
}

/* This function returns the Piece pointer to an active Piece clicked on/tapped by the user. It also displays
a green box around the Piece that the user has selected.
                                                                                Author: Ezekiel Wessendarp */
Piece* Board::SelectPiece() {
    int x, y ;
    Piece *ptr ;

    // Loop until an active Piece is selected
    for(;;) {

        // Get Touch from user
        while(!LCD.Touch(&x, &y)) {}

        /* Adjust the coordinates picked by the user to be a value on the range 0–7, preparing it for the
        Piece pointer array                                                                             */
        x -= x_pos ;
        x /= 30 ;
        y /= 30 ;

        // Only executes the statements within if an active Piece has been selected
        if(x >= 0 && x < 240 && boardState[y][x]) {
            // Draws green box around Piece to give the user visual feedback
            LCD.SetDrawColor(GREEN) ;
            LCD.DrawRectangle(boardState[y][x] -> GetX(), boardState[y][x] -> GetY(), 30, 30) ;
            LCD.DrawRectangle(boardState[y][x] -> GetX() + 1, boardState[y][x] -> GetY() + 1, 28, 28) ;

            // Back up the pointer to a temporary variable
            ptr = boardState[y][x] ;
            // Clear the pointer from the Piece pointer array since it is moving
            boardState[y][x] = nullptr ;

            // Return address of Piece that is currently not stored in the Piece pointer array
            return ptr ;
        }
    }
    
}

/* In its current implementation, this function allows the user to move a Piece to any empty spot on the board.
To correct this, the function will need to call a separate function that checks the validity of a move.
                                                                                    Author: Ezekiel Wessendarp */
void Board::SelectDest(Piece *piece) {
    int x, y ;

    // Loops until an unoccupied square is selected
    for(;;) {
        while(!LCD.Touch(&x, &y)) {}
        
        /* Adjust the coordinates picked by the user to be a value on the range 0–7, preparing it for the
        Piece pointer array                                                                             */ 
        x -= x_pos ;

        /* If the coordinates are on the range of acceptable values, AND the space is empty, AND the space
        is not a white space, break this loop                                                           */
        if(x >= 0 && x < 240 && !boardState[y / 30][x / 30] && (x / 30 + y / 30) % 2 == 1)
            break ;
        
        // Create a special case if the user selects the trash icon
        else if(x + x_pos >= 10 + sidebar_x && x + x_pos <= 24 + sidebar_x && y >= 113 && y <= 126) {
            // Overwrite the old piece location with an empty square
            DisplaySquare(piece) ;
            // Assign the piece invalid coordinates so it cannot be displayed with Piece::DisplayPiece()
            piece -> MoveToCoord(-1, -1) ;
            // Exit function
            return ;
        }
        
    }

    // Overwrite the old piece location with an empty square
    DisplaySquare(piece) ;
    // Take the Piece passed to this function and change its coordinates using a member function
    piece -> MoveToCoord(x / 30 * 30 + x_pos, y / 30 * 30) ;
    // Store the Piece's address to its new location in the Piece pointer array
    boardState[y / 30][x / 30] = piece ;
    // Display the piece at its new location
    piece -> DisplayPiece() ;
    

}



//void P1Logic(Board , int , int , int , int );
//void P2Logic(Board , int , int , int , int );
void AIturn(Board);
int CheckForWinner(int opp, Board stat);


















// Acts as main function sort of by calling all the functions written by all three
void TestingLogic(int opp, int winner) {
    /* Top-left corner of the board to pass to Board constructor and use in Piece constructor calls, it can
    take any value 0–80, but Board::DisplaySidebar() only supports 0 and 80.                                */
    int x_pos = 0 , x, y, x2, y2;


    
    // Initialize a Board on the left side of the screen
    Board gameboard(x_pos) ;
    
    /* 24 constructor calls, followed by 24 SetBoardState() calls. This creates 24 Piece objects and assigns
    starting positions to them all.                                                                         */

    Piece red1(30 + x_pos, 0, RED), red2(90 + x_pos, 0, RED), red3(150 + x_pos, 0, RED), red4(210 + x_pos, 0, RED) ;
    Piece red5(x_pos, 30, RED), red6(60 + x_pos, 30, RED), red7(120 + x_pos, 30, RED), red8(180 + x_pos, 30, RED) ;
    Piece red9(30 + x_pos, 60, RED), redA(90 + x_pos, 60, RED), redB(150 + x_pos, 60, RED), redC(210 + x_pos, 60, RED) ;
    Piece black1(x_pos, 150, BLACK), black2(60 + x_pos, 150, BLACK), black3(120 + x_pos, 150, BLACK), black4(180 + x_pos, 150, BLACK) ;
    Piece black5(30 + x_pos, 180, BLACK), black6(90 + x_pos, 180, BLACK), black7(150 + x_pos, 180, BLACK), black8(210 + x_pos, 180, BLACK) ;
    Piece black9(x_pos, 210, BLACK), blackA(60 + x_pos, 210, BLACK), blackB(120 + x_pos, 210, BLACK), blackC(180 + x_pos, 210, BLACK) ;
    gameboard.SetBoardState(0, 1, &red1) ; gameboard.SetBoardState(5, 0, &black1) ;
    gameboard.SetBoardState(0, 3, &red2) ; gameboard.SetBoardState(5, 2, &black2) ;
    gameboard.SetBoardState(0, 5, &red3) ; gameboard.SetBoardState(5, 4, &black3) ;
    gameboard.SetBoardState(0, 7, &red4) ; gameboard.SetBoardState(5, 6, &black4) ;
    gameboard.SetBoardState(1, 0, &red5) ; gameboard.SetBoardState(6, 1, &black5) ;
    gameboard.SetBoardState(1, 2, &red6) ; gameboard.SetBoardState(6, 3, &black6) ;
    gameboard.SetBoardState(1, 4, &red7) ; gameboard.SetBoardState(6, 5, &black7) ;
    gameboard.SetBoardState(1, 6, &red8) ; gameboard.SetBoardState(6, 7, &black8) ;
    gameboard.SetBoardState(2, 1, &red9) ; gameboard.SetBoardState(7, 0, &black9) ;
    gameboard.SetBoardState(2, 3, &redA) ; gameboard.SetBoardState(7, 2, &blackA) ;
    gameboard.SetBoardState(2, 5, &redB) ; gameboard.SetBoardState(7, 4, &blackB) ;
    gameboard.SetBoardState(2, 7, &redC) ; gameboard.SetBoardState(7, 6, &blackC) ;

    gameboard.DisplayBoard() ;
    gameboard.DisplaySidebar() ;

    while(winner==0){
                

                
                gameboard.SelectDest(gameboard.SelectPiece()) ;
                
                winner = CheckForWinner(opp, gameboard);
                if (opp==1){
                    AIturn(gameboard);
                    gameboard.SelectDest(gameboard.SelectPiece()) ;
                    DitherRect(gameboard.GetSidebarPos() + 3, 49, 25, 55, NAVY, GRAY) ;
                    DitherRect(gameboard.GetSidebarPos() + 28, 49, 25, 55, GRAY, NAVY) ;
                    DitherRect(gameboard.GetSidebarPos() + 53, 49, 25, 55, NAVY, GRAY) ;
                }
                else {

                    gameboard.SelectDest(gameboard.SelectPiece()) ;
                }
                winner = CheckForWinner(opp, gameboard);
                
            };


}

/* Fills a rectangular space from the top left to the bottom right using a one-pixel checkerboard pattern.
This effectively blends colors and allows you to create backgrounds with more precision than just 21 colors.
                                                                                Author: Ezekiel Wessendarp */
void DitherRect(int x, int y, int w, int h, int color1, int color2) {
    int i, j ;

    // Draw a rectangle using the first color and switch to the second color
    LCD.SetDrawColor(color1) ;
    LCD.FillRectangle(x, y, w, h) ;
    LCD.SetDrawColor(color2) ;

    // Draws diagonal lines starting from the top left until it reaches the right side
    for (i = x + 1 ; i < x + w && i - x < h ; i += 2) {
    
        LCD.DrawLine(i, y, x, i - x + y) ;
    }

    // Offset is determined by the width of the rectangle
    if (w % 2 == 0)
        i = y ;
    else if (w % 2 == 1)
        i = y + 1 ;

    // Draws diagonal lines starting from the top right until it reaches the bottom left
    for (; i <= y + h - w ; i += 2) {

        LCD.DrawLine(x + w - 1, i, x, i + w - 1) ;
    }

    // Destination coord offset is determined by rectangle height
    if (h % 2 == 0)
        j = x + 2 ;
    else if (h % 2 == 1)
        j = x + 1 ;

    // Draws diagonal lines from where it left off until it reaches the bottom right
    for (; i < y + h ; i += 2) {

        LCD.DrawLine(x + w - 1, i, j, y + h - 1) ;
        j += 2 ;
    }

    // Clears touch buffer in case the user has clicked/tapped while the figure was being drawn
    LCD.ClearBuffer() ;
}

void DrawSprite(int x, int y, int sprite, int color) {
    LCD.SetDrawColor(color) ;
    
    // Draws a pre-defined sprite at the provided coordinates
    switch (sprite) {
        case PIECE :
            LCD.FillCircle(x + 12, y + 12, 12) ;
            LCD.SetDrawColor(GRAY) ;
            LCD.DrawCircle(x + 12, y + 12, 12) ; // The "+ 15"s are here because the circle draws at
            LCD.DrawCircle(x + 12, y + 12, 8) ;  // the center of the square and not the top left
            break ;
        


        // 10 x 6 crown used for kinged pieces
        case CROWN :
            LCD.FillRectangle(x, y + 2, 10, 3) ;
            LCD.FillRectangle(x + 1, y + 5, 8, 1) ;
            LCD.FillRectangle(x, y, 1, 2) ;
            LCD.FillRectangle(x + 1, y + 1, 1, 1) ;
            LCD.FillRectangle(x + 3, y + 1, 4, 1) ;
            LCD.FillRectangle(x + 4, y, 2, 1) ;
            LCD.FillRectangle(x + 8, y + 1, 2, 1) ;
            LCD.FillRectangle(x + 9, y, 1, 1) ;
            break ;
        


        // 30 x 30 CPU chip used as turn indicator
        case CPU :
            LCD.DrawRectangle(x + 3, y + 3, 24, 24) ;
            LCD.DrawRectangle(x + 4, y + 4, 22, 22) ;
            LCD.FillRectangle(x + 7, y + 7, 16, 16) ;

            LCD.FillRectangle(x, y + 7, 3, 2) ;
            LCD.FillRectangle(x, y + 14, 3, 2) ;
            LCD.FillRectangle(x, y + 21, 3, 2) ;
            LCD.FillRectangle(x + 27, y + 7, 3, 2) ;
            LCD.FillRectangle(x + 27, y + 14, 3, 2) ;
            LCD.FillRectangle(x + 27, y + 21, 3, 2) ;

            LCD.FillRectangle(x + 7, y, 2, 3) ;
            LCD.FillRectangle(x + 14, y, 2, 3) ;
            LCD.FillRectangle(x + 21, y, 2, 3) ;

            LCD.FillRectangle(x + 7, y + 27, 2, 3) ;
            LCD.FillRectangle(x + 14, y + 27, 2, 3) ;
            LCD.FillRectangle(x + 21, y + 27, 2, 3) ;
            break ;
        


        // 30 x 30 player icon used as a turn indicator
        case PLAYER :
            LCD.FillCircle(x + 14, y + 8, 9) ;
            LCD.FillRectangle(x + 6, y + 21, 18, 1) ;
            LCD.FillRectangle(x + 4, y + 22, 22, 8) ;
            break ;
        


        case TRASH :
            LCD.SetDrawColor(WHITE) ;
            LCD.FillRectangle(x + 2, y, 10, 1) ;
            LCD.FillRectangle(x + 1, y + 1, 12, 1) ;
            LCD.FillRectangle(x, y + 2, 14, 10) ;
            LCD.FillRectangle(x + 1, y + 12, 12, 1) ;
            LCD.FillRectangle(x + 2, y + 13, 10, 1) ;

            LCD.SetDrawColor(color) ;
            LCD.FillRectangle(x + 3, y + 4, 8, 8) ;

            LCD.SetDrawColor(BLACK) ;
            LCD.DrawRectangle(x + 5, y + 1, 4, 3) ;
            LCD.FillRectangle(x + 1, y + 3, 12, 1) ;
            LCD.FillRectangle(x + 2, y + 4, 1, 8) ;
            LCD.FillRectangle(x + 11, y + 4, 1, 8) ;
            LCD.FillRectangle(x + 3, y + 12, 8, 1) ;
            break ;
    }
}


//int maim written by Ariane Clerc and Garrett Herb
void DisplayResults(int opp);
int main() {
    //define variables
    int x, y, opponent,  winner=0;
    
    

    LCD.Clear();
    
    //start loop
    while (1) 
    {
        //clear the screen
        LCD.Clear();
        //draw the lines for the choice to perform a command
        LCD.SetDrawColor(LCD.Red);
        LCD.DrawHorizontalLine(60,0,320);
        LCD.DrawHorizontalLine(120,0,320);
        LCD.DrawHorizontalLine(180,0,320);
        //list out the commands
        LCD.WriteAt("Play Game", 120,30);
        LCD.WriteAt("Statistics", 122, 90);
        LCD.WriteAt("Credits", 130, 200);
        LCD.WriteAt("Instructions",115,140);
        //wait for touch
        while(!LCD.Touch(&x,&y))
        {
        }
        //if they select play game
        if (y<YMAX/4.0) 
        {
        //clear the screen and redraw the lines for the different choices
        LCD.Clear();
        LCD.SetDrawColor(LCD.Red);
        LCD.DrawHorizontalLine(60,0,320);
        LCD.DrawHorizontalLine(120,0,320);
        //have user select what type of game they want to play
        LCD.WriteAt("Player 1 vs Player 2", 122, 30);
        LCD.WriteAt("Player 1 vs AI", 130, 90);
        LCD.WriteAt("Click to return to menu", 120,150);
        while(!LCD.Touch(&x,&y))
        {
        }
            //if they select the first option, define the opponent as player 2
            if (y<YMAX/4.0) {
                opponent=2;
            }
            //if AI is selected then set the difficulty to zero and opponent to AI
            else if (y<YMAX/2.0){
                opponent=1;
                
            }
            //if button pressed to return to home screen
            else {
                break;
            }
            TestingLogic(opponent, winner);
        }
        //if statistics is selected
        else if (y<YMAX/2.0) 
        {
       LCD.Clear();
       //display the statistics
       if (winner==1) {
           LCD.WriteLine("Player 1 Wins: 1");
           LCD.WriteLine("Player 2 Wins: 0");
           LCD.WriteLine("AI Wins: 0");
            LCD.WriteAt("Click Anywhere to return to menu",55,150);
       }
       else if (winner==2) {
           LCD.WriteLine("Player 1 Wins: 0");
           LCD.WriteLine("Player 2 Wins: 1");
           LCD.WriteLine("AI Wins: 0");
            LCD.WriteAt("Click Anywhere to return to menu",55,150);
       }
       else if (winner==3) {
           LCD.WriteLine("Player 1 Wins: 0");
           LCD.WriteLine("Player 2 Wins: 0");
           LCD.WriteLine("AI Wins: 1");
            LCD.WriteAt("Click Anywhere to return to menu",55,150);
       }
       else {
           LCD.WriteLine("Player 1 Wins: 0");
           LCD.WriteLine("Player 2 Wins: 0");
           LCD.WriteLine("AI Wins: 1");
            LCD.WriteAt("Click Anywhere to return to menu",55,150);
       }
        while(!LCD.Touch(&x,&y))
        {
        }
       
        }
        //if instructions is pressed, display intstructions
        else if (y<YMAX/4.0 * 3.0) 
        {
        LCD.Clear();
        LCD.WriteLine("Each player takes their turn by moving a");
        LCD.WriteLine("single piece only diagonally, only forward,");
        LCD.WriteLine("and only on dark squares; if you are not");
        LCD.WriteLine("capturing a piece (not jumping), you may");
        LCD.WriteLine("only move one square;"); 
        LCD.WriteLine("if you are capturing");
        LCD.WriteLine("a piece (jump), jump over the piece and");
        LCD.WriteLine("land on the next diagonal square in a");
        LCD.WriteLine("Click for more");
        //wait for touch 
        while(!LCD.Touch(&x,&y))
        {
        }
        LCD.Clear();

        LCD.WriteLine("straight line; pieces are removed when");
        LCD.WriteLine("captured; if a capture is available,");
        LCD.WriteLine("the player must jump.");
        LCD.WriteLine("Click Anywhere to return to menu");

        }
        //if credits option is chosen
        else
        {
        LCD.Clear();
        LCD.WriteAt("Authors:", 125,80);
        LCD.WriteAt("Garrett Herb",110,100);
        LCD.WriteAt("Ariane Clerc",111,120);
        LCD.WriteAt("Ezekiel Wessendarp",82,140);
         LCD.WriteAt("Click Anywhere to return to menu",55,160);
        }
        //wait for touch
        while(!LCD.Touch(&x,&y))
        {
        }
    }
    
}
//CheckForWinner function written by Ariane Clerc that checks the status of each players chips and checks to see if there is currently a winner. 
int CheckForWinner(int opponent, Board status) {
    int i, j, player1chips=0, player2chips=0, aichips=0, winner;
    //opponent = 1 for ai, 2 for player 2
    //for loops to run through all elements of the array
    for (i=0;i<8;i++) {
        for (j=0;j<8;j++){
            //if there is a value in the array instead of NULL, enter the loop
            if(status.GetBoardState(i,j)) {
                //if the chip is red, add to player1 chips, otherwise add to player 2 chips
                if (status.GetBoardState(i,j)->GetColor()== RED) {
                   player1chips++;
                }
                else {
                    if(opponent==2){
                        player2chips++;
                    }
                    else {
                        aichips++;
                    }
                }
            }
        }
    }
    //checking to see if one of the opponents has zero chips left, if they do declare the winner and return the integer value
    if (opponent == 2) {
        //if player 2 has more chips they win
        if (player1chips==0 && player2chips!=0) {
            winner=2;
        }
        //if player 1 has more chips they win
        else if (player1chips!=0 && player2chips==0) {
            winner=1;
        }
        //no winner
        else {
            winner=0;
        }
    }
    //if ai has more chips they win
    else {
        if (player1chips==0 && aichips!=0) {
            winner=1;
        }
        //if player 2 has more chips they in
        else if (player1chips!=0 && aichips==0) {
            winner=2;
        }
        //no winner
        else {
            winner=0;
        }
    }
    return winner; 
}
//DisplayResults function written by Ariane Clerc in order to clear the screen and display the winner and the stats
void DisplayResults( int opponent, int winner) { 
    int aichips=0, player1chips=0, player2chips=0;
    //check if results should be displayed
    if (winner==0) {
    }
    else {
    //clear the screen to display the winner
        LCD.Clear();
        LCD.WriteAt("Player ", 110,60);
        LCD.WriteAt(winner, 165,60); 
        LCD.WriteAt(" wins!", 175, 60);
        //display the number of chips each player had
        LCD.WriteAt("Player 1 chips remaining: ", 55,80);
        LCD.WriteAt(player1chips, 255,84);
        //if player 2 is the opponent
        if (opponent == 2) {
             LCD.WriteAt("Player 2 chips remaining: ", 55,100);
            LCD.WriteAt(player2chips, 255,104);
        }
        //if ai is the opponent
        else {
             LCD.WriteAt("AI chips remaining: ", 55,100);
        LCD.WriteAt(aichips, 255,104);
        }
        //click screen to return to the menu
        LCD.WriteAt("Click Anywhere to return to menu",25,140);
    }
}


//Author: Garrett Herb
/*This is the turn function for the AI. It goes through specific criteria for each piece and then prints on the sidebar the piece it wants moved and where it wants that piece to go*/

/*Because there are many different specific criteria, there is a lot of repetition with slight variations that all end the move after printing the randomly generated text to the screen*/
void AIturn(Board status)
{
    int i, j, k, turn,x,y;

    turn = 1;

        //while loop for regular difficulty
        while (turn)
        {
            //generate random coordinates
            i = RandInt() % 8;
            j = RandInt() % 8;

            //different if statements for different move validity
            if (status.GetBoardState(i, j))
            {
                if (status.GetBoardState(i, j)->GetColor() == RED)
                {
                    if (!status.GetBoardState(i, j)->GetKing())
                    {
                        //check for different move validity
                        //adjacent to bottom edge and corner
                        if (i == 6 && j == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j-1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                

                                turn = 0;
                            }
                        }
                        //on edges
                        else if (j == 0)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (j == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //adjacent to edges
                        else if (j == 1)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (j == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //if adjacent to bottom edge
                        else if (i == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                        }
                        //if any other move
                        else
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                    }
                    else if (status.GetBoardState(i, j)->GetKing())
                    {
                        //checkfor different move validity
                        //top corners
                        if (i == 0 && j == 1)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (i == 0 && j == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //side and adjacent to top corner
                        else if (i == 1 && j == 0)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1,j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                //if their is a diagonal piece, check for empty diagonal
                else if (status.GetBoardState(i+1,j+1))
                {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }          
                }
                        }
                        //side and adjacent to bottom corner
                        else if (i == 6 && j == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //bottom corners
                        else if (i == 7 && j == 0)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (i == 7 && j == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                            
                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //sides
                        else if (j == 0)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (j == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //top and bottom
                        else if (i == 0)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (i == 7)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //adjacent to corners
                        else if (i == 1 && j == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (i == 6 && j == 1)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //adjacent to top and bottom
                        else if (i == 1)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (i == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //adjacent to sides
                        else if (j == 1)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        else if (j == 6)
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {} 
                            }
                            if (!status.GetBoardState(i-1, j-1))
                            {
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                        //if any other king move
                        else
                        {
                            //if statement for different moves
                            if (!status.GetBoardState(i+1, j+1))
                            {   
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i+1, j-1))
                            {
                                //make move
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i+1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j+1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j+1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}

                                //make move
                                

                                turn = 0;
                            }
                            else if (!status.GetBoardState(i-1, j-1))
                            {
                                LCD.WriteAt("AI move:", 245, 50);
                                LCD.WriteAt(i, 245, 70);
                                LCD.WriteAt(j, 270, 70);
                                LCD.WriteAt(i-1, 245, 90);
                                LCD.WriteAt(j-1, 270, 90);
                                while(!LCD.Touch(&x,&y))
                                {}
                                
                                //make move
                                

                                turn = 0;
                            }
                            //if their is a diagonal piece, check for empty diagonal
                            else if (status.GetBoardState(i+1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}

                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i+1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i + 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i+2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j+1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j + 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j+2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                            else if (status.GetBoardState(i-1, j-1)->GetColor()==BLACK)
                            {
                                if (!status.GetBoardState(i - 2, j - 2))
                                {
                                    LCD.WriteAt("AI move:", 245, 50);
                                    LCD.WriteAt(i, 245, 70);
                                    LCD.WriteAt(j, 270, 70);
                                    LCD.WriteAt(i-2, 245, 90);
                                    LCD.WriteAt(j-2, 270, 90);
                                    while(!LCD.Touch(&x,&y))
                                    {}
                                    //make move
                                    

                                    turn = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
        //at end of turn, change each piece at the end of the board to a king if not kinged already
        for (k = 0; k <= 7; ++k)
        {
            if (status.GetBoardState(7, k))
            {
                if (status.GetBoardState(7, k)->GetColor() == RED)
                {
                    if (!status.GetBoardState(7, k)->GetKing())
                    {
                        status.GetBoardState(7, k)->King();
                    }
                }
            }
        }
}

 //end of code   
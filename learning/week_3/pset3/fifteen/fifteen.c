/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// empty's coordinates.
int i_zero, j_zero;

// prototypes
void clear(void);
void greet(void);
void init(int board[d][d]);
void draw(int board[d][d]);
bool move(int board[d][d], int tile);
bool won(int board[d][d]);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init(board);

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw(board);

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won(board))
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int("");
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(board, tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}
/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(int board[d][d])
{
    // tile_numbers = d * d - 1;

    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            board[i][j] = d * d - 1 - (d * i + j);
        }
    }
    
    i_zero = d - 1;
    j_zero = d - 1;
    
    if ((d * d - 1) % 2 != 0) {
        int temp = board[d - 1][d - 2];
        board[d - 1][d - 2] = board[d - 1][d - 3];
        board[d - 1][d - 3] = temp;
    }

}
/**
 * Prints the board in its current state.
 */
void draw(int board[d][d])
{
    for (int i = 0; i < d; i++){
        for (int j = 0; j < d; j++) {
            if (board[i][j] != 0) {
                printf("%3d", board[i][j]);
            }
            else {
                printf("%3c", 95);
            }
        }
        printf("\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int board[d][d], int tile)
{
    int i = -1, j = 0;

    // searching of the tile.
    do {
        i++;
        for (j = 0; board[i][j] != tile && j < d - 1; j++);
    } while (board[i][j] != tile && i < d);

    bool horizontal = (i_zero - i == 0) && (j_zero - j == -1 || j_zero - j == 1);
    bool vertical = (j_zero - j == 0) && (i_zero - i == -1 || i_zero - i == 1);

    if (horizontal || vertical) {
        // swapping.
        board[i_zero][j_zero] = board[i][j];
        board[i][j] = 0;

        // rewriting of the empty space coordinates.
        i_zero = i; j_zero = j;
        return true;
    }
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(int board[d][d])
{
    for (int i = 0; i < d; i++)
        for (int j = 0; j < d - i / (d - 1); j++) 
            if (board[i][j] != d * i + j + 1)
                return false;

    return true;
}
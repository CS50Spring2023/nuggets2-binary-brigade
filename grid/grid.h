/*
 * grid.h - header file for Nuggets grid module
 *
 * A grid stores a 2D array of gridpoints, where each point
 * represents a certain row and column in the map. Each
 * gridpoint stores the terrain of the point, gold, and
 * players. The module includes functions to move players,
 * Handle gold collection, and return the display of the
 * grid to the players.
 *
 * Binary Brigade, Spring 2023
 */

#include "grid.c"

/**************** global types ****************/
typedef struct grid grid_t;

/**************** functions ****************/

/**************** gridInit ****************/
/* The functions is responsible for 
 * initializing the grid. Given a map text
 * file and a randomeSeed as parameters, 
 * it first determines the number of rows 
 * and columns in the map. In case the 
 * randomSeed was not originally provided, 
 * the caller is responsible for passing it 
 * to this function as a negative integer.
 * Hereafter, it iterates through the
 * contents of the map, saving each point
 * in a gridpoint struct, before generating
 * the gold. The function
 * returns a pointer to the created grid
 * upon successful termination. 
 */
grid_t* gridInit(char* pathName, int randomSeed);

/**************** gridDelete ****************/
/* The function deletes the grid.
 * Upon checking that the grid is not NULL,
 * it iterates through the 2D array, freeing
 * the memory allocated in the gridInit and
 * insertGridpoints functions and later
 * frees the array and the grid itself.  
 */
void gridDelete();

/**************** blocksVisibility ****************/
/* Function determines if a point's terrain is open
 * space that can be seen through (room spot or gold).
 * Returns false if it's an open space and true if
 * it's not. 
 */
bool blocksVisibility(const int row, const int col);

/**************** getnRows ****************/
/* Return the number of rows in the grid. 
 */
int getnRows();

/**************** getnColumns ****************/
/* Returns the number of columns in the grid.
 */
int getnColumns();
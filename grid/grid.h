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

/**************** global types ****************/
typedef struct gridpoint gridpoint_t;
typedef struct grid grid_t;


/**************** functions ****************/

/**************** gridInit ****************/
/* The functions is responsible for 
*  initializing the grid. Given a map text
*  file and a randomeSeed as parameters, 
*  it first determines the number of rows 
*  and columns in the map. In case the 
*  randomSeed was not originally provided, 
*  the caller is responsible for passing it 
*  to this function as a negative integer.
*  Hereafter, it iterates through the
*  contents of the map, saving each point
*  in a gridpoint struct, before generating
*  the gold. The function
*  returns a pointer to the created grid
*  upon successful termination. 
 */
grid_t* gridInit(char* pathName, int randomSeed);

/**************** gridDelete ****************/
/* The function deletes the grid.
*  Upon checking that the grid is not NULL,
*  it iterates through the 2D array, freeing
*  the memory allocated in the gridInit and
*  insertGridpoints functions and later
*  frees the array and the grid itself.  
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
/* Function is a getter for the number of
*  rows in the grid, making the information
*  available to other modules. 
 */
int getnRows();

/**************** getnColumns ****************/
/* Function is a getter for the number of
*  columns in the grid, making the information
*  available to other modules. 
 */
int getnColumns();

/**************** getPoints ****************/
/* Function is a getter for a point
*  in the grid, making the information
*  available to other modules. 
 */
gridpoint_t* getPoint(int row, int column);

/**************** getTerrain ****************/
/* Function is a getter for the terrain of
*  the gridpoint, making the information
*  available to other modules. 
 */
char getTerrain(gridpoint_t* gridpoint);

/**************** getPlayer ****************/
/* Function is a getter for the player of
*  a gridpoint, making the information
*  available to other modules. 
 */
char getPlayer(gridpoint_t* gridpoint);

/**************** getPointColumn ****************/
/* Function is a getter for the column of
*  a gridpoint, making the information
*  available to other modules. 
 */
int getPointColumn(gridpoint_t* gridpoint);

/**************** getPointRow ****************/
/* Function is a getter for the column of
*  a gridpoint, making the information
*  available to other modules. 
 */
int getPointRow(gridpoint_t* gridpoint);

/**************** setPlayer ****************/
/* Function is a setter for the player of
*  a gridpoint, making the information
*  available to other modules. 
 */
void setPlayer(gridpoint_t* gridpoint, char player);

/**************** setTerrain ****************/
/* Function is a setter for the player of
*  a gridpoint, making the information
*  available to other modules. 
 */
void setTerrain(gridpoint_t* gridpoint, char terrain);

/**************** setPointGold ****************/
/* Function is a setter for the gold of
*  a gridpoint, making the information
*  available to other modules. 
 */
void setPointGold(gridpoint_t* gridpoint, int nGold);

/**************** getPointGold ****************/
/* Function is a getter for the column of
*  a gridpoint, making the information
*  available to other modules. 
 */
int getPointGold(gridpoint_t* gridpoint);

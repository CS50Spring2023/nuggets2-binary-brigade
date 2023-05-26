/** 
 * grid.h
 * Author: Louis Weisdorf
 * CS50 Spring 2023, Project
 * Date: 05/20/2023
 * Description: This is a header file for the grid.c program.
 */
#include <grid.c>

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

/**************** readnColumns ****************/
/* The function takes in a map file (opened 
*  in the gridInit method that called it) and
*  the number of rows in the map.
*  It reads through each row of the map, 
*  keeping track of the longest row, which then
*  becomes the number of columns in the map.
*  This number is returned.
*/
static int readnColumns(FILE* map, int nRows);

/**************** insertGridpoints ****************/
/* The function takes the pathname for a map file.
*  Upon checking the parameters,
*  the function loops through the map (rows and
*  columns), creating and inserting a gridpoint 
*  struct into the 2D array belonging to the grid.
*/
static void insertGridpoints(char* pathName);

/**************** gridDelete ****************/
/* The function deletes the grid.
*  Upon checking that the grid is not NULL,
*  it iterates through the 2D array, freeing
*  the memory allocated in the gridInit and
*  insertGridpoints functions and later
*  frees the array and the grid itself.  
*/
void gridDelete();

/**************** gridpointNew ****************/
/* The functions takes in the coordinates of
*  the gridpoint (row, column) and the terrain
*  (what type of character is at the point).
*  It allocates memory for the gridpoint and 
*  sets the properties of the gridpoint
*  according to the parameters passed in. 
*  If the memory allocation is successful, it
*  returns a pointer to the gridpoint
*  (otherwise NULL).
*/
static gridpoint_t* gridpointNew(int row, int column, char terrain);

/**************** gridDisplay ****************/
/* The function creates a string to display the
*  grid. Upon checking that the grid is not NULL, 
*  it loops over each point in the grid, printing
*  players/gold/terrain/empty spaces based on
*  what is known and visible to the player.
*/
void gridDisplay();

/**************** gridDisplaySpectator ****************/
/* The function creates a string to display the
*  grid. It is designed for the spectator mode, meaning
*  that the function has full visibility of the grid
*  as well as the gold and players in it.  
*/
char* gridDisplaySpectator(); 

/**************** generateGold ****************/
/* The functions is called in the gridInit
*  method, taking in a randomSeed.
*  Depending on the randomSeed
*  input, the function creates a random seed.
*  Tracking the amount of undistributed gold,
*  it generates gold piles of varying size 
*  (between 10 to 30 nuggets), before placing
*  the piles in random locations on the map
*  (if the spot is in a room). 
*/
static void generateGold(int randomSeed);

/**************** movePlayer ****************/
/* The function handles the overall
*  functionality related to moving a player
*  across the grid. It takes in a game and
*  player struct, as well as the letter
*  pressed by the client indicating the desired
*  movement. Depending on the letter, the
*  x- and y-coordinates of the player are
*  determined accordingly. If the letter 
*  passed in is uppercase (requesting
*  continuous movement), the movement is
*  executed so long as possible. If the letter
*  is lowercase, the movement is executed 
*  once (if possible). The execution of the
*  movement itself is handled by a call to
*  the executeMovement function.
*/
void movePlayer(game_t* game, player_t* player, char letter);

/**************** executeMovement ****************/
/* The function does the work of executing
*  the movement of a player following a letter
*  press, having been set up by the movePlayer
*  function. It takes in a game struct, player
*  struct, as well as the current and new points
*  of the player (as product of the movement). 
*  The function calls other functions 
*  to handle various cases arising from the 
*  player movement.
*/
static void executeMovement(game_t* game, player_t* player, 
                    gridpoint_t current, gridpoint_t newPoint);

/**************** movePossible ****************/
/* Function checks if a move is possible. It 
*  takes in a player struct, as well the current
*  and prospective new position of the player. 
*  If the new coordinates land the player in a 
*  room spot, gold spot, or passage, the 
*  function returns true (indicating that the 
*  move is possible).
*  Otherwise, it returns false.  
*/
static bool movePossible(player_t* player, gridpoint_t current, gridpoint_t newPoint);

/**************** foundGold ****************/
/* Function checks if the new position of the
*  player, following their movement, causes
*  them to find gold. If it does, the gold
*  of held in that point transfers to the 
*  gold purse of the player. The terrain of
*  the gridpoint is updated to reflect the
*  changes.  
*/
static void foundGold(player_t* player);

/**************** foundPlayer ****************/
/* Function handles situations where a movement
*  causes the player to collide with another
*  player. If that is the case, the players
*  switch places.
*/
static void foundPlayer(player_t* player, game_t* game, gridpoint_t current, gridpoint_t newPoint);

/**************** placePlayer ****************/
/* Function takes in a player struct, placing
*  it into the map (either in an empty room
*  spot or passage).
*/
void placePlayer(player_t* player);

/**************** blocksVisibility ****************/
/* Function determines if a point's terrain is open
*  space that can be seen through (room spot or gold).
*  Returns false if it's an open space and true if
*  it's not. 
*/
static bool blocksVisibility(const int row, const int col);

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
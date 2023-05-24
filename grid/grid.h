/** 
 * grid.h
 * Author: Louis Weisdorf
 * CS50 Spring 2023, Project
 * Date: 05/20/2023
 * Description: This is a header file for the grid.c program.
 */
#include <grid.c>

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
int readnColumns(FILE* map, int nRows);

/**************** insertGridpoints ****************/
/* The function takes the pathname for a map file.
*  Upon checking the parameters,
*  the function loops through the map (rows and
*  columns), creating and inserting a gridpoint 
*  struct into the 2D array belonging to the grid.
*/
void insertGridpoints(char* pathName);

/**************** gridDelete ****************/
/* The function takes in a grid as parameter.
*  Upon checking that the grid is not NULL,
*  it iterates through the 2D array, freeing
*  the memory allocated in the gridInit and
*  insertGridpoints functions.  
*/
void gridDelete(grid_t* grid);

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
gridpoint_t* gridpointNew(int row, int column, char terrain);

/**************** gridpointNew ****************/
/* The function takes in a grid. Upon checking
*  that the grid is not NULL, it loops over
*  each of the rows and columns, printing the
*  terrain stored in the gridpoint at that
*  location. 
*/
void gridDisplay(grid_t* grid);

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
void generateGold(int randomSeed);

/**************** movePlayer ****************/
/* The function handles the overall
*  functionality related to moving a player
*  across the grid. It takes in a game and
*  player struct, as well as the letter
*  pressed by the client indicating the desired
*  movement. Depending on the letter, the
*  x- and y-coordinates of the player are
*  changed accordingly. The function calls 
*  other functions to handle various cases
*  arising from the player movement.
*/
void movePlayer(game_t* game, player_t* player, char letter);

/**************** movePossible ****************/
/* Function checks if a move is possible. It 
*  takes in a player struct, as well as the
*  requested changes the in x- and y-coordinates
*  of the player. If the new coordinates land
*  the player in a room spot, gold spot, passage,
*  or on another player, the function returns
*  true (indicating that the move is possible).
*  Otherwise, it returns false.  
*/
bool movePossible(player_t* player, int changeRow, int changeColumn);

/**************** foundGold ****************/
/* Function checks if the new position of the
*  player, following their movement, causes
*  them to find gold. If it does, the gold
*  of held in that point transfers to the 
*  gold purse of the player. The terrain of
*  the gridpoint is updated to reflect the
*  changes.  
*/
void foundGold(player_t* player);

/**************** foundPlayer ****************/
/* Function handles situations where a movement
*  causes the player to collide with another
*  player. If that is the case, the players
*  switch places.
*/
void foundPlayer(player_t* player, game_t* game, gridpoint_t current, gridpoint_t newPoint);

/**************** placePlayer ****************/
/* Function takes in a player struct, placing
*  it into the map (either in an empty room
*  spot or passage).
*/
void placePlayer(player_t* player);
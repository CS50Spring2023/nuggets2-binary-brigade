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

/**************** gridDisplay ****************/
/* The function takes in a player, creates a 
*  string to display the player-specific grid.
*  Upon checking that the grid is not NULL, 
*  it loops over each point in the grid, printing
*  players/gold/terrain/empty spaces based on
*  what is known and visible to the player.
*/
char* gridDisplay(player_t* player);

/**************** gridDisplaySpectator ****************/
/* The function creates a string to display the
 * grid. It is designed for the spectator mode, meaning
 * that the function has full visibility of the grid
 * as well as the gold and players in it.  
 */
char* gridDisplaySpectator(); 

/**************** movePlayer ****************/
/* The function handles the overall
 * functionality related to moving a player
 * across the grid. It takes in a game and
 * player struct, as well as the letter
 * pressed by the client indicating the desired
 * movement. Depending on the letter, the
 * x and y coordinates of the player are
 * determined accordingly. If the letter 
 * passed in is uppercase (requesting
 * continuous movement), the movement is
 * executed so long as possible. If the letter
 * is lowercase, the movement is executed 
 * once (if possible). The execution of the
 * movement itself is handled by a call to
 * the executeMovement function.
 */
void movePlayer(game_t* game, player_t* player, char letter);

/**************** executeMovement ****************/
/* The function does the work of executing
*  the movement of a player following a letter
*  press, having been set up by the movePlayer
*  function. It takes in a game struct, player
*  struct, as well as the changes to x and y. 
*  The function calls other functions 
*  to handle various cases arising from the 
*  player movement.
*/
static void executeMovement(game_t* game, player_t* player, 
                    int changeRow, int changeColumn);

/**************** movePossible ****************/
/* Function checks if a move is possible. It 
*  takes in a player struct, as well prospective
*  changes in the position of the player. 
*  If the new coordinates land the player in a 
*  room spot, gold spot, or passage, the 
*  function returns true (indicating that the 
*  move is possible).
*  Otherwise, it returns false.  
*/
static bool movePossible(player_t* player, int changeRow, int changeColumn);

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
static void foundPlayer(player_t* player, game_t* game, 
                        gridpoint_t* current, gridpoint_t* newPoint);

/**************** placePlayer ****************/
/* Function takes in a player struct, placing
 * it into the map (either in an empty room
 * spot or passage).
 */
void placePlayer(player_t* player);

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
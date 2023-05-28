/* 
 * game.h - header file for CS50 game module
 * 
 * Binary Brigade, Spring, 2023
 */

#include "../player/player.h"

/**************** local global types ****************/
static const int goldTotal;    //amount of gold in each game
static const int maxPlayers;   //max amount of players a game can hold

/**************** global types ****************/
typedef struct game game_t;  // opaque to users of the module

/**************** FUNCTION ****************/
/* Create a new game structure w/ given grid parameter
 *
 * We return:
 *   pointer to a new game; NULL if error (out of memory).
 */
game_t* initialize_game(grid_t* grid);

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

/**************** placePlayer ****************/
/* Function takes in a player struct, placing
 * it into the map (either in an empty room
 * spot or passage).
 */
void placePlayer(player_t* player);

/**************** FUNCTION ****************/
/* Add a new player to the game
 *
 * We return:
 *   0 if success; 1 if error (maxPlayers already reached).
 */
int add_player(game_t* game, player_t* player);

/**************** FUNCTION ****************/
/* Caller provides a pointer to a game
 * struct and an address struct. 
 *
 * We return:
 * a pointer to the player with the address
 * passed in, or NULL if there is no such
 * player.
 */
player_t*
find_player(game_t* game, addr_t address);

/**************** FUNCTION ****************/
/* Update gold count in game
 *
 * We return:
 *   0 if success; 1 if error (new gold count isn't in correct range).
 */
int update_gold(game_t* game, int updateGoldCount);

/**************** FUNCTION ****************/
/* Returns the list of players
 *
 * We return:
 *   the list of players associated with the given game.
 */
player_t** get_players(game_t* game);

/**************** FUNCTION ****************/
/* Provided a game, the function creates
 * a summary of the game.
 *
 * We return:
 *   a string-representation of the summary.
 *   The caller is responsible for later 
 *   freeing the summary string. 
 */
char*
game_summary(game_t* game);

/* Take in a pointer to a game and frees each player in the 
 * array, then the space for the array and the game itself.
 *
 * We return:
 *   nothing
 */
void delete_game(game_t* game);
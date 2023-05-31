/* 
 * game.h - header file for CS50 game module
 * 
 * Binary Brigade, Spring, 2023
 */

#include "../player/player.h"
#include "../grid/grid.h"

/**************** global types ****************/
typedef struct game game_t;  // opaque to users of the module

/**************** FUNCTION ****************/
/* Create a new game structure w/ given grid parameter
 *
 * We return:
 *   pointer to a new game; NULL if error (out of memory).
 */
void initialize_game(grid_t* grid);

/**************** gridDisplay ****************/
/* The function takes in a player, creates a 
*  string to display the player-specific grid.
*  Upon checking that the grid is not NULL, 
*  it loops over each point in the grid, printing
*  players/gold/terrain/empty spaces based on
*  what is known and visible to the player.
*/
void gridDisplay(addr_t address, player_t* player);

/**************** gridDisplaySpectator ****************/
/* The function creates a string to display the
 * grid. It is designed for the spectator mode, meaning
 * that the function has full visibility of the grid
 * as well as the gold and players in it.  
 */
void gridDisplaySpectator(addr_t address); 

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
void movePlayer(player_t* player, char letter);

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
int add_player(player_t* player);

/**************** FUNCTION ****************/
/* Caller provides a pointer to a game
 * struct and an address struct. 
 *
 * We return:
 * a pointer to the player with the address
 * passed in, or NULL if there is no such
 * player.
 */
player_t* find_player(addr_t address);

/**************** FUNCTION ****************/
/* Add a new spectator to the game
 *
 * We return:
 *   NULL if no previous spectator;
 *   old spectator's address if previous spectator.
 */
addr_t add_spectator(addr_t spectator);

/**************** FUNCTION ****************/
/* Get spectator's address from the game
 *
 * We return:
 *   NULL if no previous spectator;
 *   spectator's address if exists
 */
addr_t get_spectator();

/**************** FUNCTION ****************/
/* Gets the grid dimensions of the game
 *
 * We return:
 *   formatted dimensions of the grid
 */
void get_grid_dimensions();

/**************** FUNCTION ****************/
/* Sets player as inactive in game
 *
 * We return:
 *   0 if success; 1 if error (no matching player)
 */
int game_inactive_player(game_t* game, player_t* player);

/**************** FUNCTION ****************/
/* Update gold count in game
 *
 * We return:
 *   0 if success; 1 if error (new gold count isn't in correct range).
 */
int update_gold(game_t* game, int updateGoldCount);

/**************** FUNCTION ****************/
/* Getter for total gold in game
 *
 * We return:
 *   total gold in game -- should always be 250.
 */
int get_total_gold(game_t* game);

/**************** FUNCTION ****************/
/* Getter for gold still available in game
 *
 * We return:
 *   current available gold in game.
 */
int  get_available_gold();


/**************** FUNCTION ****************/
/* Provided a game, the function creates
 * a summary of the game.
 *
 * We return:a
 *   a string-representation of the summary.
 *   The caller is responsible for later 
 *   freeing the summary string. 
 */
void game_summary(addr_t address);

/* Take in a pointer to a game and frees each player in the 
 * array, then the space for the array and the game itself.
 *
 * We return:
 *   nothing
 */
void delete_game();
/**************** FUNCTION ****************/
/* Returns the list of players
 *
 * We return:
 *   the list of players associated with the given game.
 */
player_t** get_players(); 

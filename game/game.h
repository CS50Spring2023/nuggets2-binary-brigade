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
player_t* find_player(game_t* game, addr_t address);

/**************** FUNCTION ****************/
/* Add a new spectator to the game
 *
 * We return:
 *   NULL if no previous spectator;
 *   old spectator's address if previous spectator.
 */
addr_t* add_spectator(game_t* game, player_t* spectator);

/**************** FUNCTION ****************/
/* Gets the grid dimensions of the game
 *
 * We return:
 *   formatted dimensions of the grid
 */
char* get_grid_dimensions(game_t* game);

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
int  get_available_gold(game_t* game);

/**************** FUNCTION ****************/
/* Returns the list of players
 *
 * We return:
 *   the list of players associated with the given game.
 */
player_t** get_players(game_t* game);

/**************** FUNCTION ****************/
/* Returns the current spectator's address
 *
 * We return:
 *   the current spectator's address if there is one and NULL if there isn't 
 */
addr_t* get_spectator_address(game_t* game);

/**************** FUNCTION ****************/
/* Provided a game, the function creates
 * a summary of the game.
 *
 * We return:a
 *   a string-representation of the summary.
 *   The caller is responsible for later 
 *   freeing the summary string. 
 */
char* game_summary(game_t* game);

/* Take in a pointer to a game and frees each player in the 
 * array, then the space for the array and the game itself.
 *
 * We return:
 *   nothing
 */
void  delete_game(game_t* game);

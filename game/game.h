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
/* Returns the current spectator's address
 *
 * We return:
 *   the current spectator's address if there is one and NULL if there isn't 
 */
addr_t* get_spectator_addr(game_t* game);

/**************** FUNCTION ****************/
/* Provided a game, the function creates
 * a summary of the game.
 *
 * We return:
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
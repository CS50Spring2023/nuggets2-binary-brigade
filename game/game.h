/* 
 * game.h - header file for CS50 game module
 * 
 * Binary Brigade, Spring, 2023
 */

/**************** local global types ****************/
static const int goldTotal;    //amount of gold in each game
static const int maxPlayers;   //max amount of players a game can hold

/**************** global types ****************/
typedef struct player player_t;  // opaque to users of the module

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
/* Update gold count in game
 *
 * We return:
 *   0 if success; 1 if error (new gold count isn't in correct range).
 */
int update_gold(game_t* game, int updateGoldCount);

/* Take in a pointer to a game and frees the space to the players array and the game
 *
 * We return:
 *   nothing
 */
void delete_game(game_t* game)
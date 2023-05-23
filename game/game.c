/* 
 * game.c - CS50 'game' module
 *
 * see game.h for more information.
 *
 * Binary Brigade, Spring, 2023
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "../grid/grid.h"
#include "../player/player.h"

/**************** local global types ****************/
static const int goldTotal = 250;
static const int maxPlayers = 26;

/**************** global types ****************/
typedef struct game{
  grid_t* grid;
  int totalGold;
  int goldAvailable;
  int playerCount;
  player_t** players;
} game_t;

/**************** FUNCTION ****************/
/* see player.h for description */
game_t* 
initialize_game(grid_t* grid)
{
  game_t* game = mem_malloc(sizeof(game_t));

  if (game == NULL) {
    return NULL;              
    
  } else {
    game->grid = grid;
    game->totalGold = goldTotal;
    game->goldAvailable = goldTotal;
    game->playerCount = 0;
    player_t** players = calloc(maxPlayers, sizeof(player_t*));
    game->players = players;
  }
}

/**************** FUNCTION ****************/
/* see player.h for description */
int
add_player(game_t* game, player_t* player)
{
  if (game->playerCount == maxPlayers){
    return 1;
  }
  game->players[game->playerCount] = player;
  game->playerCount++;
  return 0;
}

/**************** FUNCTION ****************/
/* see player.h for description */
int
update_gold(game_t* game, int updateGoldCount)
{
  if (game->goldAvailable < updateGoldCount || updateGoldCount < 0){
    return 1;
  }
  game->goldAvailable = updateGoldCount;
  return 0;
}

/* see player.h for description */
void 
delete_game(game_t* game)
{
  if (game != NULL){
    free(game->players);
    free(game);
  }
}
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
static const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**************** global types ****************/
typedef struct game{
  grid_t* grid;
  int totalGold;
  int goldAvailable;
  int playerCount;
  player_t** players;
  player_t* spectator;
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
    player_t* spectator = malloc(sizeof(player_t*));
    game->spectator = spectator;
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
  player_set_letter(player, alphabet[game->playerCount]);
  game->players[game->playerCount] = player;
  game->playerCount++;
  return 0;
}

/**************** FUNCTION ****************/
/* see player.h for description */
char*
add_spectator(game_t* game, player_t* spectator)
{
  if (game->spectator == NULL){
    game->spectator = spectator;
    return NULL;
  }
  char* pastSpectator = player_get_port(game->spectator);
  game->spectator = spectator;
  return pastSpectator;
}

/**************** FUNCTION ****************/
/* see player.h for description */
char* 
get_grid_dimensions(game_t* game)
{
  int rows = getnRows(game->grid);
  int columns = getnColumns(game->grid);
  int outputLength = strlen("GRID  ") + strlen(itoa(rows)) + strlen(itoa(columns));
  char *dimensions = malloc(sizeof(char) * outputLength);
  sprintf(dimensions, "%s %d %d", "GRID ", rows, columns);
  return dimensions;
}

/**************** FUNCTION ****************/
/* see player.h for description */
int
game_inactive_player(game_t* game, player_t* player)
{
  for (player_t* currPlayer = game->players; currPlayer != NULL; currPlayer++) {
    if (player == currPlayer){
      player_inactive(currPlayer);
      return 0;
    }
  }
  return 1;
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

/**************** FUNCTION ****************/
/* see player.h for description */
int
get_total_gold(game_t* game)
{
  return game->totalGold;
}

/**************** FUNCTION ****************/
/* see player.h for description */
int 
get_available_gold(game_t* game)
{
  return game->goldAvailable;
}

/**************** FUNCTION ****************/
/* see player.h for description */
player_t**
get_players(game_t* game)
{
  return game->players;
}

/**************** FUNCTION ****************/
/* see player.h for description */
char*
game_summary(game_t* game)
{
  char* summary = mem_malloc(sizeof(char) + 1);
  char* temp = mem_malloc(sizeof(char) + 1);
  summary = "GAME OVER:\n";
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
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
#include <ctype.h>
#include "../grid/grid.h"
#include "../player/player.h"
#include "../lib/mem.h"
#include "game.h"

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
  addr_t* spectator;
} game_t;

/**************** static functions ****************/
static bool movePossible(player_t* player, int changeRow, int changeColumn);
static void executeMovement(player_t* player, int changeRow, int changeColumn);
static void foundPlayer(player_t* player, gridpoint_t* current, gridpoint_t* updated);
static void foundGold(player_t* player);


game_t* game;

/**************** FUNCTION ****************/
/* see game.h for description */
void 
initialize_game(grid_t* grid)
{
  game = mem_malloc(sizeof(game_t));

  if (game == NULL) {
    return;              
    
  } else {
    game->grid = grid;
    game->totalGold = goldTotal;
    game->goldAvailable = goldTotal;
    game->playerCount = 0;
    player_t** players = calloc(maxPlayers, sizeof(player_t*));
    game->players = players;
    addr_t* spectator = malloc(sizeof(addr_t*));
    game->spectator = spectator;
  }

}

/**************** FUNCTION ****************/
/* see game.h for description */
int
add_player(player_t* player)
{
  if (game->playerCount == maxPlayers){
    return 1;
  }
  set_letter(player, alphabet[game->playerCount]);
  game->players[game->playerCount] = player;
  game->playerCount++;
  return 0;
}

/**************** placePlayer ****************/
/* See detailed description in grid.h. */
void 
placePlayer(player_t* player)
{
    while (true) {
        // Generating a random row and column for the player to be placed into
        int randRow = ((rand() % (getnRows(game->grid))));
        int randColumn = ((rand() % (getnColumns(game->grid))));

        gridpoint_t* randomPoint = getPoint(randRow, randColumn);
                
        /* If the random location is in a room or passage, and if there is not
        already a player in the spot */
        if ((getTerrain(randomPoint) == '.') || (((getTerrain(randomPoint) == '#')) &&
            getPlayer(randomPoint) == 0)) {
            // Inserting the player into random point
            setPlayer(randomPoint, get_letter(player));
          
            // Adding the location to the player
            set_y(player, getPointRow(randomPoint));
            set_x(player, getPointColumn(randomPoint));

      // Exiting the loop
      break;
    }
  }
}

/**************** movePlayer ****************/
/* See detailed description in grid.h. */
void 
movePlayer(player_t* player, char letter) 
{
  // Changes to location if successful movement
  int changeRow;
  int changeColumn;
    
  // Setting up the switch to handle each character press
  switch (letter) {
    case 'h': 
      changeRow = 0;
      changeColumn = -1;
      break;
    case 'l': 
      changeRow = 0;
      changeColumn = 1;
      break;
    case 'j': 
      changeRow = 1;
      changeColumn = 0;
      break;
    case 'k': 
      changeRow = -1;
      changeColumn = 0;
      break;
    case 'y': 
      changeRow = -1;
      changeColumn = -1;
      break;
    case 'u': 
      changeRow = -1;
      changeColumn = 1;
      break;
    case 'b': 
      changeRow = 1;
      changeColumn = -1;
      break;
    case 'n': 
      changeRow = 1;
      changeColumn = 1;
       break;
  }

    // If the letter is uppercase (continuous movement)
    if (isupper(letter)) {
        // As long as the move is possible, executing movement
        while (movePossible(player, changeRow, changeColumn)) {
            executeMovement(player, changeRow, changeColumn);
        }
    }

    // If the letter is lowercase (single movement)
    else {
        if (movePossible(player, changeRow, changeColumn)) {
            executeMovement(player, changeRow, changeColumn);
        }
    }
}

/**************** movePossible ****************/
/* Function checks if a move is possible. It
 * takes in a player struct, as well the current
 * and prospective new position of the player.
 * If the new coordinates land the player in a
 * room spot, gold spot, or passage, the
 * function returns true (indicating that the
 * move is possible).
 * Otherwise, it returns false.
 */
static bool 
movePossible(player_t* player, int changeRow, int changeColumn) 
{   
    // Current location of the player
    gridpoint_t* current = getPoint(get_y(player), get_x(player));

    // Potential new location of the player
    gridpoint_t* updated = getPoint( (getPointRow(current) + changeRow), (getPointColumn(current) + changeColumn) );
    
    /* If the new position is either a spot in a room, passage, or
    contains gold */
    if (( getTerrain(updated) == '.') || (getTerrain(updated) == '#') 
        || (getTerrain(updated) == '*')) {
        return true;
    }

    // If none of the above apply
    return false;
}

/**************** executeMovement ****************/
/* The function does the work of executing
 * the movement of a player following a letter
 * press, having been set up by the movePlayer
 * function. It takes in a game struct, player
 * struct, as well as the current and new points
 * of the player (as product of the movement).
 * The function calls other functions
 * to handle various cases arising from the
 * player movement.
 */
static void 
executeMovement(player_t* player, int changeRow, int changeColumn)
{
    // Current location of the player
   gridpoint_t* current = getPoint(get_y(player), get_x(player));

    // Potential new location of the player
    gridpoint_t* updated = getPoint( (getPointRow(current) + changeRow), (getPointColumn(current) + changeColumn) );
    
    // Checking if the move causes the player to step into another player
    foundPlayer(player, current, updated);

    // Updating the location of the player
    set_y(player, getPointRow(updated));
    set_x(player, getPointColumn(updated));

    // Updating the contents of the gridpoints
    setPlayer(updated, get_letter(player));

  // Checking if the move causes the player to find gold
  foundGold(player);
}

/**************** foundGold ****************/
/* Function checks if the new position of the
 * player, following their movement, causes
 * them to find gold. If it does, the gold
 * of held in that point transfers to the
 * gold purse of the player. The terrain of
 * the gridpoint is updated to reflect the
 * changes.
 */
static void 
foundGold(player_t* player)
{
    // Setting a variable for the gridpoint
    gridpoint_t* gridpoint = getPoint(get_y(player), get_x(player));

    // If there is a gold pile in the players location
    if (getTerrain(gridpoint) == '*') {
        // Assign gold from point to player
        int playerNewGold = get_gold(player) + getPointGold(gridpoint);
        set_gold(player, playerNewGold);
        setPointGold(gridpoint, 0);

        // Updating the terrain of the point
        setTerrain(gridpoint, '.');
    }
}


/**************** foundPlayer ****************/
/* Function handles situations where a movement
 * causes the player to collide with another
 * player. If that is the case, the players
 * switch places.
 */
static void 
foundPlayer(player_t* player, gridpoint_t* current, gridpoint_t* updated)
{
  // Saving the players array from the game struct in variable
  player_t** players = get_players();

    // If there is a player in the new location
    if (isalpha(getPlayer(updated))) {
        // Looping through the players in the game to find the player
        for (int i = 0; players[i] != NULL; i++) {
            // If the coordinates of the new position match the player
            if ((getPointColumn(updated) == get_x(players[i])) && 
                (getPointRow(updated) == get_y(players[i]))) {
                // Setting these coordinates to be those of current
                set_x(players[i], getPointColumn(current));
                set_y(players[i], getPointRow(current));
            }
        }
    }

    // If there is not a player in the new location
    else {
      setPlayer(current, '0');
    }
}

/**************** gridDisplay ****************/
/* See grid.h for description. */
char* 
gridDisplay(player_t* player) 
{
  // Size of grid string: rows*columns, plus one newline per row, plus one for null pointer
  //char[(grid->nRows)*(grid->nColumns + 1) + 1] gridString;
  char* gridString = mem_calloc(((getnRows(game->grid))*(getnColumns(game->grid)+ 1) + 1), sizeof(char));
  int index = 0;

  // Checking if the grid is NULL
  if (game->grid != NULL) {
    updateVisibility(player);
    // Looping over rows and columns in the grid
    for (int row = 0; row < getnRows(game->grid); row++) {
      for (int column = 0; column < getnColumns(game->grid); column++) {
        char terrain = getTerrain(getPoint(row, column));
        char playerAtPoint = getPlayer(getPoint(row, column));
        // Point contains a player
        if (playerAtPoint != 0) {
          if (isVisible(player, row, column)) {
            // If the player at the point is the player itself, print @ sign
            if (playerAtPoint == get_letter(player)) {
                gridString[index] = '@';
                index++;
            }
            gridString[index] = playerAtPoint;
            index++;
          }
          else if (isKnown(player, row, column)) {
            gridString[index] = terrain;
            index++;
          }
          else {
            gridString[index] = ' ';
            index++;
          }
        }
        // Point does not contain a player
        else {
          // Tile contains gold
           if (terrain == '*' ) {
            if (isVisible(player, row, column)) {
              gridString[index] = terrain;
              index++;
            }
            else if (isKnown(player, row, column)) {
              gridString[index] = '.';
              index++;
            }
            else {
              gridString[index] = ' ';
              index++;
            }
          }
          // Tile does not contain gold
          else {
            if (isKnown(player, row, column)) {
              gridString[index] = terrain;
              index++;
            }
            else {
              gridString[index] = ' ';
              index++;
            }
          }
        }     
      }
      // Printing newline for the next row
      gridString[index] = '\n';
      index++;
    }
  }
  return gridString;
}

/**************** gridDisplaySpectator ****************/
/* See grid.h for description. */
char* 
gridDisplaySpectator() 
{
  // Size of grid string: rows*columns, plus one newline per row, plus one for null pointer
  char* gridString = mem_calloc(((getnRows(game->grid))*(getnColumns(game->grid)+ 1) + 1), sizeof(char));
  char playerAtPoint;
  char terrain;
  int index = 0;

  // Checking if the grid is NULL
  if (game->grid != NULL) {
    // Looping over rows and columns in the grid
    for (int row = 0; row < getnRows(game->grid); row++) {
      for (int column = 0; column < getnColumns(game->grid); column++) {
        // If the point contains a player, printing the player letter
        playerAtPoint = getPlayer(getPoint(row, column));
        if (playerAtPoint != 0) {
          gridString[index] = playerAtPoint;
          index++;
        }

        // If the point does not contain a player, printing the terrain
        else {
          terrain = getTerrain(getPoint(row, column));
          gridString[index] = terrain;
          index++;
        }
      }
      // Printing newline for the next row
      gridString[index] = '\n';
    }
  }
  return gridString;
}

/**************** FUNCTION ****************/
/* see game.h for description */
player_t*
find_player(addr_t address)
{
  // Looping over players in the game
  for (int i = 0; i < game->playerCount; i++) {
    player_t* currPlayer = game->players[i];
    addr_t playerAddress = get_address(currPlayer);

    // If adresses match, returning player
    if (&address == &playerAddress) {
      return currPlayer;
    }
  } 

  // If no player was found with the correct address, returning NULL
  return NULL;
}

/**************** FUNCTION ****************/
/* see game.h for description */
addr_t*
add_spectator(addr_t* spectator)
{
  if (game->spectator == NULL){
    game->spectator = spectator;
    return NULL;
  }
  addr_t* pastSpectator = game->spectator;
  game->spectator = spectator;
  return pastSpectator;
}

/**************** FUNCTION ****************/
/* see game.h for description */
addr_t*
get_spectator()
{
  addr_t* pastSpectator = game->spectator;
  return pastSpectator;
}

/**************** FUNCTION ****************/
/* see game.h for description */
char* 
get_grid_dimensions()
{
  int rows = getnRows(game->grid);
  int columns = getnColumns(game->grid);
  // int outputLength = strlen("GRID  ") + strlen(itoa(rows)) + strlen(itoa(columns));
  int outputLength = strlen("GRID  ") + snprintf(NULL, 0, "%d", rows) + snprintf(NULL, 0, "%d", columns);
  char *dimensions = malloc(sizeof(char) * outputLength);
  sprintf(dimensions, "%s %d %d", "GRID ", rows, columns);
  return dimensions;
}

/**************** FUNCTION ****************/
/* see game.h for description */
int
game_inactive_player(game_t* game, player_t* player)
{
  for (int i = 0; i < game->playerCount; i++) {
    player_t* currPlayer = game->players[i];
    if (player == currPlayer){
      player_inactive(currPlayer);
      return 0;
    }
  }
  return 1;
}

/**************** FUNCTION ****************/
/* see game.h for description */
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
/* see game.h for description */
int
get_total_gold(game_t* game)
{
  return game->totalGold;
}

/**************** FUNCTION ****************/
/* see game.h for description */
int 
get_available_gold()
{
  return game->goldAvailable;
}


/**************** FUNCTION ****************/
/* see game.h for description */
char*
game_summary()
{
  // Length of initial statement (letter and score) and max name length
  int maxLineSize = 9 + get_MaxNameLength();
  // Calculating maximum size of the entire summary string
  int maxSummarySize = (maxLineSize + 1) * game->playerCount;

  // Allocating memmory for the summary string
  char* summary = mem_malloc(maxSummarySize * sizeof(char) + 1);

  // Inserting GAME OVER as opening line for the summary
  strcat(summary, "GAME OVER\n");

  // Looping over the players in the game, adding their information to summary
  for (int i = 0; i < game->playerCount; i++) {
    player_t* currPlayer = game->players[i];

    // Saving player information in variable currLine
    char* currLine = mem_malloc(maxLineSize * sizeof(char) + 1); 
    sprintf(currLine, "%c   %3d %s\n", 
      get_letter(currPlayer), get_gold(currPlayer), get_name(currPlayer));
    
    // Adding information to summary string
    strcat(summary, currLine);

    // Cleaning up before next line
    mem_free(currLine);
  }

  // Adding newline to end of summary for clean look
  strcat(summary, "\n");

  // Returning summary
  return summary;
}

/* see game.h for description */
void 
delete_game()
{
  if (game != NULL){
    // Freeing each player
    for (int i = 0; i < game->playerCount; i++) {
      player_delete(game->players[i]);
    }
    // player_delete(game->spectator);

    free(game->players);
    free(game->spectator);
    free(game);
  }
}

/**************** FUNCTION ****************/
/* see game.h for description */
player_t**
get_players()
{
  return game->players;
}
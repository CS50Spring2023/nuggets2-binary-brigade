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
#include "../lib/mem.h"

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

// Functions added from grid (for ease of overview, Charlie)
/* void placePlayer(player_t* player);
void movePlayer(game_t* game, player_t* player, char letter);
char* gridDisplay(player_t* player);
char* gridDisplaySpectator();
static void executeMovement(game_t* game, player_t* player, 
                    int changeRow, int changeColumn);
static void foundGold(player_t* player);
static void foundPlayer(player_t* player, game_t* game, 
                gridpoint_t* current, gridpoint_t* updated);
static bool movePossible(player_t* player, int changeRow, int changeColumn); */

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
    addr_t* spectator = malloc(sizeof(addr_t*));
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

/**************** placePlayer ****************/
/* See detailed description in grid.h. */
void 
placePlayer(player_t* player)
{
    while (true) {
        // Generating a random row and column for the player to be placed into
        int randRow = ((rand() % (grid->nRows)));
        int randColumn = ((rand() % (grid->nColumns)));

        gridpoint_t* randomPoint = grid->points[randRow][randColumn];
                
        /* If the random location is in a room or passage, and if there is not
        already a player in the spot */
        if (((randomPoint->terrain == '.') || (randomPoint->terrain == '#')) &&
            randomPoint->player == 0) {
            // Inserting the player into random point
            randomPoint->player = get_letter(player);

            // Adding the location to the player
            set_y(player, randomPoint->row);
            set_x(player, randomPoint->column);

      // Exiting the loop
      break;
    }
  }
}

/**************** movePlayer ****************/
/* See detailed description in grid.h. */
void 
movePlayer(game_t* game, player_t* player, char letter) 
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
            executeMovement(game, player, changeRow, changeColumn);
        }
    }

    // If the letter is lowercase (single movement)
    else {
        if (movePossible(player, changeRow, changeColumn)) {
            executeMovement(game, player, changeRow, changeColumn);
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
    gridpoint_t* current = grid->points[get_y(player)][get_x(player)];

    // Potential new location of the player
    gridpoint_t* updated = grid->points[current->row + changeRow]
                        [current->column + changeColumn];
    
    /* If the new position is either a spot in a room, passage, or
    contains gold */
    if ((updated->terrain == '.') || (updated->terrain == '#') 
        || (updated->terrain == '*')) {
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
executeMovement(game_t* game, player_t* player, int changeRow, int changeColumn)
{
    // Current location of the player
    gridpoint_t* current = grid->points[get_y(player)][get_x(player)];

    // Potential new location of the player
    gridpoint_t* updated = grid->points[current->row + changeRow]
                        [current->column + changeColumn];
    
    // Checking if the move causes the player to step into another player
    foundPlayer(player, game, current, updated);

    // Updating the location of the player
    set_y(player, updated->row);
    set_x(player, updated->column);

    // Updating the contents of the gridpoints
    updated->player = get_letter(player);

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
    gridpoint_t* gridpoint = grid->points[get_y(player)][get_x(player)];

    // If there is a gold pile in the players location
    if (gridpoint->terrain == '*') {
        // Assign gold from point to player
        int playerNewGold = get_gold(player) + gridpoint->nGold;
        set_gold(player, playerNewGold);
        gridpoint->nGold = 0;

        // Updating the terrain of the point
        gridpoint->terrain = '.';
    }
}

/**************** foundPlayer ****************/
/* Function handles situations where a movement
 * causes the player to collide with another
 * player. If that is the case, the players
 * switch places.
 */
static void 
foundPlayer(player_t* player, game_t* game, gridpoint_t* current, gridpoint_t* updated)
{
  // Saving the players array from the game struct in variable
  player_t** players = get_players(game);

    // If there is a player in the new location
    if (updated->player != 0) {
        // Looping through the players in the game to find the player
        for (int i = 0; players[i] != NULL; i++) {
            // If the coordinates of the new position match the player
            if ((updated->column == get_x(players[i])) && 
                (updated->row == get_y(players[i]))) {
                // Setting these coordinates to be those of current
                set_x(players[i], current->column);
                set_y(players[i], current->row);
            }
        }
    }

    // If there is not a player in the new location
    else {
        current->player = 0;
    }
}

/**************** gridDisplay ****************/
/* See grid.h for description. */
char* 
gridDisplay(player_t* player) 
{
  // Size of grid string: rows*columns, plus one newline per row, plus one for null pointer
  //char[(grid->nRows)*(grid->nColumns + 1) + 1] gridString;
  char* gridString = mem_calloc(((grid->nRows)*(grid->nColumns + 1) + 1), sizeof(char));
  char terrain;
  char playerAtPoint;
  int index = 0;

  // Checking if the grid is NULL
  if (grid != NULL) {
    updateVisibility(player);
    // Looping over rows and columns in the grid
    for (int row = 0; row < grid->nRows; row++) {
      for (int column = 0; column < grid->nColumns; column++) {
        char terrain = grid->points[row][column]->terrain;
        char playerAtPoint = grid->points[row][column]->player;
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
           if (terrain == "*" ) {
            if (isVisible(player, row, column)) {
              gridString[index] = terrain;
              index++;
            }
            else if (isKown(player, row, column)) {
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
  char* gridString = mem_calloc(((grid->nRows)*(grid->nColumns + 1) + 1), sizeof(char));
  char playerAtPoint;
  char terrain;
  int index = 0;

  // Checking if the grid is NULL
  if (grid != NULL) {
    // Looping over rows and columns in the grid
    for (int row = 0; row < grid->nRows; row++) {
      for (int column = 0; column < grid->nColumns; column++) {
        // If the point contains a player, printing the player letter
        playerAtPoint = grid->points[row][column]->player;
        if (playerAtPoint != 0) {
          gridString[index] = playerAtPoint;
          index++;
        }

        // If the point does not contain a player, printing the terrain
        else {
          terrain = grid->points[row][column]->terrain;
          gridString[index] = terrain;
          index++;
        }
      }
      // Printing newline for the next row
      gridString[index] = '\n';
    }
  }
}

/**************** FUNCTION ****************/
/* see player.h for description */
player_t*
find_player(game_t* game, addr_t address)
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
/* see player.h for description */
addr_t*
add_spectator(game_t* game, addr_t* spectator)
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
  // Length of initial statement (letter and score) and max name length
  int maxLineSize = 9 + get_MaxNameLength;
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

/* see player.h for description */
void 
delete_game(game_t* game)
{
  if (game != NULL){
    // Freeing each player
    for (int i = 0; i < game->playerCount; i++) {
      player_delete(game->players[i]);
    }
    player_delete(game->spectator);

    free(game->players);
    free(game->spectator);
    free(game);
  }
}

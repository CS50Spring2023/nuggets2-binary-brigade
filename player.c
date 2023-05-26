/* 
 * player.c - CS50 'player' module
 *
 * see player.h for more information.
 *
 * Binary Brigade, Spring, 2023
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "player.h"
#include "grid.h"

/**************** global types ****************/
typedef struct player{
   
    char* port;
    char* name;
    char letter;
    int x_coord;
    int y_coord;
    int num_gold;
    bool active;
    const int numRows;
    const int numCols;
    bool** known;
    bool** visible;

} player_t;

/**************** FUNCTION ****************/
/* see player.h for description */
player_t* 
player_new(char* port, char* name, int x, int y, char letter)
{
    player_t* player = mem_malloc(sizeof(player_t));
    const int rows = getNumRows();
    const int cols = getNumCols();

    if (player == NULL) {
        return NULL;              
    
    } else {

        // initialize contents of player structure
        player->port = port;
        player->name = name;
        player->x_coord = x;
        player->y_coord = y;
        player->num_gold = 0;
        player->active = true;
        player->letter = letter;
        player->numRows = rows;
        player->numCols = cols;
        player->known = initializeBooleanArray(rows, cols);
        player->visible = initializeBooleanArray(rows, cols);
        return player;
    }
}

/* see player.h for description */
void 
player_inactive(player_t* player)
{
    player->active = false;
}

/* see player.h for description */
void 
player_delete(player_t* player)
{
    if (player != NULL){
        free(player);
    }
}

/* see player.h for description */
char
get_letter(player_t* player)
{
    if (player != NULL){
        return player->letter;
    }
    
}

/* see player.h for description */
char
get_x(player_t* player)
{
    if (player != NULL){
        return player->x_coord;
    }
    
}

/* see player.h for description */
char
get_y(player_t* player)
{
    if (player != NULL){
        return player->y_coord;
    }
    
}

char
get_gold(player_t* player)
{
    if (player != NULL){
        return player->num_gold;
    }
    
}

void
set_x(player_t* player, int x)
{
    if (player != NULL){
        player->x_coord = x;
    }
    
}

/* see player.h for description */
void
set_y(player_t* player, int y)
{
    if (player != NULL){
        player->y_coord = y;
    }
    
}

void
set_gold(player_t* player, int gold)
{
    if (player != NULL){
        player->num_gold = gold;
    }
    
}



// gettter and setter for x and y and gold.



bool
isVisible(player_t* player, const int row, const int col)
{
  if (player->visible[row][col]) {
    return true;
  }
  else {
    return false;
  }
}

bool
isKnown(player_t* player, const int row, const int col)
{
  if (player->known[row][col]) {
    return true;
  }
  else {
    return false;
  }
}

// Function to initialize a two-dimensional boolean array
static bool** 
initializeBooleanArray(const int numRows, const int numCols) 
{
  bool** array = mem_malloc(numRows * sizeof(bool*));
  if (array == NULL) {
    // Error handling if memory allocation fails
    return NULL;
  }
  for (int i = 0; i < numRows; i++) {
    array[i] = mem_malloc(numCols * sizeof(bool));
    if (array[i] == NULL) {
      // Error handling if memory allocation fails
      return NULL;
    }
    for (int j = 0; j < numCols; j++) {
      array[i][j] = false;
    }
  }
  return array;
}

void
updateVisibility(player_t* player)
{
  for (int row = 0; row < player->numRows; row++) {
    for (int col = 0; col < player->numCols; col++) {
      // point visible, make it known
      if (lineCheck(player->y_coord, player->x_coord, row, col)) {
        player->visible[row][col] = true;
        player->known[row][col] = true;
      }
      // point not visible, but can remain known
      else {
        player->visible[row][col] = false;
      }
    }
  }
}

// returns true if point is in sight, false if not
static bool 
lineCheck(const int pr, const int pc, const int row, const int col)
{
  float rowSlope;
  float colSlope;
  float currRow;
  float currCol;
  bool down;
  bool right;

  if (row >= pr) {
    down = true;
  }
  else {
    down = false;
  }
  if (col >= pc) {
    right = true;
  }
  else {
    right = false;
  }
  
  // check each row between player and point
  if (pr != row) {
    rowSlope = (col - pc)/(row - pr);
    if (down) {
      for (int r = pr + 1; r < row; r++) {
        currRow = r;
        currCol = pc + ((currRow - pr)*rowSlope);
        // col is an integer; check the intersect point
        if (floorf(currCol) == currColl) {
          if (blocksVisibility(currRow, currCol)) {
            return false;
        }
        // col not an integer; check points left and right
        else {
          if (blocksVisibility(currRow, floorf(currCol)) && blocksVisibility(currRow, floorf(currCol))) {
            return false;
          }
        }
      }
    }
    else {
      for (int r = pr - 1; r > row; r--) {
        currRow = r;
        currCol = pc + ((currRow - pr)*rowSlope);
        // col is an integer; check the intersect point
        if (floorf(currCol) == currColl) {
          if (blocksVisibility(currRow, currCol)) {
            return false;
        }
        }
        // col not an integer; check points left and right
        else {
          if (blocksVisibility(currRow, floorf(currCol)) && blocksVisibility(currRow, floorf(currCol))) {
            return false;
          }
        }
      }
    }
  }
  // check each column between player and point
  if (pc != col) {
    colSlope = (row - pr)/(col - pc);
    if (right) {
      for (int c = pc + 1; c < col; c++) {
        currCol = c;
        currRow = pr + ((currCol - pc)*colSlope);
        // row is an integer; check the intersect point
        if (floorf(currRow) == currRow) {
          if (blocksVisibility(currRow, currCol)) {
            return false;
          }
        }
        // row is not an integer; check points above and below
        else {
          if (blocksVisibility(floorf(currRow), currCol) && blocksVisibility(floorf(currRow), currCol)) {
            return false;
          }
        }
      }
    }
    else {
      for (int c = pc - 1; c > col; c--) {
        currCol = c;
        currRow = pr + ((currCol - pc)*colSlope);
        // row is an integer; check the intersect point
        if (floorf(currRow) == currRow) {
          if (blocksVisibility(currRow, currCol)) {
            return false;
          }
        }
        // row is not an integer; check points above and below
        else {
          if (blocksVisibility(floorf(currRow), currCol) && blocksVisibility(floorf(currRow), currCol)) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

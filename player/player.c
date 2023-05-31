/*
 * player.c - CS50 'player' module
 *
 * see player.h for more information.
 *
 * Binary Brigade, Spring 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "player.h"
#include "../grid/grid.h"
#include "../support/message.h"
#include "../lib/mem.h"

/**************** local functions ****************/
static bool lineCheck(const int pr, const int pc, const int row, const int col);
static bool** initializeBooleanArray(const int numRows, const int numCols);

/**************** global constants ****************/
const int maxNameLength = 50;

/**************** global types ****************/
typedef struct player {
  char* port;
  addr_t address;
  char name[50];
  char letter;
  int x_coord;
  int y_coord;
  int num_gold;
  bool active;
  int numRows;
  int numCols;
  bool** known;
  bool** visible;
} player_t;


/**************** player_new ****************/
/* see player.h for description */
player_t*
player_new(addr_t address, char* name, int x, int y, char letter)
{
  player_t* player = mem_malloc(sizeof(player_t));
  const int rows = getnRows();
  const int cols = getnColumns();
  int nameLength = strlen(name);
  
  if (player == NULL) {
    return NULL;
  
  } else {
    // initialize contents of player structure
    player->address = address;
    player->x_coord = x;
    player->y_coord = y;
    player->num_gold = 0;
    player->active = true;
    player->letter = letter;
    player->numRows = rows;
    player->numCols = cols;
    player->known = initializeBooleanArray(rows, cols);
    player->visible = initializeBooleanArray(rows, cols); 

    if (nameLength > maxNameLength) {
      name[maxNameLength] = '\0';
      nameLength = maxNameLength;
    }
    
    // replace invalid characters with underscores
    for (int i = 0; i < nameLength; i++) {
      
      if (!isgraph(name[i]) && !isblank(name[i])) {
        name[i] = '_';
      }
    }

    strcpy(player->name, name);
    return player;
  }
}


/**************** player_inactive ****************/
/* see player.h for description */
void
player_inactive(player_t* player)
{
  if(player != NULL){
    //setting position on map back to terrain
    gridpoint_t* point = getPoint(get_y(player), get_x(player));
    setPlayer(point, '0');

    player->active = false;
  }
}


/**************** player_delete ****************/
/* see player.h for description */
void
player_delete(player_t* player)
{
  if (player != NULL) {
    for (int i = 0; i < player->numRows; i++) {
      if (player->known[i] != NULL){
        mem_free(player->known[i]);
      }
      if (player->known[i] != NULL){
        mem_free(player->visible[i]);
      }
  }
  free(player->known);
  free(player->visible);
  free(player);
  }
}


/**************** get_name ****************/
/* see player.h for description */
char*
get_name(player_t* player)
{
    if (player != NULL){
        return player->name;
    } 
    return " ";
}

/**************** get_letter ****************/
/* see player.h for description */
char
get_letter(player_t* player)
{

  if (player != NULL) {
    return player->letter;
  }
  return '\0';
}

/**************** get_MaxNameLength ****************/
/* see player.h for description */
int
get_MaxNameLength()
{
  return maxNameLength;
}


/**************** get_x ****************/
/* see player.h for description */
int
get_x(player_t* player)
{
  if (player != NULL) {
    return player->x_coord;
  }
  return -1;
}


/**************** get_y ****************/
/* see player.h for description */
int
get_y(player_t* player)
{
  if (player != NULL) {
    return player->y_coord;
  }
  return -1;
}


/**************** get_gold ****************/
/* see player.h for description */
int
get_gold(player_t* player)
{
  if (player != NULL) {
    return player->num_gold;
  }
  return -1;
}


/**************** get_address ****************/
/* see player.h for description */
addr_t
get_address(player_t* player)
{
  
  return player->address;
 
}

/**************** set_letter ****************/
/* see player.h for description */
void 
set_letter(player_t* player, char letter)
{
  if (player != NULL){
    player->letter = letter;
  }
}

/**************** set_x ****************/
/* see player.h for description */
void
set_x(player_t* player, int x)
{
  if (player != NULL){
    player->x_coord = x;
  }
}


/**************** set_y ****************/
/* see player.h for description */
void
set_y(player_t* player, int y)
{
  if (player != NULL){
    player->y_coord = y;
  }
}


/**************** set_gold ****************/
/* see player.h for description */
void
set_gold(player_t* player, int gold)
{
  if (player != NULL){
    player->num_gold = gold;
  }
}

/**************** isActive ****************/
/* see player.h for description */
bool
isActive(player_t* player)
{
  if (player != NULL) {
    return player->active;
  }
  
  return false;
}

/**************** isVisible ****************/
/* see player.h for description */
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


/**************** isKnown ****************/
/* see player.h for description */
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


/**************** updateVisibility ****************/
/* see player.h for description */
void
updateVisibility(player_t* player)
{
  const int visibilityRange = 5;
  const int pr = player->y_coord;
  const int pc = player->x_coord;

  for (int row = 0; row < player->numRows; row++) {
    for (int col = 0; col < player->numCols; col++) {
      // if point is within visibility range
      if (sqrt((row - pr)*(row - pr) + (col - pc)*(col - pc)) <= visibilityRange) {
        // point visible, make it known
        if (lineCheck(player->y_coord, player->x_coord, row, col)) {
          player->visible[row][col] = true;
          player->known[row][col] = true;
      
        } 
        else {
          // point not visible, but can remain known
          player->visible[row][col] = false;
        }
      }
    }
  }
}

/**************** initializeBooleanArray ****************/
/* Allocate a 2D array of booleans, initialized to false. */
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


/**************** lineCheck ****************/
/* Given a player's row and column and the row
 * and column of a point, return true if that
 * point is visible and false if it's not.
 */
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
        // if col is an integer, check the intersect point
        // if col not an integer, check left and right
        if (blocksVisibility(currRow, floorf(currCol)) && blocksVisibility(currRow, floorf(currCol))) {
          return false;
        }     
      }
    }
    else {
      for (int r = pr - 1; r > row; r--) {
        currRow = r;
        currCol = pc + ((currRow - pr)*rowSlope);    
        // if col is an integer, check the intersect point
        // if col not an integer, check left and right
        if (blocksVisibility(currRow, floorf(currCol)) && blocksVisibility(currRow, floorf(currCol))) {
          return false;
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
        // if row is an integer, check the intersect point
        // if row not an integer, check points above and below
        if (blocksVisibility(floorf(currRow), currCol) && blocksVisibility(floorf(currRow), currCol)) {
          return false;
        } 
      }
    }
    else { 
      for (int c = pc - 1; c > col; c--) {
        currCol = c;
        currRow = pr + ((currCol - pc)*colSlope);
        // if row is an integer; check the intersect point
        // if row is not an integer; check points above and below
        if (blocksVisibility(floorf(currRow), currCol) && blocksVisibility(floorf(currRow), currCol)) {
          return false;
        }  
      }
    }
  }
  return true;
}

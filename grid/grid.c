/*
 * grid.c - Nuggets 'grid' module
 *
 * see grid.h for more information.
 *
 * Binary Brigade, Spring 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../lib/file.h"
#include "../lib/mem.h"

/**************** types ****************/

/**************** local types ****************/
typedef struct gridpoint {
  int row;
  int column;
  int nGold;
  char player;
  char terrain;
} gridpoint_t;

typedef struct grid {
    int nRows;
    int nColumns;
    gridpoint_t*** points;
} grid_t;

/**************** global variables ****************/
grid_t* grid;

/**************** global constants ****************/
int TotalGold = 250;

/**************** functions ****************/
/**************** global functions ****************/

grid_t* gridInit(char* pathName, int randomSeed);
bool blocksVisibility(const int row, const int col);
int getnRows();
int getnColumns();
void gridDelete();

/**************** local functions ****************/

static gridpoint_t* gridpointNew(int row, int column, char terrain);
static int readnColumns(FILE* map, int nRows); 
static void insertGridpoints(char* pathName);
static void generateGold(int randomSeed); 


/**************** gridInit ****************/
/* See grid.h for description. */
grid_t* 
gridInit(char* pathName, int randomSeed) 
{
  // Allocating memory for the grid, checking for NULL pointer
  grid = mem_malloc(sizeof(grid_t));

  if (grid == NULL) {
    return NULL;
  }

  // Opening the map file, checking for readability
  FILE* map = fopen(pathName, "r");

  if (map == NULL) {
    return NULL;
  }

  // Setting the number of columns and rows in the map
  grid->nRows = file_numLines(map);
  grid->nColumns = readnColumns(map, grid->nRows);

  // Closing the file (to reset line count)
  fclose(map);

    /* Allocating memory for the rows and columns in the 
    2D array to hold gridpoints */
    grid->points = mem_malloc(grid->nRows * sizeof(gridpoint_t**));

    // Allocating memory for the columns in each row
    for (int row = 0; row < grid->nRows; row++) {
        grid->points[row] = mem_malloc(grid->nColumns * sizeof(gridpoint_t*));
    }

  // Creating gridpoints
  insertGridpoints(pathName);

  // Generating the gold, inserting it into the map
  generateGold(randomSeed);

  // Returning a pointer to the initialized grid
  return grid;
}

/**************** readnColumns ****************/
/* Returns the number of columns in a given map file. */
static int 
readnColumns(FILE* map, int nRows)
{
  // Setting a variable for the number of columns
  int nColumns = 0;

  // Reading through the map to find nColumns
  for (int row = 0; row < nRows; row++) {
    // Reading the line, determining its length
    char* line = file_readLine(map);
    int length = strlen(line);

    // If the length is greater than nColumns, updating the variable
    if (nColumns < length) {
      nColumns = length;
    }

    // Freeing memory allocated for the line
    mem_free(line);
  }

  // Returning the number of columns
  return nColumns;
}

/**************** insertGridpoints ****************/
/* The function takes the pathname for a map file.
*  Upon checking the parameters,
*  the function loops through the map (rows and
*  columns), creating and inserting a gridpoint
*  struct into the 2D array belonging to the grid.
*/
static void 
insertGridpoints(char* pathName)
{
  // Opening the map file, checking for readability
  FILE* map = fopen(pathName, "r");

  if (map == NULL || grid == NULL) {
    return;
  }

    // Looping through the map, inserting grid points
    for (int row = 0; row < grid->nRows; row++) {
        for (int column = 0; column < grid->nColumns; column++) {
            char terrain = fgetc(map);
            gridpoint_t* gridpoint = gridpointNew(row, column, terrain);
            grid->points[row][column] = gridpoint;
        }
        
    // Moving to the next line
    fgetc(map);
  }

  // Closing the file
  fclose(map);
}

/**************** gridDelete ****************/
/* See grid.h for description. */
void 
gridDelete()
{
    // Only performing operations if the grid is not NULL
    if (grid != NULL) {
        // Freeing the memory allocated within the grid
        for (int row = 0; row < grid->nRows; row++) {
            for (int column = 0; column < grid->nColumns; column++) {
                // Freeing the memory for the gridpoint
                mem_free(grid->points[row][column]);
            }

            // Freeing the memory for the row
            mem_free(grid->points[row]);
        }

  // Freeing the array and the grid itself
  mem_free(grid->points);
  mem_free(grid);
  }
} 

/**************** gridpointNew ****************/
/* The functions takes in the coordinates of
 * the gridpoint (row, column) and the terrain
 * (what type of character is at the point).
 * It allocates memory for the gridpoint and 
 * sets the properties of the gridpoint
 * according to the parameters passed in. 
 * If the memory allocation is successful, it
 * returns a pointer to the gridpoint
 * (otherwise NULL).
 */
static gridpoint_t* 
gridpointNew(int row, int column, char terrain)
{
  // Allocating memory for the gridpoint
  gridpoint_t* gridpoint = mem_malloc(sizeof(gridpoint_t));

  // Checking for successful memory allocation
  if (gridpoint == NULL) {
    return NULL;
  }

  // Setting struct variables according to parameters
  gridpoint->row = row;
  gridpoint->column = column;
  gridpoint->terrain = terrain;
  gridpoint->nGold = 0;
  gridpoint->player = '0';

  // Returning the created gridpoint
  return gridpoint;
}

/**************** generateGold ****************/
/* The functions is called in the gridInit
 * method, taking in a randomSeed.
 * Depending on the randomSeed
 * input, the function creates a random seed.
 * Tracking the amount of undistributed gold,
 * it generates gold piles of varying size
 * (between 10 to 30 nuggets), before placing
 * the piles in random locations on the map
 * (if the spot is in a room).
 */
static void 
generateGold(int randomSeed)
{
  // Setting random based on the randomSeed from the server
  srand(randomSeed);

  // Generating the amount of gold in the grid
  int undistributedGold = TotalGold;

  // Setting minimum and maximum sizes for gold piles
  int minPile = 10;
  int maxPile = 30;

  // Distributing gold for as long as there is something left to distribute
  while (undistributedGold > 0) {
      // Setting a variable to store the size of current pile
      int goldPile;

      // Making sure the last pile renders the remaining gold to 0
      if (undistributedGold <= maxPile) {
          goldPile = undistributedGold;
      }

      // If there is more gold to be distributed than maxPile
      else {
          // Creating a pile of random size (between minPile and maxPile)
          goldPile = ((rand() % (maxPile - minPile + 1)) + minPile);
      }

      // Finding a point to insert the gold
      while (true) {
          // Finding a random number for the row and column for gold to be placed
          int randRow = ((rand() % (grid->nRows)));
          int randColumn = ((rand() % (grid->nColumns)));
          
          // If the random location is in a room, inserting gold into it
          if (grid->points[randRow][randColumn]->terrain == '.') {
              // If there is no gold in the spot currently
              if (grid->points[randRow][randColumn]->nGold == 0) {
                  grid->points[randRow][randColumn]->nGold = goldPile;
                  grid->points[randRow][randColumn]->terrain = '*';
              } 

              // If there is already gold in the spot, adding to gold
              else {
                  grid->points[randRow][randColumn]->nGold += goldPile;
              }

              // Updating the number of total gold to be distributed
              undistributedGold -= goldPile;

              // Exiting the loop
              break;
          }
      }
  }
}

/**************** blocksVisibility ****************/
/* See grid.h for description. */
bool 
blocksVisibility(const int row, const int col)
{
  char terrain = grid->points[row][col]->terrain;
  if (terrain == '.' || terrain == '*') {
    return false;
  }
  else {
    return true;
  }
}

/**************** getnRows ****************/
/* See grid.h for description. */
int 
getnRows()
{
  return grid->nRows;
}

/**************** getnColumns ****************/
/* See grid.h for description. */
int 
getnColumns()
{
  return grid->nColumns;
}

/**************** getPoint ****************/
/* See grid.h for description. */
gridpoint_t* 
getPoint(int row, int column)
{
  return grid->points[row][column];
}

/**************** getTerrain ****************/
/* See grid.h for description. */
char 
getTerrain(gridpoint_t* gridpoint)
{
  return gridpoint->terrain;
}

/**************** getPlayer ****************/
/* See grid.h for description. */
char
getPlayer(gridpoint_t* gridpoint)
{
  return gridpoint->player;
}

void setPlayer(gridpoint_t* gridpoint, char player)
{
  if (gridpoint != NULL) {
    gridpoint->player = player;
  }
}

void setTerrain(gridpoint_t* gridpoint, char terrain)
{
  if (gridpoint != NULL) {
    gridpoint->terrain = terrain;
  }
}

int getPointRow(gridpoint_t* gridpoint)
{
  if (gridpoint != NULL) {
    return gridpoint->row;
  }
  return -1;
}

int getPointColumn(gridpoint_t* gridpoint)
{
  if (gridpoint != NULL) {
    return gridpoint->column;
  }
  return -1;
}

int getPointGold(gridpoint_t* gridpoint)
{
  if (gridpoint != NULL) {
    return gridpoint->nGold;
  }
  return -1;
}

void setPointGold(gridpoint_t* gridpoint, int nGold)
{
  if (gridpoint != NULL) {
    gridpoint->nGold = nGold;
  }
}

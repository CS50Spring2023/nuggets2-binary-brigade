/** 
 * grid.c<
 * CS50 Spring 2023, Project
 * Date: 05/20/2023
 * Description: This program provides the grid component of the 
 *      nuggest project.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../lib/file.h"
#include "../lib/mem.h"
#include "../player/player.h"
#include "../game/game.h"

/**************** types ****************/

// Defining the gridpoint struct
typedef struct gridpoint
{
    int row;
    int column;
    int nGold;
    char player;
    char terrain;
    bool known;
    bool visible;
} gridpoint_t;

// Defining the grid struct
typedef struct grid 
{
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
void placePlayer(player_t* player);
void movePlayer(game_t* game, player_t* player, char letter);
char* gridDisplay(player_t* player);
char* gridDisplaySpectator();
int getnRows();
int getnColumns();
void gridDelete();

/**************** local functions ****************/

static int readnColumns(FILE* map, int nRows); 
static void insertGridpoints(char* pathName);
static gridpoint_t* gridpointNew(int row, int column, char terrain);
static void generateGold(int randomSeed); 
static void executeMovement(game_t* game, player_t* player, 
                    int changeRow, int changeColumn);
static void foundGold(player_t* player);
static void foundPlayer(player_t* player, game_t* game, 
                gridpoint_t* current, gridpoint_t* updated);
static bool movePossible(player_t* player, int changeRow, int changeColumn);


/**************** gridInit ****************/
/* See detailed description in grid.h. */
grid_t* gridInit(char* pathName, int randomSeed) 
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
/* See detailed description in grid.h. */
static int readnColumns(FILE* map, int nRows)
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
/* See detailed description in grid.h. */
static void insertGridpoints(char* pathName)
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
            //fprintf(stdout, "Row: %d. Column: %d. Terrain: %c\n", row, column, terrain);
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
/* See detailed description in grid.h. */
void gridDelete()
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
/* See detailed description in grid.h. */
static gridpoint_t* gridpointNew(int row, int column, char terrain)
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
    gridpoint->player = 0;

    // Returning the created gridpoint
    return gridpoint;
}

/**************** gridDisplay ****************/
/* See detailed description in grid.h. */
char* gridDisplay(player_t* player) 
{
  // Size of grid string: rows*columns, plus one newline per row
  //char[(grid->nRows)*(grid->nColumns + 1)] gridString;
  char* gridString = mem_calloc((grid->nRows)*(grid->nColumns + 1), sizeof(char));

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
                sprintf(gridString, "%c", '@');
            }
            sprintf(gridString, "%c", playerAtPoint);
          }
          else if (isKnown(player, row, column)) {
            sprintf(gridString, "%c", terrain);
          }
          else {
            sprintf(gridString, " ");
          }
        }
        // Point does not contain a player
        else {
          // Tile contains gold
           if (terrain == "*" ) {
            if (isVisible(player, row, column)) {
              sprintf(gridString, "%c", terrain);
            }
            else if (isKown(player, row, column)) {
              sprintf(gridString, ".");
            }
            else {
              sprintf(gridString, " ");
            }
          }
          // Tile does not contain gold
          else {
            if (isKnown(player, row, column)) {
              sprintf(gridString, "%c", terrain);
            }
            else {
              sprintf(gridString, " ");
            }
          }
        }     
      }
      // Printing newline for the next row
      sprintf(gridString, "\n");
    }
  }
  return gridString;
}

/**************** gridDisplaySpectator ****************/
/* See detailed description in grid.h. */
char* gridDisplaySpectator() 
{   
    // Allocating memory for the grid string
    char* gridString = mem_calloc((grid->nRows)*(grid->nColumns + 1), sizeof(char));

    // Checking if the grid is NULL
    if (grid != NULL) {
        // Looping over rows and columns in the grid
        for (int row = 0; row < grid->nRows; row++) {
            for (int column = 0; column < grid->nColumns; column++) {
                // If the point contains a player, printing the player letter
                char player = grid->points[row][column]->player;
                if (player != 0) {
                    sprintf(gridString, "%c", player);
                }

                // If the point does not contain a player, printing the terrain
                else {
                    char terrain = grid->points[row][column]->terrain;
                    sprintf(gridString, "%c", terrain);
                }

            }

            // Printing newline for the next row
            sprintf(gridString, "\n");
        }
    }
}

/**************** generateGold ****************/
/* See detailed description in grid.h. */
static void generateGold(int randomSeed)
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

/**************** movePlayer ****************/
/* See detailed description in grid.h. */
void movePlayer(game_t* game, player_t* player, char letter) 
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

/**************** executeMovement ****************/
/* See detailed description in grid.h. */
static void executeMovement(game_t* game, player_t* player, 
                    int changeRow, int changeColumn)
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
/* See detailed description in grid.h. */
static void foundGold(player_t* player)
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
/* See detailed description in grid.h. */
static void foundPlayer(player_t* player, game_t* game, 
                gridpoint_t* current, gridpoint_t* updated)
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

/**************** movePossible ****************/
/* See detailed description in grid.h. */
static bool movePossible(player_t* player, int changeRow, int changeColumn) 
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

/**************** placePlayer ****************/
/* See detailed description in grid.h. */
void placePlayer(player_t* player)
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

/**************** blockVisibility ****************/
/* See detailed description in grid.h. */
static bool blocksVisibility(const int row, const int col)
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
/* See detailed description in grid.h. */
int getnRows()
{
    return grid->nRows;
}

/**************** getnColumns ****************/
/* See detailed description in grid.h. */
int getnColumns()
{
    return grid->nColumns;
}
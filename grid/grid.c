/** 
 * grid.c
 * Author: Louis Weisdorf
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
    gridpoint_t** points;
} grid_t;

typedef struct player
{   
    char* port;
    char* name;
    char letter;
    int x_coord;
    int y_coord;
    int num_gold;
    bool active;

} player_t;

typedef struct game
{   
    grid_t* grid;
    int nGoldTotal;
    int nGoldRemaining;
    player_t** players;
} game_t;

/**************** global variables ****************/
grid_t* grid;

/**************** functions ****************/

grid_t* gridInit(char* pathName, int randomSeed);
int readnColumns(FILE* map, int nRows); 
void insertGridpoints(char* pathName);
void gridDelete(grid_t* grid);
gridpoint_t* gridpointNew(int row, int column, char terrain);
void gridDisplay(grid_t* grid);
void generateGold(int randomSeed); 
void movePlayer(player_t* player, char letter);
void foundGold(player_t* player);
void foundPlayer(player_t* player, game_t* game, gridpoint_t current, gridpoint_t new);
bool movePossible(player_t* player, int changeRow, int changeColumn);
void placePlayer(player_t* player);

int main(const int argc, char *argv[]) 
{
    grid_t* grid = gridInit("../maps/main.txt", 2);
    gridDisplay(grid);
    gridDelete(grid);
}

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
    grid->points = mem_malloc(grid->nRows * sizeof(gridpoint_t*));

    // Allocating memory for the columns in each row
    for (int row = 0; row < grid->nRows; row++) {
        grid->points[row] = mem_malloc(grid->nColumns * sizeof(gridpoint_t));
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
int readnColumns(FILE* map, int nRows)
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
void insertGridpoints(char* pathName)
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
            grid->points[row][column] = *gridpoint;
            mem_free(gridpoint);
        }
        
        // Moving to the next line
        fgetc(map);
    }

    // Closing the file
    fclose(map);
}

/**************** gridDelete ****************/
/* See detailed description in grid.h. */
void gridDelete(grid_t* grid)
{
    // Only performing operations if the grid is not NULL
    if (grid != NULL) {
        // Freeing the memory allocated for each row and column
        for (int row = 0; row < grid->nRows; row++) {
            mem_free(grid->points[row]);
        }

        // Freeing the array and the grid itself
        mem_free(grid->points);
        mem_free(grid);
    }
} 

/**************** gridpointNew ****************/
/* See detailed description in grid.h. */
gridpoint_t* gridpointNew(int row, int column, char terrain)
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
    gridpoint->player = NULL;

    // Returning the created gridpoint
    return gridpoint;
}

/**************** gridDisplay ****************/
/* See detailed description in grid.h. */
void gridDisplay(grid_t* grid) 
{
    // Checking if the grid is NULL
    if (grid != NULL) {
        // Looping over rows and columns in the grid
        for (int row = 0; row < grid->nRows; row++) {
            for (int column = 0; column < grid->nColumns; column++) {
                // If the point contains a player, printing the player letter
                char player = grid->points[row][column].player;
                if (player != NULL) {
                    fprintf(stdout, "%c", player);
                }

                // If the point does not contain a player, printing the terrain
                else {
                    char terrain = grid->points[row][column].terrain;
                    fprintf(stdout, "%c", terrain);
                }
                
            }

            // Printing newline for the next row
            fprintf(stdout, "\n");
        }
    }
}

/**************** generateGold ****************/
/* See detailed description in grid.h. */
void generateGold(int randomSeed)
{
    // If a randomSeed has not been passed in
    if (randomSeed < 0) {
        srand(getpid());
    }

    // If a randomSeed has been passed in
    else {
        srand(randomSeed);
    }

    // Generating the amount of gold in the grid
    // APPLY CHANGES HERE WHEN GAME MODULE IS FUNCTIONAL
    int undistributedGold = 250;

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
            if (grid->points[randRow][randColumn].terrain == '.') {
                // If there is no gold in the spot currently
                if (grid->points[randRow][randColumn].nGold == 0) {
                    grid->points[randRow][randColumn].nGold = goldPile;
                    grid->points[randRow][randColumn].terrain = '*';
                } 

                // If there is already gold in the spot, adding to gold
                else {
                    grid->points[randRow][randColumn].nGold += goldPile;
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
void movePlayer(player_t* player, char letter) 
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

    // Current location of the player
    gridpoint_t current = grid->points[player->x_coord][player->y_coord];

    // Potential new location of the player
    gridpoint_t new = grid->points[current.row + changeRow][current.column + changeColumn];

    // Checking if the move can be made
    if (movePossible(player, changeRow, changeColumn)) {
        // Checking if the move causes the player to step into another player


        // Updating the location of the player
        player->x_coord += changeColumn;
        player->y_coord += changeRow;

        // Updating the contents of the gridpoints
        new.player = player->letter;
        current.player = NULL;

        // If the player did not come from a passage
        if (current.terrain != '#') {
            // Resetting the terrain to be an empty room spot
            new.terrain = '.';
        }

        // Checking if the move causes the player to find gold
        foundGold(player);
    }
}


/**************** foundGold ****************/
/* See detailed description in grid.h. */
void foundGold(player_t* player)
{
    // Setting a variable for the gridpoint
    gridpoint_t gridpoint = grid->points[player->x_coord][player->y_coord];

    // If there is a gold pile in the players location
    if (gridpoint.terrain == '*') {
        // Assign gold from point to player
        player->num_gold += gridpoint.nGold;
        gridpoint.nGold = 0;

        // Updating the terrain of the point
        gridpoint.terrain = '.';
    }
}

/**************** foundPlayer ****************/
/* See detailed description in grid.h. */
void foundPlayer(player_t* player, game_t* game, gridpoint_t current, gridpoint_t new)
{
    // If there is a player in the new location
    if (new.player != NULL) {
        // Switching the player letters
    }

    // If there is a player in the new location
        // Loop through the players in the game struct
            // If the coordinates match
                // Set the coordinates to be those of current

}

/**************** movePossible ****************/
/* See detailed description in grid.h. */
bool movePossible(player_t* player, int changeRow, int changeColumn) 
{   
    // Current location of the player
    gridpoint_t current = grid->points[player->x_coord][player->y_coord];

    // New (potential) location of the player 
    gridpoint_t new = grid->points[current.row + changeRow][current.column + changeColumn];

    /* If the new position is either a spot in a room, passage, 
    contains gold or another player */
    if ((new.terrain == '.') || (new.terrain == '#') || (new.terrain == '*') 
        || (isalpha(new.terrain))) {
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

        gridpoint_t randomPoint = grid->points[randRow][randColumn];
                
        /* If the random location is in a room or passage, and if there is not
        already a player in the spot */
        if (((randomPoint.terrain == '.') || (randomPoint.terrain == '#')) &&
            randomPoint.player == NULL) {
            // Inserting the player into random point
            randomPoint.player = player->letter;

            // Adding the location to the player
            player->x_coord = randomPoint.column;
            player->y_coord = randomPoint.row;

            // Exiting the loop
            break;
        }
    }
}

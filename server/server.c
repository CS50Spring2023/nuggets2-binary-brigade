/* 
 * server.c - CS50 'server' module
 *
 * see server.h for more information.
 *
 * Binary Brigade, Spring, 2023
 */

#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "../support/message.h"
#include "../grid/grid.h"
#include "../game/game.h"

int main(int argc, char *argv[])
{
  if (argc != 2 || argc != 3){
    fprintf(stderr, "invalid number of arguments -- must have either 1 or 2 arguments (mapfile and randomSeed)\n");
    return 1;
  }

  FILE *fp;
  fp = fopen(argv[1], "r");
  if (fp == NULL){
    fprintf(stderr, "Map txt file is not a readable file");
    return 1;
  }
  close(fp);

  int randomSeed;
  if (argc == 3){
    randomSeed = atoi(argv[2]);
  } else {
    randomSeed = getpid();
  }

  grid_t* grid = gridInit(argv[1], randomSeed);
  game_t* game = initialize_game(grid);


  int portNumber = message_init(NULL);
  if (portNumber == 0){
    fprintf(stderr, "Error initializing network");
    return 1;
  }

  return 0;
}
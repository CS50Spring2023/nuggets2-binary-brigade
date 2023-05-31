/* 
 * Nuggets server.c file
 * 
 * Creates a server for the Nuggets game
 * and allows clients to connect and send
 * inputs, and returns the display and 
 * messages to them. Server handles all of
 * the game logic, including everything in
 * the game, grid, and player modules.
 *
 * Binary Brigade, Spring, 2023
 */

#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../support/message.h"
#include "../grid/grid.h"
#include "../game/game.h"
#include "../player/player.h"
#include "../lib/mem.h"
#include "../support/log.h"

/**************** local global types ****************/
static const int maxPlayers = 26;

/**************** file-local functions ****************/

static bool handleMessage(void* arg, const addr_t from, const char* message);
static void goldUpdate(addr_t address, player_t* player, int collected);
static void spectatorGoldUpdate(addr_t address);

/***************** main *******************************/
int 
main(int argc, char *argv[])
{ 
  
  if (argc != 2 && argc != 3){
    fprintf(stderr, "invalid number of arguments -- must have either 1 or 2 arguments (mapfile and randomSeed)\n");
    return 1;
  }

  FILE *fp;
  fp = fopen(argv[1], "r");
  
  if (fp == NULL){
    fprintf(stderr, "Map txt file is not a readable file\n");
    return 2;
  }
  fclose(fp);

  int randomSeed;
  
  if (argc == 3){
    randomSeed = atoi(argv[2]);
  
  } else {
    randomSeed = getpid();
  }

  grid_t* grid = gridInit(argv[1], randomSeed);

  initialize_game(grid);

  // initialize the message module (without logging)
  int myPort = message_init(NULL);

  if (myPort == 0) {
    return 3; // failure to initialize message module
  } else {
    printf("Ready to play, waiting at port %d\n", myPort);
  }

  bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

  // shut down the message module
  message_done();
  delete_game();
  gridDelete();
  
  return ok? 0 : 4; // status code depends on result of message_loop
}

/**************** handleMessage ****************/
/* Datagram received; print it, read a line from stdin, and use it as reply.
 * We ignore 'arg' here.
 * Return true if EOF on stdin or any fatal error.
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
  // print the message and a prompt
  printf("'%s'\n", message);

  //client has input play
  if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
    char name[strlen(message) - 5];
    
    strcpy(name, message + 5);

    if (strlen(name) == 0){
      //sending message to client that name is empty
      message_send(from, "QUIT Sorry - you must provide player's name.");
    
    } else {
      
      char letter = ' ';
      player_t* player = player_new(from, name, 0, 0, letter);
      
      if (add_player(player) != 0){

        message_send(from, "QUIT Game is full: no more players can join.\n");
      
      } else {
        placePlayer(player);
        char letter = get_letter(player);
        
        char* line = mem_malloc(sizeof(char)*5);
        strcpy(line, "OK ");
        int temp = strlen(line);
        line[temp] = letter;
        line[temp+1] = '\0';

        message_send(from, line); //sending ok and letter of player
        mem_free(line);

        //sending grid dimensions, gold update, and display
        get_grid_dimensions(from);
        goldUpdate(from, player, 0);
        gridDisplay(from, player);

        player_t** players = get_players();
        for (int i = 0; i < maxPlayers; i++) {
          if (players[i] != NULL && !message_eqAddr(from, get_address(players[i]))){
            gridDisplay(get_address(players[i]), players[i]);  //sends display update to all players
          }
        }
        if (message_isAddr(get_spectator())){
          gridDisplaySpectator(get_spectator());  //sends display update to spectator
        }
      }
    }
  
  //client has input spectate
  } else if (strcmp(message, "SPECTATE") == 0) {
    addr_t oldSpectator = add_spectator(from);
    
    if (message_isAddr(oldSpectator)){
      //sending a message to the old spectator that they have been replaced
      message_send(oldSpectator, "QUIT You have been replaced by a new spectator.");
    }

    //sending grid dimensions, gold update, and display
    get_grid_dimensions(from);
    spectatorGoldUpdate(from);
    gridDisplaySpectator(from);
  
  //client has input a keystroke
  } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
    //extract key command
    char key[strlen(message) - 4];
    strcpy(key, message + 4);
    
    printf("this is key: %s\n", key);
    fflush(stdout);
    printf("this is message: %s\n", message);
    fflush(stdout);

    if (strcmp(key, "Q") == 0) {
      if (find_player(from) != NULL){
        player_t* player = find_player(from);
        player_inactive(player);
        message_send(from, "QUIT Thanks for playing!");
      } else {
        add_spectator(message_noAddr());
        message_send(from, "QUIT Thanks for watching!");
      }
    } else if (!message_eqAddr(from, get_spectator())){
      if (find_player(from) != NULL){
        player_t* player = find_player(from);

        //getting prev info about gold and position
        int prevGold = get_gold(player);
        int prevAvailable = get_available_gold();
        int prevX = get_x(player);
        int prevY = get_y(player);

        movePlayer(player, *key);

        //getting updated info about gold and position
        int newGold = get_gold(player);
        int newAvailable = get_available_gold();
        int newX = get_x(player);
        int newY = get_y(player);

        //comparing to see if messages needs to be sent
        if (newGold != prevGold || prevAvailable != newAvailable){
          goldUpdate(from, player, newGold-prevGold);

          player_t** players = get_players();
          for (int i = 0; i < maxPlayers; i++) {
            if (players[i] != NULL && !message_eqAddr(from, get_address(players[i]))){
              goldUpdate(get_address(players[i]), players[i], 0); //sends game gold update to all
            }
          }
          spectatorGoldUpdate(get_spectator());
        }
        if (prevX != newX || prevY != newY){
          player_t** players = get_players();
          for (int i = 0; i < maxPlayers; i++) {
            if (players[i] != NULL){
              gridDisplay(get_address(players[i]), players[i]); //sends game gold update to all players
            }
          }
          if (message_isAddr(get_spectator())){
            gridDisplaySpectator(get_spectator());
          }
        }
      }
    }
  }
  
  if (get_available_gold() == 0){    //game is over
    player_t** players = get_players();
    for (int i = 0; i < maxPlayers; i++) {
      if (players[i] != NULL){
        game_summary(get_address(players[i])); //sends game summary to all players
      }
    }

    addr_t address = get_spectator();
    if (message_isAddr(address)){
      game_summary(address); //sends game summary to a spectator if it exists
    }
    return true;
  }
  // normal case: keep looping
  return false;
}

/**************** goldUpdate ****************/
/* 
 * Formats a goldUpdate correctly for each player using helper functions
 * 
 * Caller provides:
 *   game, player, and collected
 * We return:
 *   char* update of gold
 */
static void
goldUpdate(addr_t address, player_t* player, int collected) {
  int n = collected;
  int p = get_gold(player);
  int r = get_available_gold();
  
  char update[100];
  sprintf(update, "GOLD %d %d %d", n, p, r);
  
  message_send(address, update);
}

/**************** spectatorGoldUpdate ****************/
/* 
 * Formats a goldUpdate correctly for a spectator using a helper function
 * 
 * Caller provides:
 *   game
 * We return:
 *   char* update of gold
 */
static void
spectatorGoldUpdate(addr_t address) {
  int n = 0;
  int p = 0;
  int r = get_available_gold();

  char update[100];
  sprintf(update, "GOLD %d %d %d", n, p, r);
  
  message_send(address, update);
}

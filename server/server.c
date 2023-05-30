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
static char* goldUpdate(player_t* player, int collected);
static char* spectatorGoldUpdate();
static char* getDisplay(player_t* player);
static char* getDisplaySpectator();

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
    
    printf("This is the new name %s\n", name);

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
        message_send(from, get_grid_dimensions());
        message_send(from, goldUpdate(player, 0));
        message_send(from, getDisplay(player));

        player_t** players = get_players();
        for (int i = 0; i < maxPlayers; i++) {
          if (players[i] != NULL && !message_eqAddr(from, get_address(player))){
            message_send(get_address(players[i]), getDisplay(players[i])); //sends game gold update to all players
          }
        }
        message_send(get_spectator(), getDisplaySpectator());
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
    message_send(from, get_grid_dimensions());
    message_send(from, spectatorGoldUpdate());
    message_send(from, getDisplaySpectator());
  
  //client has input a keystroke
  } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
    //extract key command
    char key[strlen(message) - 4];
    strcpy(key, message + 4);

    printf("this is key: %s\n", key);
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
          message_send(from, goldUpdate(player, newGold-prevGold));

          player_t** players = get_players();
          for (int i = 0; i < maxPlayers; i++) {
            if (players[i] != NULL && !message_eqAddr(from, get_address(player))){
              message_send(get_address(players[i]), 0); //sends game gold update to all
            }
          }
          message_send(get_spectator(), spectatorGoldUpdate());
        }
        if (prevX != newX || prevY != newY){
          player_t** players = get_players();
          for (int i = 0; i < maxPlayers; i++) {
            if (players[i] != NULL){
              message_send(get_address(players[i]), getDisplay(players[i])); //sends game gold update to all players
            }
          }
          message_send(get_spectator(), getDisplaySpectator());
        }
      }
    }
  }
  
  if (get_available_gold() == 0){    //game is over
    char* summary = game_summary();
    
    player_t** players = get_players();
    for (int i = 0; i < maxPlayers; i++) {
      if (players[i] != NULL){
        message_send(get_address(players[i]), summary); //sends game summary to all players
      }
    }

    addr_t address = get_spectator();
    if (message_isAddr(address)){
      message_send(address, summary); //sends game summary to a spectator if it exists
    }
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
static char*
goldUpdate(player_t* player, int collected) {
  int n = collected;
  int p = get_gold(player);
  int r = get_available_gold();

  char* stringLength = mem_malloc(sizeof(char) * 3);
  sprintf(stringLength, "%d%d%d", n, p, r);
  int outputLength = strlen("GOLD   ") + strlen(stringLength);
  
  char *update = malloc(sizeof(char) * outputLength);
  sprintf(update, "GOLD %d %d %d", n, p, r);
  free(stringLength);

  return update;
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
static char*
spectatorGoldUpdate() {
  int n = 0;
  int p = 0;
  int r = get_available_gold();

  char* stringLength = mem_malloc(sizeof(char) * 3);
  sprintf(stringLength, "%d%d%d", n, p, r);
  int outputLength = strlen("GOLD   ") + strlen(stringLength);
  
  char *update = malloc(sizeof(char) * outputLength);
  sprintf(update, "GOLD %d %d %d", n, p, r);
  free(stringLength);

  return update;
}

/**************** getDisplay ****************/
/* 
 * Formats a display update for client
 * 
 * Caller provides:
 *   player
 * We return:
 *   char* update of display
 */
static char*
getDisplay(player_t* player) {
  char* display = gridDisplay(player);
  int outputLength = strlen("DISPLAY\n") + strlen(display) + 1;
  char* updateDisplay = malloc(sizeof(char) * outputLength);
  
  sprintf(updateDisplay, "DISPLAY\n%s", display);
  free(display);

  return updateDisplay;
}

/**************** getSpectatorDisplay ****************/
/* 
 * Formats a display update for spectator client
 * 
 * Caller provides:
 *   player
 * We return:
 *   char* update of display
 */
static char*
getDisplaySpectator() {
  char* display = gridDisplaySpectator();

  int outputLength = strlen("DISPLAY\n") + strlen(display) + 1;
  char* updateDisplay = malloc(sizeof(char) * outputLength);

  strcpy(updateDisplay, "DISPLAY\n");
  strcat(updateDisplay, display);
  free(display);

  return updateDisplay;
}
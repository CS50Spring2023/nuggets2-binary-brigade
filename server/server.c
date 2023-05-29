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

/**************** local global types ****************/
static const int maxPlayers = 26;

/**************** global variable ****************/
game_t* game;

/**************** file-local functions ****************/

static bool handleMessage(void* arg, const addr_t from, const char* message);
static char* goldUpdate(game_t* game, player_t* player, int collected);
static char* spectatorGoldUpdate(game_t* game);
static char* getDisplay(player_t* player);
static char* getDisplaySpectator();

/***************** main *******************************/
int 
main(int argc, char *argv[])
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
  fclose(fp);

  int randomSeed;
  if (argc == 3){
    randomSeed = atoi(argv[2]);
  } else {
    randomSeed = getpid();
  }

  grid_t* grid = gridInit(argv[1], randomSeed);
  // game_t* game = initialize_game(grid);
  initialize_game(grid);

  // initialize the message module (without logging)
  int myPort = message_init(NULL);
  if (myPort == 0) {
    return 2; // failure to initialize message module
  } else {
    printf("serverPort=%d\n", myPort);
  }

  fprintf(stdout, "Ready to play, waiting at port '%d'", myPort);

  bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

  // shut down the message module
  message_done();
  delete_game();
  gridDelete();
  
  return ok? 0 : 1; // status code depends on result of message_loop
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
  printf("> ");
  fflush(stdout);

  // allocate a buffer into which we can read a line of input
  // (it can't be any longer than a message)!
  char line[message_MaxBytes];

  // read a line from stdin: this is VERY unusual for a handleMessage()
  if (fgets(line, message_MaxBytes, stdin) == NULL) {
    // EOF case: stop looping
    putchar('\n');
    return true;
  } else {
    // strip trailing newline
    const int len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
      line[len-1] = '\0';
    }
    //client has input play
    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
      char name[strlen(message) - 5];
      strncpy(name, message + 5, strlen(message) - 5);
      if (strlen(name) == 0){
        //sending message to client that name is empty
        message_send(from, "QUIT Sorry - you must provide player's name.");
      }
      else {
        char* letter = "";
        player_t* player = player_new(from, name, 0, 0, *letter);
        placePlayer(player);
        if (add_player(game, player) != 0){
          message_send(from, "QUIT Game is full: no more players can join.");
        } else {
          char letter = get_letter(player);
          strcat(line, strcat("OK ", &letter));
          message_send(from, line); //sending ok and letter of player

          //sending grid dimensions, gold update, and display
          message_send(from, get_grid_dimensions(game));
          message_send(from, goldUpdate(game, player, 0));
          message_send(from, getDisplay(player));
        }
      }
    //client has input spectate
    } if (strcmp(message, "SPECTATE") == 0) {
      addr_t* oldSpectator = add_spectator(game, &from);
      if (oldSpectator != NULL){
        //sending a message to the old spectator that they have been replaced
        message_send(*oldSpectator, "QUIT You have been replaced by a new spectator.");
      } else {
      //sending grid dimensions, gold update, and display
      message_send(from, get_grid_dimensions(game));
      message_send(from, spectatorGoldUpdate(game));
      message_send(from, getDisplaySpectator());
      }
    //client has input a keystroke
    } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
      const char* key = message + strlen("KEY ");
      player_t* player = find_player(game, from);
      int prevGold = get_gold(player);
      movePlayer(game, player, *key);
      int currGold = get_gold(player);
      message_send(from, goldUpdate(game, player, currGold-prevGold));
    } else if (strcmp(line, "Q") == 0) {
      if (find_player(game, from) != NULL){
        player_t* player = find_player(game, from);
        player_inactive(player);
        message_send(from, "QUIT Thanks for playing!");
      } else {
        add_spectator(game, NULL);
        message_send(from, "QUIT Thanks for watching!");
      }
    } else if (get_available_gold(game) == 0){    //game is over
      char* summary = game_summary(game);
      
      player_t** players = get_players(game);
      for (int i = 0; i < maxPlayers; i++) {
        if (players[i] != NULL){
          message_send(get_address(players[i]), summary); //sends game summary to all players
        }
      }

      addr_t* address = get_spectator(game);
      if (address != NULL){
        message_send(*address, summary); //sends game summary to a spectator if it exists
      }
    }

    // normal case: keep looping
    return false;
  }
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
goldUpdate(game_t* game, player_t* player, int collected){
  int n = collected;
  int p = get_gold(player);
  int r = get_available_gold(game);

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
spectatorGoldUpdate(game_t* game){
  int n = 0;
  int p = 0;
  int r = get_available_gold(game);

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
getDisplay(player_t* player){
  char* display = gridDisplay(player);
  int outputLength = strlen("DISPLAY\n") + strlen(display) + 1;
  char* updateDisplay = malloc(sizeof(char) * outputLength);
  updateDisplay = strcat("DISPLAY\n", display);
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
getDisplaySpectator(){
  char* display = gridDisplaySpectator();
  int outputLength = strlen("DISPLAY\n") + strlen(display) + 1;
  char* updateDisplay = malloc(sizeof(char) * outputLength);
  updateDisplay = strcat("DISPLAY\n", display);
  free(display);

  return updateDisplay;
}
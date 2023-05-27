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
#include "../../shared/tse/libcs50.a"
#include "../support/message.h"
#include "../grid/grid.h"
#include "../game/game.h"

/**************** local global types ****************/
static const int maxPlayers = 27;
static hashtable_t* playersHash;
static game_t* game;

/**************** file-local functions ****************/

static bool handleMessage(void* arg, const addr_t from, const char* message);

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
  close(fp);

  int randomSeed;
  if (argc == 3){
    randomSeed = atoi(argv[2]);
  } else {
    randomSeed = getpid();
  }

  grid_t* grid = gridInit(argv[1], randomSeed);
  game_t* game = initialize_game(grid);
  playersHash = hashtable_new(27); 

  // initialize the message module (without logging)
  int myPort = message_init(NULL);
  if (myPort == 0) {
    return 2; // failure to initialize message module
  } else {
    printf("serverPort=%d\n", myPort);
  }

  bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

  // shut down the message module
  message_done();
  
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

    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
      const char* name = message + strlen("PLAY ");
      if (strlen(name) == 0){
        message_send(from, "QUIT Sorry - you must provide player's name.");
      }
      else {
        //need to find port
        player_t* player = player_new("port", name, 0, 0, "");
        place_player(player);
        if (add_player(game, player) != 0){
          message_send(from, "QUIT Game is full: no more players can join.");
        } else {
          //hashtable_insert(playersHash, from, player);
          strcat(line, strcat("OK ", get_letter(player)));
          message_send(from, get_grid_dimensions(game));
          message_send(from, goldUpdate(game, player, 0));
          message_send(from, getDisplay(player));
        }
      }
    } if (strcmp(message, "SPECTATE") == 0) {
      addr_t* oldSpectator = add_spectator(game, from);
      if (oldSpectator != NULL){
        //oldSpectator is a port number -- how do I get addr_t
        message_send(oldSpectator, "QUIT Game is full: another spectator has joined.")
      } else {
      message_send(from, get_grid_dimensions(game));
      message_send(from, spectatorGoldUpdate(game));
      message_send(from, gridDisplaySpectator());
      }
  
    } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
      const char* key = message + strlen("KEY ");
      game
      //player_t* player = hashtable_find(playersHash, from);
      int prevGold = get_gold(player);
      movePlayer(game, player, key);
      int currGold = get_gold(player);
      message_send(from, goldUpdate(game, player, currGold-prevGold));
    } else if (line == "Q") {
      player_t* player = hashtable_find(playersHash, from);
      player_inactive(player);
      //send quit message
    }

    // normal case: keep looping
    return false;
  }
}

static char*
goldUpdate(game_t* game, player_t* player, int collected){
  int n = collected;
  int p = get_gold(player);
  int r = get_available_gold(game);

  int outputLength = strlen("GOLD   ") + strlen(itoa(n)) + strlen(itoa(p)) strlen(itoa(r));
  char *update = malloc(sizeof(char) * outputLength);
  update = "GOLD "+itoa(n)+" "+itoa(p)+" "+itoa(r); 

  return update;
}

static char*
spectatorGoldUpdate(game_t* game){
  int n = 0;
  int p = 0;
  int r = get_available_gold(game);

  int outputLength = strlen("GOLD   ") + strlen(itoa(n)) + strlen(itoa(p)) strlen(itoa(r));
  char *update = malloc(sizeof(char) * outputLength);
  update = "GOLD "+itoa(n)+" "+itoa(p)+" "+itoa(r); 

  return update;
}

static char*
getDisplay(player_t* player){
  char* display = gridDisplay(player);
  int outputLength = strlen("DISPLAY\n") + strlen(display) + 1;
  char *update = malloc(sizeof(char) * outputLength);
  char* updateDisplay = "DISPLAY\n"+display;

  return updateDisplay;
}
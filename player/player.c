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

/**************** global types ****************/
typedef struct player{
   
    char* port;
    char* name;
    int x_coord;
    int y_coord;
    int num_gold;
    bool active;

} player_t;

/**************** FUNCTION ****************/
/* see player.h for description */
player_t* 
player_new(char* port, char* name, int x, int y)
{
    player_t* player = mem_malloc(sizeof(player_t));

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
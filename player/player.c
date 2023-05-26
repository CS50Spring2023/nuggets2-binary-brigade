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
    char letter;
    int x_coord;
    int y_coord;
    int num_gold;
    bool active;

} player_t;

/**************** FUNCTION ****************/
/* see player.h for description */
player_t* 
player_new(char* port, char* name, int x, int y, char letter)
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
        player->letter = letter;
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

/* see player.h for description */
char
get_letter(player_t* player)
{
    if (player != NULL){
        return player->letter;
    }
    
}

/* see player.h for description */
char
get_x(player_t* player)
{
    if (player != NULL){
        return player->x_coord;
    }
    
}

/* see player.h for description */
char
get_y(player_t* player)
{
    if (player != NULL){
        return player->y_coord;
    }
    
}

/* see player.h for description */
char
get_gold(player_t* player)
{
    if (player != NULL){
        return player->num_gold;
    }
    
}

/* see player.h for description */
void
set_x(player_t* player, int x)
{
    if (player != NULL){
        player->x_coord = x;
    }
    
}

/* see player.h for description */
void
set_y(player_t* player, int y)
{
    if (player != NULL){
        player->y_coord = y;
    }
    
}

/* see player.h for description */
void
set_gold(player_t* player, int gold)
{
    if (player != NULL){
        player->num_gold = gold;
    }
    
}
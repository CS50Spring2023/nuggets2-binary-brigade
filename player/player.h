/* 
 * player.h - header file for CS50 player module
 * 
 * Binary Brigade, Spring, 2023
 */


/**************** global types ****************/
typedef struct player player_t;  // opaque to users of the module

/**************** FUNCTION ****************/
/* Create a new player structure w/ given parameters
 *
 * We return:
 *   pointer to a new playeryer; NULL if error (out of memory).
 */
player_t* player_new(char* port, char* letter, int x, int y);

/* Take in a pointer to a player and makes it inactive
 *
 * We return:
 *   nothing
 */
void player_inactive(player_t* player);

/* Take in a pointer to a player and deletes it + frees up the space
 *
 * We return:
 *   nothing
 */
void player_delete(player_t* player);
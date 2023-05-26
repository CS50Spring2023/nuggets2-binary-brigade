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
player_t* player_new(char* port, char* name, int x, int y, char letter);

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

/* Take in a pointer to a player 
 *
 * We return:
 *   player's letter
 */
char get_letter(player_t* player);

/* Take in a pointer to a player 
 *
 * We return:
 *   player's x
 */
char get_x(player_t* player);

/* Take in a pointer to a player 
 *
 * We return:
 *   player's y
 */
char get_y(player_t* player);

/* Take in a pointer to a player 
 *
 * We return:
 *   player's gold
 */
char get_gold(player_t* player);

/* Take in a pointer to a player and change player's x
 *
 * We return:
 *   nothing
 */
void set_x(player_t* player, int x);

/* Take in a pointer to a player and change player's y
 *
 * We return:
 *   nothing
 */
void set_y(player_t* player, int y); 

/* Take in a pointer to a player and change player's gold
 *
 * We return:
 *   nothing
 */
void set_gold(player_t* player, int gold);

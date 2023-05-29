/*
 * player.h - header file for CS50 player module
 *
 * Binary Brigade, Spring, 2023
 */

#include "../support/message.h"

// /**************** global types ****************/
typedef struct player player_t;  // opaque to users of the module

/**************** FUNCTION ****************/


/* Create a new player structure w/ given parameters
 *
 * We return:
 *   pointer to a new playeryer; NULL if error (out of memory).
 */
player_t* player_new(addr_t address, char* name, int x, int y, char letter);


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
 *   player's name
 */
char* get_name(player_t* player);

/* Take in a pointer to a player 
 *
 * We return:
 *   player's letter
 */
char get_letter(player_t* player);

/*  
 *
 * We return:
 *   MaxNameLength global variable.
 */
int get_MaxNameLength();


/* Take in a pointer to a player 
 *
 * We return:
 *   player's x
 */
int get_x(player_t* player);


/* Take in a pointer to a player
 *
 * We return:
 *   player's y
 */
int get_y(player_t* player);


/* Take in a pointer to a player
 *
 * We return:
 *   player's gold
 */
int get_gold(player_t* player);


/* Take in a pointer to a player
 *
 * We return:
 *   player's address
 */
addr_t get_address(player_t* player);

/* Take in a pointer to a player and change player's letter
 *
 * We return:
 *   nothing
 */
void set_letter(player_t* player, char letter);

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


/* Given a row and col, tell if the player currently sees
 * that point.
 *
 * We return:
 *   true if point is visible, false if not
 */
bool isVisible(player_t* player, const int row, const int col);


/* Given a row and col, tell if the player knows (has ever
 * seen) that point.
 *
 * We return:
 *   true if point is known, false if not
 */
bool isKnown(player_t* player, const int row, const int col);


/* Updates visibility by changing the values of the known
 * and visible boolean arrays.
 *
 * We return:
 *   nothing
 */
void updateVisibility(player_t* player);
# CS50 Nuggets
## Implementation Spec
### Binary Brigade, Spring, 2023

In this document we reference the Requirements Specification and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.

For each, client and server, we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan
-  Exit Codes

#### Plan for division of labor

- Charlie: Server and game
- Kashan: Client
- Louis: Grid
- Nick: Player/Visibility

### Client

#### Data structures 

We use one main data structure, `client_info`, that is created inside the client program itself. 

#### Control flow

The Client is implemented in one file `client.c`, with 11 functions.

##### main

The `main` function does the following:
    
        calls `parseArgs` to validate the arguments
        initializes the `client_info` data structure
        calls initDisplay() to initialize the display
        calls setup_server() to initialize the network and join the game
        calls message_loop() with handleTimeout(), handleInput() and handleMessage() 
        calls message_done() and endwin()
        free necessary memory space
        exit
        

##### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* there are only 2 or 3 arguments passed in
* the port is a number

##### handleInput

Takes in message (char) from the player and send it to the the server with the right format. 

    ensure that handleInput isn't called with arg= NULL
	check if handleInput is called with a correspondent
    read a single character using getch()
	ensure it is not a newline character
        if so return false
    format the message by adding "KEY" to the start of it
    use sprintf to make sure message doesn't go over the message_MasBytes
    send message to server
    

##### handleMessage

This function uses the message from server to handle the changes that need to be made. It either calls helper functions or handles the message in here.

	extract message type from the message
    if "OK"
        store the player letter into client_info
    else if "GRID"
        store the grid size
        validate that display size is at least that
        if it isn't
            prompt the client to resize until it is
        when it's the right size, store current display size in client_info
        refresh()
    else if "GOLD":
        extract the collected, purse, and remaining from the message
        update these fields in the client_info
    else if "DISPLAY"
        call handleDisplay() with the message
        store the current display into client_info's last known display
    else if "QUIT"
        call handleQuit() with the message
        return true
    else if "ERROR"
        call handle_error() with the message
    else
        in case of unknown/malformed message, display it at the top

##### server_setup

This function uses the hostname and port to create a connection with the server and sends an initial message depending on whether the client is a player or spectator.

    set up serverPort on which to receive messages (stderr)
    check to see if port was set up properly
    call message_setAddr from the message module provided to with hostname, port, and pointer to server to set up the server
    send initial message to server
        "PLAY *playername*" if playername provided
        "SPECTATE" otherwise
    return the server

##### handleTimeout

This function contains the mechanism for handling timeout.

    if the timeout is on:
        send message to stderr
        return true;
    return true
    
##### handle_display

This function takes in the message and prints out the display and status line.

    clear window and move to (1,0)
    print each lien of the grid string
    print the status line using info from client_info
        if gold_update from client_info true
            print out the status line w/ the collected amount
            set gold_upudate to false
    refresh()      

##### handle_quit

This function uses the server message to handle the quit protocol.

    if case of game over
        clear screen
        print the game over message line by line
    otherwise
        print the message received from server at (0,0)
        sleep for 3 secs to allow users time to read the message
        
##### handle_error

This function takes in the message and prints out the explanation before calling handle_display.

    format the message to not include "ERROR"
    print message at (0,0) in format "Server message: *explanation*"
    refresh()
    
##### initDisplay

This function initializes the display.

    initialize curses
    set background and text colors
    initialize the size of the windo to current screen size
    refresh()

##### duplicate_str

This function duplicates a string. We use to to keep a copy of the latest display.

    if the last_display in client info isn't null
        free it
    get the length of the string, including the null terminator
    allocate memory for the new string in client_info's latest_display
    if successful
        copy the string to the newly allocated memory
    
#### Other modules

##### support

We utilize the support module provided to us. More specifically, we rely on the support of message and log from this module.

#### Implementation Note

All of our messages to the player show up on the display, this includes error messages and the Game Over summary. This was a stylistic choice that our team decided to make because it kept the terminal clean. 

#### Function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function in`client.c` and is not repeated here.

```c
int main(int argc, char* argv[]);
bool parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername);
bool handleInput(void* arg);
bool handleTimeout(void* arg;
bool handleMessage(void* arg, const addr_t from, const char* message);
addr_t server_setup(char* hostname, char* port, char* playername);
void handle_display(const char* message);
void handle_quit(const char* message);
void handle_error(const char* message);
void initDisplay();
void duplicate_str(const char*);
```

#### Error handling and recovery
All command-line parameters are rigorously checked before any data structures are allocated or work begins. Problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are also handled. For example, before storing the message in `last_display` in `duplicate_str`, we ensure that memory has been allocated properly.

Other player errors are also handled, mostly on the server side. Additionally, before the game begins, the client ensures that the window size is large enough to display the map. If it's not sufficient, it continuously prompts the player to increase the window size until it meets the requirements. It also displays the current size of the player's window to provide better information to the player.

#### Testing plan

For the testing of the client module, we utilize the server provided in the shared directory for the class. We run client with that server and make sure it can send and receive messages to the server as well as update the display for the player accordingly. We test with all manner of inputs to make sure that the client is able to handle any user error. 

#### Exit Codes

0: Program ran successfully 
1: Invalid number of arguments or invalid arguments 
2: Message_loop ran into problems

### Server

#### Data structures 

We do not create any new data structures within `server.c`. 

#### Control flow

The Server is implemented in one file `server.c`, with 6 functions.

##### main

The `main` function does the following:
    
        confirms validity of num arguments
        checks if the map file is a readable file
        initializes a random seed based on input or lack of input
        initializes grid
        initializes game
        initializes message module
        calls message_loop until game is over or error
        shuts down message module
        deletes the game
        deletes the grid
        returns 0 if no errors or 1 if errors exist from message_loop
        

##### handleMessage

The `handleMessage` function does the following:
    
        prints message and a prompt
        allocate buffer for a line of input
        reads a line from stdin
        if line is EOF stop looping
        else: strip trailing line
        
        PLAY
        check if message starts with PLAY
        if so check if player's name is empty
        else: create new player and add to the game
        if game is full send appropriate message back
        else: send OK message back with client's new letter
        Then sends grid dimensions, gold update and display
        
        SPECTATE
        check if message starts with SPECTATE
        if so add a new spectator
        if an old spectator existed send an appropriate message back and replace them with new spectator
        Then sends grid dimensions, gold update and display to new spectator
        
        KEY
        check if message starts with KEY
        checks if key pressed is equal to Q
        if so then finds player within game, sets as inactive, and sends quit message
        if find_player is null, key is from spectator and it replaces spectator with NULL and sends quit message
        if key != Q and find_player != null then find player inside of the game
        store x, y, and curr gold count before move player
        call move player from game which executes movement of player
        store x, y, and curr gold count after move player
        if currgold is not equal to previous gold then send goldUpdate to client
        if prev x and y's are not same as curr x and y's send display update to client
        
        END of game
        checks if available gold is equal to zero
        if so retrieves game summary
        iterates through players array in game and sends game summary to all players
        sends game summary to spectator if one exists
        
        returns false -- to keep logging

##### goldUpdate

The `goldUpdate` function does the following:
    
        sets a variable n to the amount of gold a certain player just collected (input in the function)
        calls get gold and get available gold and sets them to variables p and r respectively 
        creates a new string that begins with "GOLD" and then is followed by n p and r
        returns this new string
        
##### spectatorGoldUpdate

The `spectatorgoldUpdate` function does the following:
    
        sets variables n and p to zero as spectators can't pickup gold
        calls get available gold and sets it to variable r
        creates a new string that begins with "GOLD" and then is followed by n p and r
        returns this new string

#### Function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function in`server.c` and is not repeated here.

```c
int main(int argc, char *argv[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static char* goldUpdate(game_t* game, player_t* player, int collected);
static char* spectatorGoldUpdate(game_t* game);
```

#### Error handling and recovery

- Command line arguments are tested in main and if there are any issues we print to stderr with a suitable error message
- In main we also check if there is an error with initializing the message module
- If message loop has an error our main function returns 4; else returns 0
- Our handleMessage has simple checks for client input such as inputting an empty name or having maximum number of players in the game and sends appropriate messages back to the client

#### Testing plan

- For the testing of the server module, we utilized the miniclient provided in the support directory inside of our repository.

#### Exit Codes

- If main has the incorrect number of inputs our main function returns 1
- If the given map file cannot be read from, our main function returns 2
- If message module cannot be initialized our main function returns 3
- If message loop has an error our main function returns 4
- Else: main returns 0

### Game

#### Data structures 

The game module implements the `game` data structure which stores the grid, an array of all of the players, the spectator, and the current and total amount of gold.

#### Control flow

The grid module is implemented in the file `game.c` with corresponding header file `game.h`, with the following functions:

##### initialize_game

Given a pointer to a grid, initialize a game. The array of players is initially empty, there is initially no specator, and the avaiable and total gold are based on a constant given in the requirements.

##### gridDisplay

Given a player, creates and returns a string that represents the map according to what that player can see. Returns the string.

Pseudocode:

```
create grid string
for r in rows
  for c in cols
    if there's a player at point
      if point is visible
        if that player is the player
          print @
        else
          print player's letter
      else if point is known
        print point's terrain
      else if point is not known
        print space
    else if there's no player
      if point contains gold
        if point is visible
          print *
        else if point is known
          print room spot
        else
          print space
      else if point doesn't contain gold
        if point is known
          print point's terrain
        else
          print space
return grid string
```

##### gridDisplaySpectator

Create and return a string that displays the map without regard for visibility, because the spectator can see everything.

Pseudocode:

```
create grid string
for r in rows
  for c in cols
    if there's a player at the point
      print the player's letter
    else
      print the point's terrain
return grid string
```

##### movePlayer

Given a game, player, and keypress, move the player according to the keypress, if possible.

Pseudocode:

```
if keypress is lowercase
  if player can move in specified direction
    move the player by updating coordinates
if keypress is uppercase
  while player can move in specified direction
    move player
```

##### placePlayer

Given a player, places that player in a room spot or tunnel.

Pseudocode:

```
until valid spot is chosen
  randomly choose a point
  if point is room or tunnel
    if no other player at point
      place the player at that point
      break
```

##### add_player

Given a pointer to a player struct, if there aren't already 26 players, add that player to the game and decide its letter based on which number player it is.

##### find_player

Given an address, either return a pointer to the player who has that address, or null if there is no such player.

##### add_spectator

Given an address, add a spectator to the game. Return the previous spectator's adress, or NULL if there was no previous spectator. 

##### Getters

`find_player` - Returns pointer to a player given an address.
`get_spectator` - Returns adress of current spectator.
`get_grid_dimensions` - Return dimensions of the grid.
`get_total_gold` - Return total amount of gold.
`get_available_gold` - Return remaining amount of gold.
`get_players` - Return the array of pointers.

##### Setters

`game_inactive_player` - Set a player to inactive.
`update_gold` - Set gold count.

##### game_summary

Send a message of the game over summary to the given address, including the names, letters, and gold totals of each player.

##### delete_game

Deletes the game struct and frees all associated memory.

#### Function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function in`game.h` and is not repeated here.

```c
game_t* initialize_game(grid_t* grid);
char* gridDisplay(player_t* player);
char* gridDisplaySpectator();
void movePlayer(player_t* player, char letter);
void placePlayer(player_t* player);
int add_player(player_t* player);
player_t* find_player(addr_t address);
addr_t add_spectator(addr_t spectator);
addr_t get_spectator();
void get_grid_dimensions();
int game_inactive_player(game_t* game, player_t* player);
int update_gold(game_t* game, int updateGoldCount);
int get_total_gold(game_t* game);
int get_available_gold();
void game_summary(addr_t address);
void delete_game();
player_t** get_players(); 
```

#### Testing plan

Once grid and player are done, we run the three modules together to see if games can successfully be made and updated. As we test server and client together, we will make sure that game works the way we want it to.

### Grid

#### Data structures 

The grid module implements the `grid` data structure, which represents the game's map and stores all of the information about the terrain and where the players and gold are. `grid` stores the number of rows and columns and a 2D array of gridpoints. Gridpoints are represented by the `gridpoint` structure, which contains the row, column, and type of terrain of a point in the map, as well as the amount of gold and the player standing there, if any.

#### Control flow

The grid module is implemented in the file `grid.c` with corresponding header file `grid.h`, with the following functions:

##### gridInit

Given a map text file and a random seed, creates a grid struct. Reads the file to determine the number of rows and columns and iterates through each character in the file to produce a gridpoint with the corresponding terrain. It then generates the gold based on the seed provided.

Pseudocode:

```
allocate memory for grid
read map text file
rows = number of lines
cols = characters per line
create 2D array of gridpoints based on number of rows and columns
for each row in file
  for each col in file
    gridpointArray[row, column] = character
randomly distribute piles of random amounts of gold to valid gridpoints
return grid
```

##### Getters

`getnRows` – getting the number of rows in the grid.
`getnColumns` – getting the number of columns in the grid.
`getPoint` – getting a pointer to gridpoint struct at a given location.
`getTerrain` – getting the terrain stored at a given gridpoint.
`getPlayer` – getting the player (letter) stored at a given gridpoint.
`getPointRow` – getting the y-coordinate (row) of a gridpoint struct.
`getPointColumn` – getting the x-coordinate (column) of a gridpoint struct.
`getPointGold` – getting the amount of gold stored in a point.


##### Setters

`setPlayer` – setting the player (letter) stored at a given gridpoint.
`setTerrain` – setting the terrain stored at a given gridpoint.
`setPointGold` – setting the amount of gold stored at a given gridpoint.


##### gridDelete

Frees the memory used on the grid by deleting each gridpoint, then deleting the grid itself.

##### blocksVisibility

Determines if a gridpoint's terrain blocks visibility or not. Returns true if it's a wall or tunnel, and false if it's a room spot or gold.

#### Function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function in`grid.h` and is not repeated here.

```c
grid_t* gridInit(char* pathName, int randomSeed);
void gridDelete();
bool blocksVisibility(const int row, const int col);
int getnRows();
int getnColumns();
gridpoint_t* getPoint(int row, int column);
char getTerrain(gridpoint_t* gridpoint);
char getPlayer(gridpoint_t* gridpoint);
int getPointColumn(gridpoint_t* gridpoint);
int getPointRow(gridpoint_t* gridpoint);
void setPlayer(gridpoint_t* gridpoint, char player);
void setTerrain(gridpoint_t* gridpoint, char terrain);
void setPointGold(gridpoint_t* gridpoint, int nGold);
int getPointGold(gridpoint_t* gridpoint);
```

#### Testing plan

Before integrating grid with player and game, hardcode some inputs to make sure it runs and doesn't have any memory leaks. As we integrate it with grid and game, make sure that dependency and interactions between modules all work correctly.

### Player

#### Data structures 

The player module implements the `player` data structure, which represents each player in the game and stores the player's port, name, letter, x and y coordinates, its amount of gold, whether or not it's active, and 2D arrays representing which parts of the map are known and visible to the player.

#### Control flow

The player module is implemented in the file `player.c` with corresponding header file `player.h`, with the following functions:

##### player_new

Given a port, name, letter, and x and y coordinate, initalize a new player. Store all of those parameters, initialize the player to be active and have 0 gold, and create 2D boolean arrays for which points are known and visible based on the number of rows and columns in the grid.

##### Getters 


`get_name` - Returns player's name.
`get_letter` - Returns player's letter.
`get_MaxNameLength` - Returns max name length constant.
`get_x` - Returns player's x coordinate.
`get_y` - Returns player's y coordinate.
`get_gold` - Returns player's amount of gold.
`get_address` - Returns player's address.
`isActive`- Returns true/false based on whether a given player is active.
`isVisible`- Returns true/false based on whether a given point is visible to a player.
`isKnown` - Returns true/false based on whether a given point is known to a player.

##### Setters

`player_inactive` - Set player to inactive.
`set_letter` - Set player's letter.
`set_x` - Set player's x coordinate.
`set_y` - Set player's y coordinate.
`set_gold` - Set player's amount of gold.


##### updateVisibility

To update visibility, take the row and column position of the player and iterate through each point in the grid to check if that point is visible from the player's position. To check if a point is visible, check each row and each column between the player and the point, exclusive. For each row, find the point where the line between the player and the tile intersects that row. If that point is exactly on a column, visibility continues if the gridpoint at that row and column is an open space, and is broken otherwise. If that point is betwen two columns, check both gridpoints to its left and right. If either are open spaces, visibility continues, and if both aren't, visibility is broken. For each column, do the same thing, except find the row where the line intersects with a given column, and either check that gridpoint if it exactly intersects with one, or check the gridpoint above and the gridpoint below.

Also make functions `isKnown` and `isVisible` to return true or false based on whether or not a given point is known/visible to a given player.


Pseudocode for updateVisibility:

```
for r in numRows
  for c in numCols
    call lineCheck with pr, pc, r, c
    if visible
      visible[r, c] = true
      known[r, c] = true
    else
      visible[r, c] = false
```

Pseudocode for lineCheck:

```
create bools right and down
if r >= pr
  right = true
else
  right = false
if c >= pc
  down = true
else
  down = false
rowSlope = |(c - pc)/(r - pr)|
colSlope = |(r - pr)/(c - pc)|
for each row in between pr and r, exclusive // increase if down, decrease if not
  col = pc + rowSlope*(row - pr)
  if col is an int
     if (row, col) is not an open space
       return false
  else
    if (row, col rounded down) and (row, col rounded up) are not open spaces
      return false
count = 1
for each col in between pc and c, exclusive // increase if right, decrease if not
  row = pr + colSlope*(col - pc)
  if row is an int
    if (row, col) is not an open space
      return false
  else
    if (row rounded down, col) and (row rounded up, col) are not open spaces
      return false
return true
```

#### Function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function in`player.h` and is not repeated here.

```c
player_t* player_new(char* port, char* name, int x, int y, char letter);
void player_inactive(player_t* player);
void player_delete(player_t* player);
char* get_name(player_t* player);
char get_letter(player_t* player);
int get_MaxNameLength();
char get_x(player_t* player);
char get_y(player_t* player);
int get_gold(player_t* player);
addr_t get_address(player_t* player);
void set_letter(player_t* player, char letter);
void set_x(player_t* player, int x);
void set_y(player_t* player, int y); 
void set_gold(player_t* player, int gold);
bool isActive(player_t* player); 
bool isVisible(player_t* player, const int row, const int col);
bool isKnown(player_t* player, const int row, const int col);
void updateVisibility(player_t* player);
```

#### Extra credit

We will limit the range of visibility to a radius of 5 around the player. In `updateVisibility`, we will only call `lineCheck` on points that are within 5, which we will determine by calculating the distance from the point at (row, col) to the point (pr, pc) using the Pythagorean theorem.

#### Testing plan

Because most of the functions in player are relatively simple getters/setters, we can test them as we run the game to integrate client and server, and as we test grid and game. To test visibility, check if the equations work mathematically outside of the context of the game to see if they calculate the lines correctly.

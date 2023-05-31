# CS50 Nuggets
## Design Spec
### Binary Brigade, Spring, 2023

According to the Requirements Spec, the Nuggets game requires two standalone programs: a client and a server. Our design also includes game, grid, and player modules. We describe each program and module separately. We do not describe the `support` library nor the modules that enable features that go beyond the spec. We avoid repeating information that is provided in the requirements spec.

## Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.

To join the game, the user uses the following syntax on the command-line:

    ./client hostname port [playername]
    
If the user inputs a *playername*, they will become a player, which prompts the game-playing mode described in the requirements spec. The user will play the game using the keystrokes described below.

If the user does not input a *playername*, they will become a spectator, which prompts the bird's-eye view state described in the requirements spec.

#### Inputs (keystrokes) from client

- `Q` quit the game.
- `h` move left, if possible
- `l` move right, if possible
- `j` move down, if possible
- `k` move up, if possible
- `y` move diagonally up and left, if possible
- `u` move diagonally up and right, if possible
- `b` move diagonally down and left, if possible
- `n` move diagonally down and right, if possible
- capital movement key moves as many steps in that direction as possible

#### Outputs (display)

A string representation of the current grid, messages about the status of the game, and a game summary after the game ends.
### Functional decomposition into modules

- `main` takes in command line arguments and calls helper functions to do the rest
- `parseArgs` validates command line arguments and stores them in the specified variables
- `handleInput` takes in messages (chars) from the player and send it to the the server after formatting
- `handleMessage` uses the message from server to deal with the changes that need to be made. Either calls helper functions or handles the messages itself
- `handleTimeout` contains the mechanism for handling time out 
- `server_setup` uses the hostname and port to create a connection with the server and sends an initial message depending on whether the client is a player or spectator
- `handle_display` takes in the message and prints out the display and status line
- `handle_quit` uses the server message to handle the quit protocol
- `handle_error` takes in the message and prints out the explanation on top of screen
- `initDisplay` initializes the display 
- `duplicate_str` duplicates string; used to to keep track of latest display 
 
### Pseudo code for logic/algorithmic flow

The server will run as follows:

```
  execute from a command line per the requirement spec
  parse the command line, validate parameters
  call initDisplay() to set up the display
  call server_setup() to set up the connection to the server
  initialize the 'message' module
  pass in handleInput and handleMessage to message_loop to start the game
    handleInput with take in messages from user and send formatted messages to server
    handleMessage will take in messages from the clients and update accordingly
  call message_done() when client's game ends or quits
  clean up and exit
```

### Major data structures

Major data structure used for the client `client_info`, which is created inside the client itself. It holds the following information for the client:

- int display_nr - number of rows of the display window
- int display_nc - number of columns of the display window
- int map_nr - number of rows in the map
- int map_nc - number of columns in the map
- int collected - amount of gold collected
- int purse - amount of gold in the purse
- int remaining - amount of gold remaining
- char* playername - player's name
- char playerletter - player's letter
- char* last_display - the last display received from server
- bool gold_update - tracks whether gold was updated or not

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

#### Inputs 
The server will take messages from the client(s), communicating whatever action the client is performing.

#### Outputs 
The server sends various messages to the clients currently in the game. These include grid information (for new players), grid displays, and quit, update, and game over messages.

### Functional decomposition into modules

- `initializeGame` sets up the data structures for the game 
- `runGame` calls message_loop and uses the messages received to call relevant functions to run the game
- `gameOver` informs the clients that game is over; clean up; exit 
 
### Pseudo code for logic/algorithmic flow

The server will run as follows:

```
  execute from a command line per the requirement spec
  parse the command line, validate parameters
  call initializeGame() to set up data structures
  initialize the 'message' module
  calls runGame() to start the game
  print the port number on which we wait
  call message_loop(), to await clients
  listen for messages from clients and call methods in 
  the relevant data structures 
  call gameOver() to inform all clients the game has ended
  clean up
```

### Major data structures

Major data structures are the following, which are described in their own module sections:

- game
- grid
- player

---

## Game

The game data structure will hold the information pertaining to the current game. This entails the amount of gold remaining in the game (whether or not the game is still going on), an overview of the players in the game (pointers to player structs), and a pointer to the grid. The game data structure will be used as a global variable, which will allow us to refer to its contents at various points throughout the program. In addition, this will save us the trouble of passing along the struct across functions, as well as checking for NULL-pointers at every stage.

### Functional decomposition

- `newGame` given a grid, creates a new game; initializes the game variables, such as gold, players, and stores the grid. Returns a pointer to the game structure 
- `addPlayer` given a player, add the player to the game
- `deleteGame` frees the memory allocated for the game struct and the information within it.


### Pseudo code for logic/algorithmic flow

`newGame`
```
  takes in the grid as the input, 
  create a Game structure to create a Game object
  create variables: gold, reference to players, reference to grid
  return a pointer to Game 
```

`addPlayer`
```
  take pointer to player
  add player to game and determine its letter
```

`deleteGame`
```
  take in an initialized game struct
  free the memory allocated for the struct itself and for the information it holds
```

### Major data structures

- `game` Stores a universal grid, an array of pointers to all of the players, the total amount of gold, and the amount of available gold

### Extra credit

When one player collides with another, it will steal the other player's gold.

## Grid

The grid struct will be initialized with a map txt file. It holds types of spaces with an internal struct gridpoint. Spaces can be gold, empty in a room, occupied by a player, in a tunnel, empty outside of a room, or a wall. Our gridpoint struct will hold these and be updated as players move around the grid. These gridpoints will be held in a 2-dimensional array that we can iterate through and call methods on. 

### Functional decomposition

- `init` Given map, determine the number of rows and columns and the terrain type of each point, and create grid.
- `generateGold` determine amount of gold and location of gold based on certain parameters and random seed
- `update` update the gridpoints within grid based on player movement (using gridpoint helper) -- includes vision as well as location and presence of gold
- `sendMap` sends the map relating to each player in order for it to be displayed for each player based on visibility and past history of viewing rooms and tunnels
- `delete` delete the grid and clear memory once game is over

### Pseudo code for logic/algorithmic flow

```
  load in map file and generate grid data structure
  randomly load in gold into locations within rooms based on a random seed
  run a while loop while game is still going
    update map based on player movement and gold allocation
    update gold location and availability based on player movement
    update visibility 
    send player maps to each player to display current status
  end game for all players
  send game summary
  free all memory associated with players and grids
```

### Major data structures

- `grid` Stores number of rows, number of columns, and a 2D array of gridpoints.
- `gridpoint` Stores its row, its column, what type of terrain it is, what player (if any) is standing there, and how much gold (if any) it has.

## Player

The player data structure will hold the information pertaining to a player. This will include the player's port, letter, location, and amount of gold. 

### Functional decomposition

- `updateVisibility` Uses current coordinates and layout of the grid to determine which points should be visible or not.
- `newPlayer` taken in name, coordinate, and gold level as input creates a new player struct to be used by other modules and server. Returns a pointer to a new player
- `deletePlayer` Frees the memory associated with the player and removes it from the game.

### Pseudo code for logic/algorithmic flow

`newPlayer`
```
  take in inputs and validates them
  create a new player and returns a pointer to new player
```

`updateVisibility`
```
  take in a pointer to a player
  for each point in the grid
    check all points in a line between player and the point
    if vision isn't blocked, point is visible
    if vision is blocked, it is not visible
```

### Major data structures

- `player` Stores the information of how the client connects to the server, its name, letter, coordinates, amount of gold, and what parts of the map are known and visible to it.

### Extra credit

To implement the visibility range limitation, we will restrict `updateVisibility` to only check points within a radius of 5 from the player.

# CS50 Nuggets
## Design Spec
### Binary Brigade, Spring, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

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

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

#### Inputs 
The sever will take messages from the client(s), communicating whatever action the client is performing.

#### Outputs 
The sever sends various messages to the clients currently in the game. These includes grid information (for new players), grid displays, and quit messages.  

### Functional decomposition into modules

> List and briefly describe any modules that comprise your server, other than the main module.

- `initializeGame` sets up the data structures for the game 
- `runGame` calls message_loop and uses the messages received to call relevant functions to run the game
- `gameOver` informs the clients that game is over; clean up; exit 
 
### Pseudo code for logic/algorithmic flow

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

The server will run as follows:

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
    calls runGame() to start the game
	print the port number on which we wait
	call message_loop(), to await clients
    listen for messages from clients and call methods in 
    the relevent data structures 
	call gameOver() to inform all clients the game has ended
	clean up

### Major data structures

Major data structures described in sections below:

- grid
- game
- player

#### Game
The game data structure will hold the information pertaining to the current game. This entails the amount of gold remaining in the game (whether or not the game is still going on), an overview of the players in the game (pointers to player structs), and a pointer to the grid. The game data structure will be used as a global variable, which will allow us to refer to its contents at various points throughout the program. In addition, this will save us the trouble of passing along the struct across functions, as well as checking for NULL-pointers at every stage.

#### Player
The player data structure will hold the information pertaining to a player. This will include the player's port, letter, location, and amount of gold. 

#### Grid
The grid struct will be initialized with a map txt file. It holds types of spaces with an internal struct gridpoint. Spaces can be gold, empty in a room, ocuppied by a player, in a tunnel, empty outside of a room, or a wall. Our gridpoint struct will hold these and be updated as players move around the grid. These gridpoints will be held in a 2-dimensional array that we can iterate through and call methods on. 

#### Gridpoint
The gridpoint struct represents each square in the map. It tracks what type of terrain each spot is, what player (if any) is standing there, how much gold (if any) it has, if it's known, and if it's currently visible.

---

## Grid

### Functional decomposition

- `init` Given map, determine the number of rows and columns and the terrain type of each point, and create grid.
- `generateGold` determine amount of gold and location of gold based on certain parameters and random seed
- `update` update the gridpoints within grid based on player movement (using gridpoint helper) -- includes vision as well as location and presence of gold
- `sendMap` sends the map relating to each player in order for it to be displayed for each player based on visibility and past history of viewing rooms and tunnels
- `delete` delete the grid and clear memory once player is done or game is over

### Pseudo code for logic/algorithmic flow

load in map file and generate grid data structure
randomly load in gold into locations within rooms based on a random seed
run a while loop while game is still going
    - update map based on player movement and gold allocation
    - update gold location and availability based on player movement
    - update visibilty 
    - send player maps to each player to display current status
End game for all players
Send game summary
Free all memory associated with players and grids

### Major data structures

- `grid` Stores number of rows, number of columns, and a 2D array of gridpoints.
- `gridpoint` Stores its row, its column, what type of terrain it is, what player (if any) is standing there, how much gold (if any) it has, if it's known by the player, and if it's currently visible to the player.

## Game

### Functional decomposition

- `newGame` given a grid, creates a new game; initializes the game variables, such as gold, players, and stores the grid. Returns a pointer to the game strucutre 


### Pseudo code for logic/algorithmic flow

newGame
-  Takes in the grid as the input, 
-  Creates a Game structure to create a Game object
-  Uses creates the variables that will store the information of the game - gold pile, reference to players, and the grid
-  Returns a pointer to Game 

### Major data structures

- `game` Stores a universal grid, pointers to all of the players, the number of piles left, the amount of gold left, and all of the piles.
- `pile` Stores coordinates of pile and amount of gold in it.

## Player

### Functional decomposition

- `updateVisibility` Uses current coordinates and layout of the grid to determine which points should be visible or not.
- `newPlayer` taken in name, coordinate, and gold level as input creates a new player struct to be used by other moduals and server. Returns a pointer to a new player

### Pseudo code for logic/algorithmic flow

newPlayer
- Take in inputs and validates them
- Create a new player using and returns a pointer to new player

updateVisibility
- Take in a pointer to a player and their current coordinate
- Updates the visibility of the player

### Major data structures

-`player` Stores its grid, whether it's a player or a spectator, its name, letter, coordinates, and amount of gold.
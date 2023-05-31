/*
 * Nuggets client.c file
 *
 * Connects a player to a server, sends
 * keystrokes, and recieves a display
 * of the map and information about the game.
 * 
 * Binary Brigade, Spring, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>
#include <ncurses.h> 
#include <unistd.h>
#include <signal.h>  
#include "../support/message.h"
#include "../support/log.h"

bool parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
addr_t server_setup(char* hostname, char* port, char* playername);
void handle_display(const char* message);
void handle_quit(const char* message);
void handle_error(const char* message);
void initDisplay();
void duplicate_str(const char*);
bool handleTimeout(void* arg);

// helper struct to hold all the client info we need throughout the program
typedef struct client_info{
    int display_nr;
    int display_nc;
    int map_nr;
    int map_nc;
    int collected;
    int purse;
    int remaining;
    char* playername;
    char playerletter;
    char* last_display;
    bool gold_update;
    bool timeout_on;
} client_info_t;

client_info_t* client_info;

/**************** main ****************/
/* 
 * takes in commmand line arguments and calls helper functions to do rest
 */
int 
main(int argc, char* argv[])
{
    char* hostname;
    char* port;
    char* playername = (argc == 4) ? argv[3] : NULL;

    if (!parseArgs(argc, argv, &hostname, &port, &playername)) {
        printf("Usage: ./client hostname port [playername]\n");
        return 1;
    }

    client_info = calloc(1, sizeof(client_info_t));

    // store playername for later use
    client_info->playername = playername;

    // initialize display
    initDisplay();

    client_info->timeout_on = true;

    // initalize network + join the game
    addr_t server = server_setup(hostname, port, playername);

    // message loop to run program
    bool ok = message_loop(&server, 3, handleTimeout, handleInput, handleMessage);
   
    // clean up message modual
    message_done();
    endwin();
    free(client_info->last_display);
    free(client_info);
   
    return ok? 0 : 2;
}

/**************** handleMessage ****************/
/* 
 * uses the message from server to deal with the changes that need to be made
 * either calls helper functions or handles the message in here
 * 
 * Caller provides:
 *   arg, address from, and the message received
 * We return:
 *   false to keep game going; true otherwise
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{   
    // extract the message type from the message
    char messageType[256];
    sscanf(message, "%s", messageType);

    if (strcmp(messageType, "OK") == 0){
        
        // if message OK, store the player's letter
        sscanf(message, "%*s %c", &client_info->playerletter);
        client_info->timeout_on = false;
    
    } else if (strcmp(messageType, "GRID") == 0){
        
        // if the message is GRID, store the grid size + validate that display size is at least that
        
        int nrows, ncols;
        sscanf(message, "%*s %d %d", &nrows, &ncols);
        
        client_info->map_nc = ncols;
        client_info->map_nr= nrows;
        
        // get curr display size
        getmaxyx(stdscr, client_info->display_nr, client_info->display_nc);

        // don't let the user proceed until the display is at least as large as the size of the grid
        while (nrows +1 > client_info->display_nr || ncols > client_info->display_nc) {
            
            clear();
            mvprintw(0, 0, "Your window must be at least %d high", nrows+1);
            mvprintw(1, 0, "Your window must be at least %d wide", ncols);
            mvprintw(3, 0, "Current size: %d (height) x %d (width)", client_info->display_nr, client_info->display_nc);
            mvprintw(2, 0, "Resize your window, and press Enter to continue.");
           
            refresh();

            while (getch() != '\n') {
                // Wait for Enter key press
            }

            // get the update display size
            getmaxyx(stdscr, client_info->display_nr, client_info->display_nc);
            refresh();

        }
        
    } else if (strcmp(messageType, "GOLD") == 0){

        // if message is GOLD, extract the info and store in client) info for use later
        sscanf(message, "%*s %d %d %d", &client_info->collected, &client_info->purse, &client_info->remaining);
        client_info->gold_update = true;

    } else if (strcmp(messageType, "DISPLAY") == 0){
        
        // if message is DISPLAY, call handle display with the message
        handle_display(message);

        // update last_display only when the message starts with "DISPLAY"
        // this way we can come back to the last known display at any point 
        const char* prefix = "DISPLAY";
        
        if (strncmp(message, prefix, strlen(prefix)) == 0) {
            
            duplicate_str(message); 
        }

    } else if (strcmp(messageType, "QUIT") == 0){
        
        handle_quit(message);

        // return true to exit the message loop
        return true;
    
    } else if (strcmp(messageType, "ERROR") == 0) {

        // if message is ERROR, call handle error with the message to take care of it
        handle_error(message);
        
    } else {
        // if unknown or malformed message, just display to player what came from server
        
        mvprintw(0, 0, "Server message: %s", message);
    }

    // return false to keep the message looop going
    return false;
}


/**************** handle_quit ****************/
/* 
 * uses the server message to handle the quit protocol
 * 
 * Caller provides:
 *   the message
 * We return:
 *   nothing
 */
void 
handle_quit(const char* message) 
{
    
    if (strncmp(message, "QUIT GAME OVER:", strlen("QUIT GAME OVER:")) == 0) {
        
        clear();
        // extract the remaining part of the message after the colon
        const char* explanation = message + strlen("QUIT GAME OVER:") + 1;

        // display the remaining message on the screen
        mvprintw(0, 0, "GAME OVER:\n");
        refresh();

        // print line by line
        int line = 1;
        while (*explanation != '\0') {
            mvprintw(line, 0, "%s", explanation);
            explanation = strchr(explanation, '\n');
            if (explanation == NULL) {
                break;
            }

            // move past the '\n' character
            explanation++;  
            line++;
        }
        
        refresh();
        sleep(5);
    
    } else {

        const char* spacePos = strchr(message, ' ');
        if (spacePos != NULL) {
            // extract the remaining part of the message after the space
            const char* explanation = spacePos + 1;

            // display the remaining message on the screen
            mvprintw(0, 0, "%s", explanation);
            refresh();
        }
        // sleep for 3 seconds to allow the player enough time to read
        sleep(3);
    }
}

bool 
handleTimeout(void* arg)
{   
    if (client_info->timeout_on){
        
        fprintf(stderr, "Sever took too long to respong. Good bye!\n");
        return true;
    }
    return false;
}

/**************** handle_display ****************/
/* 
 * Takes in the messsage and prints out the display and status line 
 * 
 * Caller provides:
 *   the message
 * We return:
 *   nothing
 */
void 
handle_display(const char* message)
{

    // skip the "DISPLAY" part of the message
    const char* gridString = message + strlen("DISPLAY\n");

    // clear the window and move to the top-left corner
    clear();
    move(1, 0);
        
    printw("%.*s", client_info->display_nc, gridString);  // Print one line


    // update status line
    char statusLine[256];
    
    if (client_info->playername != NULL) {

        if (client_info->gold_update){
            snprintf(statusLine, sizeof(statusLine), "Player %c has %d nuggets (%d nuggets unclaimed). %d collected",
                 client_info->playerletter, client_info->purse, client_info->remaining, client_info->collected);
                 client_info->gold_update = false;

        } else {
            snprintf(statusLine, sizeof(statusLine), "Player %c has %d nuggets (%d nuggets unclaimed).",
                 client_info->playerletter, client_info->purse, client_info->remaining);
        }
    } else {
        snprintf(statusLine, sizeof(statusLine), "Spectator: %d nuggets unclaimed.", client_info->remaining);
    }

    // print the status line at the top of the screen
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "%s", statusLine);
    
    // refresh the window to show the changes
    refresh();
    
}


/**************** handle_error ****************/
/* 
 * Takes in the messsage and prints out the explanation before calling handle_display
 * 
 * Caller provides:
 *   the message
 * We return:
 *   nothing
 */
void 
handle_error(const char* message) 
{

    const char* spacePos = strchr(message, ' ');
    if (spacePos != NULL) {
        
        // extract the remaining part of the message after the space
        const char* explanation = spacePos + 1;

        // display the remaining message on the screen
        mvprintw(0, 0, "Server message: %s", explanation);
        
        // refresh to show changes
        refresh();
    } 
 
}


/**************** handleInput ****************/
/* 
 * Takes in message (char) from the player and send it to the the server with the right format 
 * 
 * Caller provides:
 *   void* arg (server)
 * We return:
 *   false to keep looping; true otherwise
 */
static bool 
handleInput(void* arg) {

    // We use 'arg' to receive an addr_t referring to the 'server' correspondent.
    // Defensive checks ensure it is not NULL pointer or non-address value.
   
    addr_t* server = arg;

    if (server == NULL) {
        log_v("handleInput called with arg=NULL");
        return true;
    }

    if (!message_isAddr(*server)) {
        log_v("handleInput called without a correspondent.");
        printf("You have no correspondent.\n");
        fflush(stdout);
        return true;
    }

    // read a single character of input using getch()
    int ch = getch();  

    // check if the character is a newline (Enter key)
    if (ch == '\n') {

        // ignore newline character and continue waiting for input
        return false;  
    }

    // format the message with the key
    char message[message_MaxBytes];
    snprintf(message, message_MaxBytes, "KEY %c", ch);

    // send the message to the server
    message_send(*server, message);
    
    // normal case: keep looping
    return false;
}


/**************** server_setup ****************/
/* 
 * Uses the hostname and port to create a connection with the server and sends an 
 * initial message depending on whether the client is a player or spectator
 * 
 * Caller provides:
 *   void* arg (server)
 * We return:
 *   false to keep looping; true otherwise
 */
addr_t
server_setup(char* hostname, char* port, char* playername)
{
    // set up a server port on which to receive messages
    int serverPort = message_init(stderr);
    if (serverPort == 0) {
        
        fprintf(stderr, "Error setting up server port\n");
        exit(1);
    }

    addr_t server;
    if (!message_setAddr(hostname, port, &server)) {
        fprintf(stderr, "can't form address from %s %s\n", hostname, port);
        // bad hostname/port
        exit(2);
    }

    if (playername != NULL) {
        
        char message[message_MaxBytes];
        snprintf(message, message_MaxBytes, "PLAY %s", playername);
        message_send(server, message);
    
    } else {
        message_send(server, "SPECTATE");

    }

    return server;
}


/**************** parseArgs ****************/
/* 
 * Validates command line arguments and stores them in the spedified vars
 * 
 * Caller provides:
 *   argc, argv, pointer to pointers for hostname, port, and playername
 * We return:
 *   true if valid arguments, false otherwise
 */
bool 
parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername) 
{
    
    // check number of arguments
    if (argc < 3 || argc > 4) {
        return false;
    }

    // store hostmae
    *hostname = argv[1];

    // make sure port is a number
    if (atoi(argv[2]) == 0 ) {
        return false;
    }

    // store port
    *port = argv[2];

    // if playername provided, store it, otherwise it's nun for spectator
    *playername = (argc == 4) ? argv[3] : NULL;

    // if we get here, valid arguments
    return true;
}


/**************** initDisplay ****************/
/* 
 * initializes the display
 * 
 * Caller provides:
 *   nothing
 * We return:
 *   nothing
 */
void 
initDisplay()
{
    // Initialize curses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    // Set the background and text colors
    start_color();
    if (client_info->playername == NULL){
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
        
    } else {
        init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    }
    

    wbkgd(stdscr, COLOR_PAIR(1));
    
    // Initialize the size of the window to current screen size
    getmaxyx(stdscr, client_info->display_nr, client_info->display_nc);

    refresh();
}


/**************** duplicate_str ****************/
/* 
 * duplicates string. We use to to keep a copy of the latest display
 * 
 * Caller provides:
 *   the string to duplicate
 * We return:
 *   pointer to a string
 */
void 
duplicate_str(const char* str) 
{
    if (client_info->last_display != NULL) {
        free(client_info->last_display); 
    }

    // get the length of the string, including the null terminator
    size_t len = strlen(str) + 1;  
    // allocate memory for the new string
    client_info->last_display = malloc(len);  

    if (client_info->last_display != NULL) {
        // copy the string to the newly allocated memory
        memcpy(client_info->last_display, str, len);  
    }
}

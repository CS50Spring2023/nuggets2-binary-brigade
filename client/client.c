/*
 * Nuggets: client.c file
 * 
 * Binary Brigade, Spring, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include "../support/message.h"

#define NR 20
#define NC 20

// GLOBAL VAR
char* playername;

// can i have nr and nc as globals??????

int parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername);
bool handleInput(void* arg);
addr_t* server_setup(char* hostname, char* port, char* playername);
void handle_display(const char* message);
void handle_quit(const char* message);
void handle_error(const char* message);
void update_status_line(int collected, int purse, int remaining);
WINDOW* initDisplay();

int main(int argc, char *argv[])
{
    char* hostname;
    char* port;
    playername = (argc == 4) ? argv[3] : NULL;

    if (parseArgs(argc, argv, &hostname, &port, &playername) != 0) {
        printf("Usage: ./client hostname port [playername]\n");
        return 1;
    }

    // TODO/ PROBLEM initialize display
    WINDOW* win = initDisplay();

    // initalize network + join the game
    addr_t* server = server_setup(hostname, port, playername);

    // message stuff
    bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);

    // clean up message modual
    message_done();
    endwin();
    return ok? 0 : 10;
}


bool
handleMessage(void* arg, const addr_t from, const char* message)
{
    char messageType[256];
    sscanf(message, "%s", messageType);

    if (strcmp(messageType, "OK") == 0){
        
        // OK message received, do nothing
    
    } else if (strcmp(messageType, "GRID") == 0){

        // still need to implement this
        int nrows, ncols;
        sscanf(message, "%*s %d %d", &nrows, &ncols);

        if (nrows != NR + 1 || ncols != NC){
            
            handle_display("Please resize the window to fit the grid.");
            while (getch() == ERR){
                
            }
            clear();
        }
        
    } else if (strcmp(messageType, "GOLD") == 0){

        int collected;
        int purse;
        int remaining;

        sscanf(message, "%*s %d %d %d", &collected, &purse, &remaining);
        update_status_line(collected, purse, remaining);

    } else if (strcmp(messageType, "DISPLAY") == 0){
    
        handle_display(message);

    } else if (strcmp(messageType, "QUIT") == 0){
        
        endwin(); 
        printf("%s\n", message);
        exit(0);
    
    } else if (strcmp(messageType, "ERROR") == 0) {

        endwin(); 
        fprintf(stderr, "Server error: %s\n", message);
        exit(1);
    
    } else{

        // malformed message
        fprintf(stderr, "Malformed message from the server: %s\n", message);
        refresh();
        
    }

    return false;
}

void 
handle_display(const char* message)
{
    move(1, 0);
    clrtoeol();
    printw("%s", message);
    refresh();
}
void 
update_status_line(int collected, int purse, int remaining) 
{
    // Construct the status line string
    char statusLine[256];
    
    if (playername != NULL) {
        snprintf(statusLine, sizeof(statusLine), "Player %s has %d nuggets (%d nuggets unclaimed).",
                 playername, purse, remaining);
    } else {
        snprintf(statusLine, sizeof(statusLine), "Spectator: %d nuggets unclaimed.", remaining);
    }

    // Print the status line at the top of the screen
    move(0, 0);
    clrtoeol();
    printw("%s", statusLine);
    refresh();
}

bool 
handleInput(void* arg) {
    
    addr_t* server = arg;
    
    if (_SC_THREAD_SPORADIC_SERVER == NULL) { // defensive
        
        log_v("handleInput called with arg=NULL");
        return true;
    }
  
    if (!message_isAddr(*server)) {
        
        log_v("handleInput called without a correspondent.");
        printf("You have no correspondent.\n");
        fflush(stdout);
        return false;
    }

    char message[message_MaxBytes];
 
    if (fgets(message, message_MaxBytes, stdin) != NULL) {
        
        const int len = strlen(message);

        // PROBLEM: What if we have more than one key per line
        if (len > 2){
            // message cannot be more than on character (key is only one char?)
            return false;
        
        } else if (len > 0) {
            // change newline to null
            message[len-1] = '\0'; 
        }

        // format message
        snprintf(message, message_MaxBytes, "KEY %c", message);

        // send the line to our correspondent
        message_send(*server, message);
        return false;
   
    } else {
        // EOF
        return true; 
    }
    
}


addr_t* 
server_setup(char* hostname, char* port, char* playername)
{
    // set up a server port on which to receive messages
    int serverPort = message_init(stderr);
    if (serverPort == 0) {
        
        fprintf(stderr, "Error setting up server port\n");
        return 1;
    }

    addr_t server;
    if (!message_setAddr(hostname, port, &server)) {
        fprintf(stderr, "can't form address from %s %s\n", hostname, port);
        return 2; // bad hostname/port
    }

    if (playername != NULL) {
        
        char message[message_MaxBytes];
        snprintf(message, message_MaxBytes, "PLAY %s", playername);
        message_send(server, message);
    
    } else {
        message_send(server, "SPECTATE");

    }
}


int parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername) {
    
    if (argc < 3 || argc > 4) {
        return 1;
    }

    *hostname = argv[1];

    if (atoi(argv[2]) == 0 ) {
        return 2;
    }
    *port = argv[2];

    *playername = (argc == 4) ? argv[3] : NULL;

    return 0;
}

WINDOW* 
initDisplay()
{
    // Initialize curses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    // Create a new window for the game display
    WINDOW* win = newwin(NR + 1, NC, 0, 0);
}
/*
 * Nuggets: client.c file
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



// can i have nr and nc as globals??????

int parseArgs(const int argc, char* argv[], char** hostname, char** port, char** playername);
bool handleInput(void* arg);
bool handleMessage(void* arg, const addr_t from, const char* message);
addr_t server_setup(char* hostname, char* port, char* playername);
void handle_display(const char* message);
void handle_quit(const char* message);
void handle_error(const char* message);
WINDOW* initDisplay();



typedef struct client_info{
    int NR;
    int NC;
    int map_nr;
    int map_nc;
    int collected;
    int purse;
    int remaining;
    WINDOW* win;
    char* playername;
    char playerletter;
    char* last_display;
} client_info_t;

client_info_t* client_info;

int main(int argc, char *argv[])
{
    char* hostname;
    char* port;
    char* playername = (argc == 4) ? argv[3] : NULL;

    if (parseArgs(argc, argv, &hostname, &port, &playername) != 0) {
        printf("Usage: ./client hostname port [playername]\n");
        return 1;
    }

    client_info = calloc(1, sizeof(client_info_t));

    // TODO/ PROBLEM initialize display
    client_info->win  = initDisplay();

    client_info->playername = playername;

    // initalize network + join the game
    addr_t server = server_setup(hostname, port, playername);

    // message stuff
    bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);

    // clean up message modual
    message_done();
    
    delwin(client_info->win);
    endwin();
    free(client_info);
    return ok? 0 : 10;
}


bool
handleMessage(void* arg, const addr_t from, const char* message)
{
    char messageType[256];
    sscanf(message, "%s", messageType);

    if (strcmp(messageType, "OK") == 0){
        
        sscanf(message, "%*s %c", &client_info->playerletter);
    
    } else if (strcmp(messageType, "GRID") == 0){
    
        int nrows, ncols;
        sscanf(message, "%*s %d %d", &nrows, &ncols);
        client_info->map_nc = ncols;
        client_info->map_nr= nrows;
        getmaxyx(stdscr, client_info->NR, client_info->NC);

        while (nrows +1 > client_info->NR || ncols > client_info->NC) {
            
            clear();
            mvprintw(0, 0, "Your window must be at least %d high", nrows+1);
            mvprintw(1, 0, "Your window must be at least %d wide", ncols);
            mvprintw(3, 0, "Current size: %d (height) x %d (width)", client_info->NR, client_info->NC);
            mvprintw(2, 0, "Resize your window, and press Enter to continue.");
           
            refresh();

            while (getch() != '\n') {
                // Wait for Enter key press
            }

            getmaxyx(stdscr, client_info->NR, client_info->NC);
            refresh();

        }

        wresize(client_info->win, client_info->NR, client_info->NC);
        mvprintw(7, 0, "Current size: %d (height) x %d (width)", client_info->NR, client_info->NC);
        
        
    } else if (strcmp(messageType, "GOLD") == 0){


        sscanf(message, "%*s %d %d %d", &client_info->collected, &client_info->purse, &client_info->remaining);
       

    } else if (strcmp(messageType, "DISPLAY") == 0){
        handle_display(message);

    } else if (strcmp(messageType, "QUIT") == 0){
        
        const char* spacePos = strchr(message, ' ');
        if (spacePos != NULL) {
            // Extract the remaining part of the message after the space
            const char* explanation = spacePos + 1;

            // Display the remaining message on the screen
            mvprintw(0, 0, "Server message: %s", explanation);
            refresh();
        }
        sleep(3);
        return true;
    
    } else if (strcmp(messageType, "ERROR") == 0) {

        handle_error(message);
        
    }

    return false;
}

void handle_display(const char* message)
{

    // Skip the "DISPLAY" part of the message
    const char* gridString = message + strlen("DISPLAY\n");

    // Clear the window and move to the top-left corner
    clear();
    move(1, 0);
        
        
    // Print each line of the grid string
    while (*gridString != '\0')
    {
        printw("%.*s", client_info->NC, gridString);  // Print one line
        gridString += client_info->NC;  // Move to the next line
    }

    // update status line
    char statusLine[256];

    if (client_info->playername != NULL) {
        snprintf(statusLine, sizeof(statusLine), "Player %c has %d nuggets (%d nuggets unclaimed).\n",
                 client_info->playerletter, client_info->purse, client_info->remaining);
    } else {
        snprintf(statusLine, sizeof(statusLine), "Spectator: %d nuggets unclaimed.\n", client_info->remaining);
    }

    // Print the status line at the top of the screen
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "%s", statusLine);

    char* tmp_display = (char*)message;
    client_info->last_display = tmp_display;
    
    // Refresh the window to show the changes
    refresh();
    
}

void 
handle_error(const char* message) 
{

    const char* spacePos = strchr(message, ' ');
    if (spacePos != NULL) {
        // Extract the remaining part of the message after the space
        const char* explanation = spacePos + 1;

        // Display the remaining message on the screen
        mvprintw(0, 0, "Server message-> %s", explanation);
        refresh();
    }
    sleep(1);
    handle_display(client_info->last_display);
 
}


bool 
handleInput(void* arg) {
    addr_t* server = arg;

    if (server == NULL) {
        log_v("handleInput called with arg=NULL");
        return true;
    }

    if (!message_isAddr(*server)) {
        log_v("handleInput called without a correspondent.");
        printf("You have no correspondent.\n");
        fflush(stdout);
        return false;
    }

    int ch = getch();  // Read a single character of input using getch()

    // Check if the character is a newline (Enter key)
    if (ch == '\n') {
        return false;  // Ignore newline character and continue waiting for input
    }

    // Format the message with the key
    char message[message_MaxBytes];
    snprintf(message, message_MaxBytes, "KEY %c", ch);

    // Send the message to the server
    message_send(*server, message);
    return false;
}

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
        exit(2); // bad hostname/port
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

    // Set the background color
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(1));
    refresh();

    // starter we will update later
    client_info->NR = 0;
    client_info->NC = 0;

    // Create a new window for the game display
    WINDOW* win = newwin(client_info->NR + 1, client_info->NC, 0, 0);
    wbkgd(win, COLOR_PAIR(1));
    refresh();
    return win;
}

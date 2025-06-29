#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

// Global variable to store original terminal settings
struct termios orig_termios;

// Restore terminal settings when program exits
void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}


// Enable raw mode (noncanonical mode, disable echo)
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);

    atexit(disable_raw_mode);  // Ensure settings are restored on exit
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}


int main(void) {
    int x = 10, y = 5;  // Starting cursor position
    char ch; // User input
    
    enable_raw_mode();
    
    // Clear screen before starting
    //printf("\033[2J");
    
    while(1) {
        // Clear the screen and move the cursor to home position
        printf("\033[H");  // Move to top left
        //printf("\033[2J"); // Clear the entire screen

        // Move the cursor to (y, x) and display the marker
        printf("\033[%d;%dH", y, x);
        //printf("X");
        fflush(stdout);    // Ensure output is shown immediately
        
        // Read a character from input
        ch = getchar();
        if(ch == 'q') break; // Exit if 'q' is pressed


        
        // Check for arrow key sequence (starts with escape character)
        //if(ch == '\033') { 
        //    getchar();  // Skip the '[' character
        //    ch = getchar();
        //    switch(ch) {
        //        case 'A': // Up arrow
        //            y = (y > 1 ? y - 1 : y);
        //            break;
        //        case 'B': // Down arrow
        //            y = y + 1;
        //            break;
        //        case 'C': // Right arrow
        //            x = x + 1;
        //            break;
        //        case 'D': // Left arrow
        //            x = (x > 1 ? x - 1 : x);
        //            break;
        //    }
        //}
    }
    
    // Reset terminal settings and clear screen before exiting
    disable_raw_mode();
    printf("\033[H");
    printf("\033[2J");
    printf("Exited.\n");
    return 0;
}

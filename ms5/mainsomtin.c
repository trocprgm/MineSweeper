#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ANSIescape.h"

//Colors
#define RED 240, 10, 10 
#define GREEN 10, 240, 10

struct termios orig_termios;
int wcol,wrow,x = 10, y = 10;

void disable_raw_mode() {
// Restore terminal settings when the program exits
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios); //apply orig_termios settings
}

void enable_raw_mode() {
// Enable raw mode (noncanonical mode and disable echo)
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);  // Ensure settings are restored on exit
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &raw); //apply raw settings
}

void get_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    wcol=w.ws_col;
    wrow=w.ws_row;
    printf("Rows: %d\n",wrow);
    printf("Columns: %d\n",wcol);
}

int quick_key() {
    char ch;
    ch = getchar();
    switch (ch) {
        case 'k':
            if(y>1) y--;
            break;
        case 'j':
            y++;
            break;
        case 'h':
            if(x>1) x--;
            break;
        case 'l':
            x++;
            break;
        case 'q':
            return 1;
    }
    return 0;
}


int main() {
    get_size();
    x=wcol/2;
    y=wrow/2;
    enable_raw_mode();
    chide();
    int runcond = 1;
    while (runcond) {
        chome();
        fgcolor(RED);
        printf("X: %d Y: %d",x,y);
        clearcolor();
        cpos(x,y);
        fgcolor(GREEN);
        printf("X");
        clearcolor();
        fflush(stdout);
        runcond -= quick_key();
    }
    disable_raw_mode();
    cshow();
    clearcolor();
    fflush(stdout);
    chome();
    clearterm();
    printf("Exited.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#define BLACK        0x00, 0x00, 0x00
#define BLUE         0x00, 0x00, 0xAA
#define GREEN        0x00, 0xAA, 0x00
#define CYAN         0x00, 0xAA, 0xAA
#define RED          0xAA, 0x00, 0x00
#define MAGENTA      0xAA, 0x00, 0xAA
#define BROWN        0xAA, 0x55, 0x00
#define LIGHTGRAY    0xAA, 0xAA, 0xAA
#define DARKGRAY     0x55, 0x55, 0x55
#define LIGHTBLUE    0x55, 0x55, 0xFF
#define LIGHTGREEN   0x55, 0xFF, 0x55
#define LIGHTCYAN    0x55, 0xFF, 0xFF
#define LIGHTRED     0xFF, 0x55, 0x55
#define LIGHTMAGENTA 0xFF, 0x55, 0xFF
#define YELLOW       0xFF, 0xFF, 0x55
#define WHITE        0xFF, 0xFF, 0xFF 

//This header file contains functions for all the ANSI escape codes so that I don't have to constantly fucking look at them
struct termios orig_termios;

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

void bgcolor(int red,int green, int blue) {
    printf("\033[48;2;%d;%d;%dm",red,green,blue);
}

void clearcolor() {
    printf("\e[m"); // reset color changes
}

void fgcolor(int red,int green,int blue) {
    printf("\033[38;2;%d;%d;%dm",red,green,blue);
}


void clearterm() {
    printf("\033[2J");
}
    
void chome() {
    printf("\033[H");
}

void cpos(int x, int y) {
    //Positions the cursor before print opperation
    // printf("X: %d Y: %d",x,y);
    printf("\033[%d;%dH", y, x);
}

void cshow() {
    printf("\e[?25h"); // show cursor */
}

void chide() {
    printf("\e[?25l");
}


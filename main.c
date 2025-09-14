#include "ANSIescape.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define BORDERSYM "&"
#define DEBUG false

#define gamewidth 100
#define gameheight 30
#define bombcount 400

int ghx, ghy, px = 10, py = 10, flags = 0, runcond = 1;
//there are two cordinate systems: global and field
//global position can be seen in the status bar as X and Y, field can be seen as fX and fY
//ghx ghy are the cordinates of the 'origin'
//px and py is the player cordinate in the global system

char openfield[gameheight][gamewidth];
char hiddenfield[gameheight][gamewidth];
char ringfield[gameheight][gamewidth];
//openfield represents the visible field
//hidden field represents the hidden bomb and number layer
//ringfield is a layer used to process large areas of blank cells

void outfield(char array[gameheight][gamewidth], const char *filename);
void get_size();
void action(int action);
void mossy(int j, int i);
bool safeplace(int j, int i);
void showfield(int j, int i);
int quick_key();
void flatten();
void renderfield(char field[gameheight][gamewidth]);
void renderplayer();
void setfield();
void gameloop();

void showfield(int j, int i) { openfield[j][i] = hiddenfield[j][i]; }

bool safeplace(int j, int i) {
    if ((0 <= i) && (i < gamewidth) && (0 <= j) && (j < gameheight)) return true;
    else return false;
}

void outfield(char array[gameheight][gamewidth], const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    for (int i = 0; i < gameheight; i++) {
        for (int j = 0; j < gamewidth; j++) {
            fprintf(file, "%c", array[i][j]);
            if (j < gamewidth - 1) {
                fprintf(file, "");
            }
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
}

void get_size() {
  int wcol, wrow;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  wcol = w.ws_col;
  wrow = w.ws_row;
  printf("Rows: %d\n", wrow);
  printf("Columns: %d\n", wcol);
  ghx = (wcol / 2) - (gamewidth / 2);
  ghy = (wrow / 2) - (gameheight / 2);
}


void action(int action) {
  int lpx = (px - ghx);
  int lpy = (py - ghy);
  char ch = openfield[lpy][lpx];
  char hch = hiddenfield[lpy][lpx];
  switch (action) {
  case 1:
    if (ch == ' ') {
      openfield[lpy][lpx] = 'F';
    }
    if (ch == 'F') {
      openfield[lpy][lpx] = ' ';
    }
    break;
    // flag
  case 2:
    if (hch == 'B')
      runcond = 2;
    if (hch == ' ') {
        mossy(lpy,lpx);
      break;
    } else
      showfield(lpy, lpx);
    break;
  }
}

int quick_key() {
  int gmx = ghx + (gamewidth - 1);
  int gmy = ghy + (gameheight - 1);
  //upperlimits on player travel
  char ch;
  ch = getchar();
  switch (ch) {
  case 'k':
    if ((py > ghy) | DEBUG)
      py--;
    break;
  case 'j':
    if ((py < gmy) | DEBUG)
      py++;
    break;
  case 'h':
    if ((px > ghx) | DEBUG)
      px--;
    break;
  case 'l':
    if ((px < gmx) | DEBUG)
      px++;
    break;
  case 'f':
    action(1);
    /* *input = 'F'; */
    break;
  case 'a':
    action(2);
    /* *input = ' '; */
    break;
  case 'q':
    return 1;
  }
  return 0;
}

void renderfield(char field[gameheight][gamewidth]) {
  clearcolor();
  fgcolor(BLUE);
  bgcolor(BLUE);
  cpos((ghx - 1), (ghy - 2));
  for (int i = 0; i <= (gamewidth + 1); i++) {
    printf(BORDERSYM);
  }
  cpos((ghx - 1), (ghy - 1));
  for (int i = 0; i <= (gamewidth + 1); i++) {
    printf(BORDERSYM);
  }
  for (int i = ghy; i < (ghy + gameheight); i++) {
    // chome();
    cpos((ghx - 1), i);
    printf(BORDERSYM);
    for (int i = 1; i <= gamewidth; i++) {
      printf(" ");
    }
    printf(BORDERSYM);
  }
  cpos((ghx - 1), ((ghy + (gameheight - 1)) + 1));
  for (int i = 0; i <= (gamewidth + 1); i++) {
    printf(BORDERSYM);
  }
  clearcolor();
  fgcolor(WHITE);
  bgcolor(BLUE);
  cpos(ghx, (ghy - 2));
  printf("X: %d Y: %d fX: %d fY: %d   Bombcount: %d  Flags: %d", px, py, (px - ghx),
         (py - ghy), bombcount, flags);
  cpos(ghx, (ghy - 1));
  fflush(stdout);
  flags = 0;
  cpos(ghx, ghy);
  for (int j = 0; j < gameheight; j++) {
    cpos(ghx, ghy + j);
    for (int i = 0; i < gamewidth; i++) {
      if (field[j][i] == ' ') {
        bgcolor(LIGHTCYAN);
        printf(" ");
      } else if (field[j][i] == 'F') {
        bgcolor(BLACK);
        fgcolor(RED);
        printf("%C", field[j][i]);
        flags++;
      } else if (!(field[j][i] == ' ')) {
        bgcolor(YELLOW);
        fgcolor(BLACK);
        printf("%C", field[j][i]);
      }
    }
  }
}

void renderplayer() {
  char ch = openfield[(py - ghy)][(px - ghx)];
  cpos(px, py);
  fgcolor(BLUE);
  bgcolor(WHITE);
  printf("%C", ch);
  clearcolor();
}

void setfield() {
  srand(time(NULL));
  int warehouse = bombcount;
  int squares = gamewidth * gameheight;
  float prob = (float)warehouse / (float)(squares);
  //Lays the bombs
  for (int j = 0; j < gameheight; j++) {
    for (int i = 0; i < gamewidth; i++) {
      float randfloat = (float)rand() / (float)RAND_MAX;
      if (warehouse > 0 && (prob > randfloat)) {
        warehouse--;
        hiddenfield[j][i] = 'B';
      } else {
        hiddenfield[j][i] = ' ';
      }
    }
  }
  //Creates the numbers
  for (int j = 0; j < gameheight; j++) {
    for (int i = 0; i < gamewidth; i++) {
      int count = 0;
      if (!(hiddenfield[j][i] == 'B')) {
        for (int a = -1; a <= 1; a++) {
          for (int b = -1; b <= 1; b++) {
        if (((i + a) >= 0) && ((j + b) >= 0) &&
            (hiddenfield[j + b][i + a] == 'B'))
          count++;
      }
    }
    hiddenfield[j][i] = '0' + count;
    if (count == 0)
      hiddenfield[j][i] = ' ';
  }
}
}
}

void gameloop() {
  py = ghy + (gameheight / 2);
  px = ghx + (gamewidth / 2);
  enable_raw_mode();
  chide();
  while (runcond == 1) {
    chome();
    bgcolor(LIGHTCYAN);
    // fgcolor(REDI);
    int r, g, b;
    // hextoint(RED,&r,&g,&b);
    renderfield(openfield);
    renderplayer();
    fflush(stdout);
    runcond -= quick_key();
  }
  disable_raw_mode();
  cshow();
  clearcolor();
  fflush(stdout);
  chome();
  if (!DEBUG) clearterm();
}

void mossy(int j, int i) {
    //mossy propigates revielded tiles starting at j and i
    //'V' is used as a marker char
    if (hiddenfield[j][i] == ' ') {
        ringfield[j][i] = 'V';
    }
    for (int a = 1; a <= 10; a++) {
        //moss loop
      for (int tj = 0; tj < gameheight; tj++) {
        for (int ti = 0; ti < gamewidth; ti++) {
            if (ringfield[tj][ti] == 'V') {
                openfield[tj][ti] = '~';
                for (int a = -1; a <= 1; a++) {
                  for (int b = -1; b <= 1; b++) {
                      if ((ringfield[tj+a][ti+b] == ' ') && safeplace((tj+a), (ti+b))) {
                          ringfield[tj+a][ti+b] = 'V';
                      } else if (!(hiddenfield[tj+a][ti+b] == ' ') && safeplace((tj+a), (ti+b))) {
                          showfield((tj+a),(ti+b));
                  }
                }
            }
        }
      }
    }
    }
    if (DEBUG) outfield(ringfield, "ringfield.txt");
}

int main() {
  // Setup
  get_size(); //Gets the size of the terminal window and sets global vars accordingly. This function is linux exclusive.
  setfield(); //set the hidden field
  memset(openfield, ' ', sizeof(openfield)); //sets the open field
  memcpy(ringfield, hiddenfield, sizeof(hiddenfield)); //sets the ringfield
  if (DEBUG) outfield(ringfield, "ringfield.txt"); //creates runtime text doc for ringfield
  gameloop(); //main gameloop
  // Game end check
  if (runcond == 2)
    printf("You've Blown up\n");
  else if (runcond == 3)
    printf("You've Won\n");
  printf("Exited.\n");
  return 0;
}

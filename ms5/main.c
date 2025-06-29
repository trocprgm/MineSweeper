#include "ANSIescape.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// Colors
#define REDI 240, 10, 10
#define GREENI 10, 240, 10
#define BORDERCOLOR 220, 220, 10
#define BORDERSYM "&"

// Game Variables
/* #define gamewidth 50 */
/* #define gameheight 30 */
/* #define bombcount 250 */
/* #define gamewidth 64 */
/* #define gameheight 16 */

#define gamewidth 100
#define gameheight 30
#define bombcount 400

int wcol, wrow, ghx, gmx, ghy, gmy, px = 10, py = 10, flags = 0, runcond = 1, boxxys = 0, boxxyso = 0, nc = 0;

char hiddenfield[gameheight][gamewidth];
char openfield[gameheight][gamewidth];

void showfield(int j, int i) { openfield[j][i] = hiddenfield[j][i]; }

void get_size() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  wcol = w.ws_col;
  wrow = w.ws_row;
  printf("Rows: %d\n", wrow);
  printf("Columns: %d\n", wcol);
  ghx = (wcol / 2) - (gamewidth / 2);
  ghy = (wrow / 2) - (gameheight / 2);
  gmx = ghx + (gamewidth - 1);
  gmy = ghy + (gameheight - 1);
}

void action(int action) {
  char ch = openfield[(py - ghy)][(px - ghx)];
  char hch = hiddenfield[(py - ghy)][(px - ghx)];
  switch (action) {
  case 1:
    if (ch == ' ') {
      openfield[(py - ghy)][(px - ghx)] = 'F';
    }
    if (ch == 'F') {
      openfield[(py - ghy)][(px - ghx)] = ' ';
    }
    break;
    // flag
  case 2:
    if (hch == 'B')
      runcond = 2;
    if (hch == ' ') {
      break;
    } else
      openfield[(py - ghy)][(px - ghx)] = hch;
    break;
    // test
  }
}

int quick_key() {
  char ch;
  ch = getchar();
  switch (ch) {
  case 'k':
    if (py > ghy)
      py--;
    break;
  case 'j':
    if (py < gmy)
      py++;
    break;
  case 'h':
    if (px > ghx)
      px--;
    break;
  case 'l':
    if (px < gmx)
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

void flatten() {
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
  cpos((ghx - 1), (gmy + 1));
  for (int i = 0; i <= (gamewidth + 1); i++) {
    printf(BORDERSYM);
  }
  clearcolor();
  fgcolor(WHITE);
  bgcolor(BLUE);
  cpos(ghx, (ghy - 2));
  printf("X: %d Y: %d   Bombcount: %d  Flags: %d  B: %d %d", (px - ghx),
         (py - ghy), bombcount, flags, boxxys, boxxyso);
  cpos(ghx, (ghy - 1));
  fflush(stdout);
}

void renderfield(char field[gameheight][gamewidth]) {
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
            // vv check if ring member
            // ringfield[i][j] = ' ';
            // buildboxring(i, j);
            // if (isvoidnei(i,j)) {
            //     nc++;
            // }
        // vv check if bomb
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
    flatten();
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
  clearterm();
}

void setopen() {
    for (int j = 0; j < gameheight; j++) {
        for (int i = 0; i < gamewidth; i++) {
            openfield[j][i] = ' ';
        }
    }
}

int main() {
  // Setup
  get_size(); //Gets the size of the terminal window and sets global vars accordingly. This function is linux exclusive.
  // memset(openfield, ' ', sizeof(openfield));
  setfield();
  // memcpy(openfield, hiddenfield, sizeof(hiddenfield));
  setopen();
  memcpy(openfield, hiddenfield, sizeof(hiddenfield));
  // Setup end

  gameloop();

  // Game end check
  if (runcond == 2)
    printf("You've Blown up\n");
  else if (runcond == 3)
    printf("You've Won\n");
  printf("Exited.\n");
  return 0;
}

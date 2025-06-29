#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ANSIescape.h"

//Colors
#define REDI 240, 10, 10 
#define GREENI 10, 240, 10
#define BORDERCOLOR 220, 220, 10
#define BORDERSYM "&"

//Game Variables
#define gamewidth 50
#define gameheight 30
/* #define bombcount 250 */
/* #define gamewidth 64 */
/* #define gameheight 16 */
#define bombcount 40


int wcol,wrow,ghx,gmx,ghy,gmy,px = 10, py = 10,flags = 0,runcond = 1,boxxys=0,boxxyso=0;
char tringa[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};  
char tringb[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};  
char tringc[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};  
char hiddenfield[gameheight][gamewidth];
char openfield[gameheight][gamewidth];


typedef struct {
    int i,j;
    short vects[8][2];
    short avects[8][2];
    char ring[8];
    short vva[8];
    short nva[8];
} box;

typedef struct {
    short sj,si;
    size_t nintc,ointc;
    char nint[100][2];
    char oint[100][2];
} moss;

void showfield(int j,int i) {
    openfield[j][i] = hiddenfield[j][i];
}

box buildbox(int i, int j) {
    box r = {
        i,
        j,
        {{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}},
        {{j-1,i-1}, {j-1,i}, {j-1,i+1}, {j,i+1}, {j+1,i+1}, {j+1,i}, {j+1,i-1}, {j,i-1}},
    }; 
    for (int a = 0; a < 8; a++) {
        short mj, mi;
        mj = r.avects[a][0];
        mi = r.avects[a][1];
        char tech = hiddenfield[mj][mi];
        r.ring[a] = tech;
        tringc[a] = tech;
        if (tech == ' ') {
            r.vva[a]=1;
            tringa[a]='1';
            }
        else tringa[a] = '0';
    }
    /* contructs void neighbor */ 
    for (int a = 0; a < 8; a++) {
        if (( a == 0 ) && ((' ' == r.ring[7]) || (' ' == r.ring[1]) ))  {
            r.nva[a] = 1;
            tringb[a] = '1';
        } else if (( a == 7 ) && ((' ' == r.ring[6]) || (' ' == r.ring[0]) )) {
            r.nva[a] = 1;
            tringb[a] = '1';
        } else if ((' ' == r.ring[a-1]) || (' ' == r.ring[a+1]))  {
            r.nva[a] = 1;
            tringb[a] = '1';
        }
    }
    /* moves void neighbor to openfield */
    for (int a = 0; a < 8; a++) {
        if(r.nva[a] == 1 && !(r.ring[a] == ' ')) {
            showfield((r.avects[a][0]),(r.avects[a][1]));
            tringb[a] = '0' + 1;
        } else tringb[a] = '0';
    }
    return r;
}


void mossy(moss * tmoss) {

}

void spread() {
    char ch = openfield[(py-ghy)][(px-ghx)];
    char hch = hiddenfield[(py-ghy)][(px-ghx)];
    int sj = py-ghy;
    int si = px-ghx;
    int mj, mi;
    if (si <= (gamewidth/2)) {
            //move right
        while (hiddenfield[sj][si] == ' ') {
            si++;
        }
    } else {
        while (hiddenfield[sj][si] == ' ') {
            //move left
            si--;
        }
    }
    if (0 <= si && si < gamewidth) {
        mi = si;
        mj = sj;
        boxxys = mj;
        boxxyso = mi;
        moss mossish;
        mossish.sj = mj;
        mossish.si = mi;
        mossish.nint[0][0] = mj;
        mossish.nint[0][1] = mi;
        mossish.nintc++;
        mossy(&mossish);
        /* openfield[mj][mi] = hiddenfield[mj][mi]; */
        showfield(mj,mi);
        buildbox(mi,mj);
    }
}

void get_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    wcol=w.ws_col;
    wrow=w.ws_row;
    printf("Rows: %d\n",wrow);
    printf("Columns: %d\n",wcol);
    ghx = (wcol/2)-(gamewidth/2);
    ghy = (wrow/2)-(gameheight/2);
    gmx = ghx + (gamewidth - 1);
    gmy = ghy + (gameheight - 1); 
}


void action(int action) {
    char ch = openfield[(py-ghy)][(px-ghx)];
    char hch = hiddenfield[(py-ghy)][(px-ghx)];
    switch (action) {
        case 1:
            if (ch == ' ') {
                openfield[(py-ghy)][(px-ghx)] = 'F';
            }
            if (ch == 'F') {
                openfield[(py-ghy)][(px-ghx)] = ' ';
            }
            break;
            //flag
        case 2:
            if (hch == 'B') runcond = 2;
            if (hch == ' ') {
                spread();
            }
            else openfield[(py-ghy)][(px-ghx)] = hch;
            break;
            //test
    }
}

int quick_key() {
    char ch;
    ch = getchar();
    switch (ch) {
        case 'k':
            if(py>ghy) py--;
            break;
        case 'j':
            if (py<gmy) py++;
            break;
        case 'h':
            if(px>ghx) px--;
            break;
        case 'l':
            if(px<gmx) px++;
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
    cpos((ghx-1),(ghy-2));
    for (int i = 0; i <= (gamewidth + 1); i++) {
        printf(BORDERSYM);
    }
    cpos((ghx-1),(ghy-1));
    for (int i = 0; i <= (gamewidth + 1); i++) {
        printf(BORDERSYM);
    }
    for (int i = ghy; i < (ghy + gameheight); i++) {
        //chome();
        cpos((ghx-1),i);
        printf(BORDERSYM);
        for (int i = 1; i <= gamewidth; i++) {
            printf(" ");
        }
        printf(BORDERSYM);
    }
    cpos((ghx-1),(gmy+1));
    for (int i = 0; i <= (gamewidth + 1); i++) {
        printf(BORDERSYM);
    }
    clearcolor();
    fgcolor(WHITE);
    bgcolor(BLUE);
    cpos(ghx,(ghy-2));
    printf("X: %d Y: %d   Bombcount: %d  Flags: %d  B: %d %d",( px -ghx),( py -ghy),bombcount,flags,boxxys,boxxyso);
    cpos(ghx,(ghy-1));
    fflush(stdout);
    printf("#");
    for (int i = 0; i < 8; i++) printf("%c",tringa[i]);
    printf("#    ");
    printf("#");
    for (int i = 0; i < 8; i++) printf("%c",tringb[i]);
    printf("#    ");
    printf("#");
    for (int i = 0; i < 8; i++) printf("%c",tringc[i]);
    printf("#");
}

void renderfield(char field[gameheight][gamewidth]) {
    flags = 0;
    cpos(ghx,ghy);
    for (int j = 0; j < gameheight; j++) {
        cpos(ghx,ghy + j);
        for (int i = 0; i < gamewidth; i++) {
            if (field[j][i] == ' ') {
                bgcolor(LIGHTCYAN);
                printf(" ");
            }
            else if ( field[j][i] == 'F') {
                bgcolor(BLACK);
                fgcolor(RED);
                printf("%C",field[j][i]);
                flags++;
            }
            else if ( !(field[j][i] == ' ') ) {
                bgcolor(YELLOW);
                fgcolor(BLACK);
                printf("%C",field[j][i]);
            }
        }
    }
}
void renderplayer() {
    char ch = openfield[(py-ghy)][(px-ghx)];
    cpos(px,py);
    fgcolor(BLUE);
    bgcolor(WHITE);
    printf("%C",ch);
    clearcolor();
}

void setfield() {
    srand(time(NULL));
    int warehouse = bombcount;
    int squares = gamewidth * gameheight;
    float prob = (float)warehouse / (float)(squares);
    for (int j = 0; j < gameheight; j++) {
        for (int i = 0; i < gamewidth; i++) {
            float randfloat = (float)rand() /(float)RAND_MAX;
            if (warehouse > 0 && (prob > randfloat)) {
                warehouse--;
                hiddenfield[j][i] = 'B';
            }
            else {
                hiddenfield[j][i] = ' ';
            }
        }
    }
    for (int j = 0; j < gameheight; j++) {
        for (int i = 0; i < gamewidth; i++) {
            int count = 0;
            if (!( hiddenfield[j][i] == 'B' )) {
                for (int a = -1; a <= 1; a++) {
                    for (int b =-1; b <= 1; b++) {
                        if (((i + a) >= 0) && ((j + b) >= 0) && (hiddenfield[j+b][i+a] == 'B')) count++; 
                    }
                }
                hiddenfield[j][i] = '0' + count;
                if (count == 0) hiddenfield[j][i] = ' ';
            }
        }
    }
}

void setopen() {
    for (int j = 0; j < gameheight; j++) {
        for (int i = 0; i < gamewidth; i++) {
            openfield[j][i] = ' ';
        }
    }
}

void gameloop() {
    py=ghy + (gameheight/2);
    px=ghx + (gamewidth/2);
    enable_raw_mode();
    chide();
    while (runcond == 1) {
        chome();
        bgcolor(LIGHTCYAN);
        //fgcolor(REDI);
        int r,g,b;
        //hextoint(RED,&r,&g,&b);
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

int main() {
    get_size();
    setfield();
    setopen();
    /* memcpy(openfield,hiddenfield,sizeof(hiddenfield)); */
    gameloop();
    if (runcond == 2) printf("You've Blown up\n");
    else if (runcond == 3) printf("You've Won\n");
    printf("Exited.\n");
    return 0;
}

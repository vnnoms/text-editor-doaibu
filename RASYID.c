#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"
#include "CALLISTA.h"
Editor E; 

// cursor
void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void moveCursor(int row, int col) {
    printf("\033[%d;%dH", row + 3, col +1);
}

// clear
void clearScreen() {
    printf("\033[2J"); 
    printf("\033[H"); 
}

void clearRows(int start, int end, int width) {
    for(int r = start; r <= end; r++) {
        printf("\033[%d;1H", r);
        printf("%*s", width, "");
    }
}

// render
void renderHeader() {
    printf("Text Editor Doa ibu");
    printf("\033[2;1H");
    printf("\n");
}

void renderScroll(Tab *TT, int a) {
    clearRows(3, 3 + SCREEN_HEIGHT - 1, MAX_COLS);
    moveCursor(0,0);

    for(int y=a; y<SCREEN_HEIGHT + a; y++) {
        printf("%s", TT->text[y]);
        printf("\n");
    }
}

// tab
void newTab() {
	if(E.n_tabs >= MAX_TABS) return; 
    Tab *TT = &E.tabs[E.n_tabs];       

    TT->text = malloc(sizeof(char*) * MAX_ROWS);  
	for(int i=0; i<MAX_ROWS; i++){
	    TT->text[i] = malloc(sizeof(char) * MAX_COLS);
	    for(int j=0; j<MAX_COLS; j++)
	        TT->text[i][j] = '\0';  
	}
	
    TT->cursor_y = 0;
    TT->cursor_x = 0;
    TT->filename[0] = '\0';        

    E.curr_tab = E.n_tabs;
    (E.n_tabs)++;
}

void swicthTab(Tab *TT, int tabLoc) {
    TT = &E.tabs[tabLoc];
}

// input handler
void inputCharHandler(Tab *TT, int c) {
    switch (c) { 
        // ctrl + q 
        case 17 : {
            exit(0);
            break;
        }

        case 6 : {
            clearScreen(); 
            findText(TT);
            
            printf("\nTekan tombol apa saja untuk kembali ke editor...");
            _getch(); 
            
            clearScreen();
            renderHeader();
            redrawText(TT);
            break;
        }
        default:
            // add character
            if (c >= 32 && c <= 126) {
                findPosNT(TT, setPosNTInsert);
                if(E.pos_Y >= 0) {
                    insert(TT, c);
                    redrawInsertText(TT);
                }
                
            }

            break;

    }
}

void arrowKeyHandler(Tab *TT,int c) {
    switch (c) {  
        // arrow down
        case 72 : {
            // ini mmasih perlu dikembangin lagi
            if(TT->cursor_y> 0) {
                if (TT->text[TT->cursor_y-1][TT->cursor_x] != '\0') TT->cursor_y--;
                else if(TT->text[TT->cursor_y-1][0] != '\0') {
                    setPosToNTRow(TT, TT->cursor_y-1, 'd');
                }
            }

            break;
        }

        // arrow up
        case 80 : {
            if(TT->cursor_y < MAX_ROWS - 1) {
                if (TT->text[TT->cursor_y+1][TT->cursor_x] != '\0') TT->cursor_y++;
                else if(TT->text[TT->cursor_y+1][0] != '\0') {
                    setPosToNTRow(TT, TT->cursor_y+1, 'u');
                }
            }
            
            break;
        }

        // arrow left
        case 75 : {
            if (TT->cursor_x != 0 || TT->cursor_y != 0) {
                if(TT->cursor_x == 0) {
                    TT->cursor_x = MAX_COLS-2;
                    TT->cursor_y--; 	
                } else TT->cursor_x--; 
            }
            
            break;
        }

        // arrow right
        case 77 : {
            if(TT->cursor_x >= MAX_COLS - 2) {
                TT->cursor_x=0;
                TT->cursor_y++;
            } else if(TT->cursor_x < MAX_COLS - 2 && TT->text[TT->cursor_y][TT->cursor_x] != '\0') TT->cursor_x++;
            
            break;
        }
            
        
        default:
            break;
    }
}

// find and set Null Terminator possision
void setPosNTInsert(int y, int x) {
    E.pos_Y = y;
    E.pos_X = x;
}

void setPosNTDel(int y, int x) {
    if(x==0) {
        E.pos_Y = y-1;
        E.pos_X = MAX_COLS-2;
    } else {
        E.pos_Y = y;
        E.pos_X = x-1;
    }
}

// find 
void findPosNT(Tab *TT, void (*setPosNT)(int, int)) {
    int cursor_x = TT->cursor_x;
    int cursor_y = TT->cursor_y;
    char **text = TT->text;

    for(int y = cursor_y; y < MAX_ROWS; y++) {
        for(int x = cursor_x; x < MAX_COLS; x++) {
            if(y == MAX_ROWS-1 && x == MAX_COLS-2 && text[y][x] != '\0') {
                E.pos_Y = -1;
                E.pos_X = -1;
                return;
            }

            if(x != (MAX_COLS-1)) {
                if(text[y][x] == '\0') {
                    setPosNT(y,x);
                    return;
                }
            }
        }

        cursor_x = 0;
    }
}

void setPosToNTRow(Tab *TT,int y, char c) {
    for(int x = 0; x < MAX_COLS; x++){
        if(TT->text[y][x] == '\0') {
            if(c == 'u') TT->cursor_y++;
            else if (c == 'd') TT->cursor_y--;
            TT->cursor_x= x;
            return;
        }
    }
}

// insert char
void insert(Tab *TT, int c) {
    int cursor_x = TT->cursor_x;
    int cursor_y = TT->cursor_y;
    int pos_X =  E.pos_X;
    int pos_Y = E.pos_Y;
    char **text = TT->text;

    int limit_x = 0;
    
    for(int y = pos_Y; y >= cursor_y; y--) {
        if(y == cursor_y) limit_x = cursor_x; 

        for(int x = pos_X; x >= limit_x; x--) {
            if(y+1 < MAX_ROWS && x == MAX_COLS-2) text[y+1][0] = text[y][x];
            else text[y][x+1] = text[y][x];
        }

        pos_X = MAX_COLS-2;
    }

    text[cursor_y][cursor_x] = c;
    TT->cursor_x++;
    
    if(cursor_x >= MAX_COLS-2) {
        TT->cursor_x = 0;
        TT->cursor_y++;
    }
}

void redrawInsertText(Tab *TT) {
    int a = TT->cursor_y-SCREEN_HEIGHT+1;
    if(a<0) a=0;

    hideCursor();
    renderScroll(TT, a);
    moveCursor(TT->cursor_y-a,TT->cursor_x);
    showCursor();
}
#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"
Editor E; 

// CURSOR

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void moveCursor(int row, int col) {
    printf("\033[%d;%dH", row + 3, col +1);
}

// SET CURSOR

void setPosToNTRow(Tab *TT,int y, char c) {
    for(int x = 0; x < MAX_COLS; x++){
        if(TT->text[y][0] == '\0' && !TT->isNewLine[y]) return;

        if(TT->text[y][x] == '\0') {
            if(c == 'u') TT->cursor_y++;
            else if (c == 'd') TT->cursor_y--;
            TT->cursor_x= x;
            return;
        }
    } 
}

// CLEAR

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

// RENDER

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

// REDRAW 

void redrawText(Tab *TT) {
    int a = TT->cursor_y-SCREEN_HEIGHT+1;
    if(a<0) a=0;

    hideCursor();
    renderScroll(TT, a);
    moveCursor(TT->cursor_y-a,TT->cursor_x);
    showCursor();
}

// TAB

void newTab() {
	if(E.n_tabs >= MAX_TABS) return; 
    Tab *TT = &E.tabs[E.n_tabs];       

    TT->text = malloc(sizeof(char*) * MAX_ROWS);  
	for(int i=0; i<MAX_ROWS; i++){
	    TT->text[i] = malloc(sizeof(char) * MAX_COLS);
	    for(int j=0; j<MAX_COLS; j++)
	        TT->text[i][j] = '\0';  
	}

    TT->isNewLine = malloc(sizeof(bool*) * MAX_ROWS);
    for(int i = 0; i < MAX_ROWS; i++) TT->isNewLine[i] = false;
	
    TT->isNewLine[0] = true;
    TT->cursor_y = 0;
    TT->cursor_x = 0;
    TT->filename[0] = '\0';        

    E.curr_tab = E.n_tabs;
    (E.n_tabs)++;
}

void swicthTab(Tab *TT, int tabLoc) {
    TT = &E.tabs[tabLoc];
}

// INPUT HANDLER

void inputCharHandler(Tab *TT, int c) {
    switch (c) { 
        // ctrl + q 
        case 17 : {
            exit(0);
            break;
        }

        default:

            // Enter
            if(c == 13) {
                if(!TT->isNewLine[MAX_ROWS-1]) {
                    newline(TT);
                    redrawText(TT);
                }
            }

            // ESC -> keluar text editor
            if (c == 27) {
                clearScreen();
                exit(0);
            }

            // add character
            if (c >= 32 && c <= 126) {
                findTailAfterCursor(TT, TT->cursor_y, TT->cursor_x);
                if(E.pos_Y >= 0 && !TT->isNewLine[MAX_ROWS-1]) {
                    insert(TT, TT->cursor_y, TT->cursor_x, c);
                    redrawText(TT);
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
                else {
                    setPosToNTRow(TT, TT->cursor_y-1, 'd');
                }
            }

            break;
        }

        // arrow up
        case 80 : {
            if(TT->cursor_y < MAX_ROWS - 1) {
                if (TT->text[TT->cursor_y+1][TT->cursor_x] != '\0') TT->cursor_y++;
                else {
                    setPosToNTRow(TT, TT->cursor_y+1, 'u');
                }
            }
            
            break;
        }

        // arrow left 
        case 75 : {
            if (TT->cursor_x != 0 || TT->cursor_y != 0) {
                if(TT->cursor_x == 0) {
                    setPosToNTRow(TT, TT->cursor_y-1, 'd');	
                } else TT->cursor_x--; 
            }
            
            break;
        }

        // arrow right
        case 77 : {
            bool *NL = TT->isNewLine;
            int cursor_y = TT->cursor_y;

            if(TT->cursor_x < MAX_COLS - 1 && TT->text[TT->cursor_y][TT->cursor_x] != '\0') TT->cursor_x++;
            else if((TT->cursor_x >= MAX_COLS - 1) || NL[cursor_y+1]) {
                TT->cursor_x=0;
                TT->cursor_y++;
            } 
            
            break;
        }
            
        
        default:
            break;
    }
}

// FIND

// Mencari posisi karakter terakhir di baris independent (isNewline = true) tertentu
void findTailAfterCursor(Tab *TT, int cursor_y, int cursor_x) {
    char **text = TT->text;

    // Intial value buat kalo tidak ketemu karakter terakhir
    int lastY = -1;
    int lastX = -1;

    // CASE 1: posisi awal (0,0) dan kosong
    if (cursor_y == 0 && cursor_x == 0 && text[0][0] == '\0') {
        E.pos_Y = 0;
        E.pos_X = 0;
        return;
    }

    // cari batas bawah (stop di newline / baris kosong)
    for (int y = cursor_y + 1; y < MAX_ROWS; y++) {
        if (TT->isNewLine[y] || text[y][0] == '\0') {
            lastY = y - 1;
            break;
        }
    }

    // CASE 2: kalau tidak ketemu boundary (berarti sampai bawah semua kepake)
    if (lastY == -1) {
        lastY = MAX_ROWS - 1;
    }

    // 🔍 cari karakter terakhir di baris lastY
    for (int x = MAX_COLS - 2; x >= 0; x--) {
        if (text[lastY][x] != '\0') {
            lastX = x;
            break;
        }
    }

    // CASE 3: kalau baris ternyata kosong
    // if (lastX == -1) {
    //     lastY = cursor_y;
    //     lastX = cursor_x;
    // }

    // CASE 4: FULL BUFFER (ga ada space lagi)
    if (lastY == MAX_ROWS - 1 && lastX == MAX_COLS - 2 &&
        text[lastY][lastX] != '\0') {

        E.pos_Y = -1;
        E.pos_X = -1;
        return;
    }

    // SET posisi akhir
    E.pos_Y = lastY;
    E.pos_X = lastX;
}

// Mencari baris independent setelah baris kursor
int findRowNLFromTop(Tab *TT, int cursor_y) {
    for(int y = cursor_y + 1; y < MAX_ROWS;y++) {
        if(TT->isNewLine[y]) return y;
    }

    return -1;
}

// Mencari baris paling bawah yang aktif (isNewline = true atau karakter awal baris != '\0)
int findLastRowFromDown(Tab *TT, int cursor_y) {
    for(int y=MAX_ROWS-1; y>=cursor_y;y--) {
        if(TT->text[y][0] != '\0' || TT->isNewLine[y]) return y;
    }
    return cursor_y;
}

// INSERT

void insert(Tab *TT, int cursor_y, int cursor_x, int c) {
    int pos_X =  E.pos_X;
    int pos_Y = E.pos_Y;
    char **text = TT->text;
    int rowNL = -1;
    
    // Jika posisi karakter berada di karakter kosong, maka tidak diswap
    if(text[cursor_y][cursor_x] != '\0') {

        // swap character
        for(int y = pos_Y; y >= cursor_y; y--) {
            int limit_x = (y == cursor_y) ? cursor_x : 0;

            for(int x = pos_X; x >= limit_x; x--) {
                if(y+1 < MAX_ROWS && x == MAX_COLS-2) {

                    // jika kursor berada di baris ujung karakter kalimat, di kolom MAX_COLS - 1,  dan dibawahnya adalah baris independent (isNewline = true),
                    // maka semua baris dibawah baris ini turun sebanyak 1 baris

                    if(y == pos_Y && (rowNL = findRowNLFromTop(TT, y)) >= 0){
                        int lastRow = findLastRowFromDown(TT, y);
                        if(lastRow == MAX_ROWS-1) return;
                        // Note: move mulai dari last row
                        moveDown1Row(TT, lastRow, rowNL);
                    }
                    text[y+1][0] = text[y][x];
                }else if (x+1 < MAX_COLS-1) {
                    text[y][x+1] = text[y][x];
                }
            }

            pos_X = MAX_COLS-2;
        }
    } else if (cursor_x == MAX_COLS-1) {
        // jika kursor berada di kolom MAX_COLS - 1 dan dibawahnya adalah baris independent (isNewline = true),
        // maka semua baris dibawah baris ini turun sebanyak 1 baris
        if((rowNL = findRowNLFromTop(TT, cursor_y)) >= 0){
            int lastRow = findLastRowFromDown(TT, cursor_y);
            if(lastRow == MAX_ROWS-1) return;
            // Note: move mulai dari last row
            moveDown1Row(TT, lastRow, rowNL);
        }
    }

    // set cursor menuju baris di bawahnya dan pada kolom paling pertama,
    // jika kolom kursor = MAX_COLS - 1
    if(TT->cursor_x >= MAX_COLS-1) {
        TT->cursor_x = 0;
        TT->cursor_y++;
    }

    // Menambahkan karakter
    text[TT->cursor_y][TT->cursor_x] = c;

    // set kolom cursor_x ke kolom sebelahanya (setelah insert karakter)
    TT->cursor_x++;
}

// NEWLINE

// Membuat baris baru
void newline(Tab *TT) {
    int cursor_x = TT->cursor_x;
    int cursor_y = TT->cursor_y;
    char **text = TT->text;
    bool *NL = TT->isNewLine;

    // jika kursor berada di baris tidak independent (isNewline = false) serta di kolom pertama,
    // maka set baris tsb menjadi independet
    if(!NL[cursor_y] && cursor_x == 0) {
        NL[cursor_y] = true;
    } else {

        // kalau baris bawah ada newline -> geser dulu
        if(NL[cursor_y+1] == true) {
            int lastRow = findLastRowFromDown(TT, cursor_y);
            moveDown1Row(TT, lastRow, cursor_y+1);
        }

        // pindahin karakter setelah kursor (yang dienter) dari BELAKANG  ke depan (baris independent baru)
        for(int x = MAX_COLS-2; x >= cursor_x; x--) {
            if(text[cursor_y][x] == '\0') continue;

            char temp = text[cursor_y][x];
            text[cursor_y][x] = '\0'; // hapus dari baris lama

            TT->cursor_y = cursor_y + 1;
            TT->cursor_x = 0;

            findTailAfterCursor(TT, cursor_y + 1, 0); // biar insert tau tail
            insert(TT, cursor_y + 1, 0, temp);
        }
    

        // set newline
        NL[cursor_y+1] = true;

        // update cursor
        TT->cursor_y = cursor_y + 1;
        TT->cursor_x = 0;
    }
}

// REMOVE

// Hapus semua karakter yang berada di setelah kursor yg sedang dienter
void removeCache1Row(Tab *TT, int row, int col) {
    char **text = TT->text;
    for(int x = col; x < MAX_COLS; x++) {
        if (text[row][x] == '\0') break;
        text[row][x] = '\0';
    }
}


// MOVE

// turun 1 baris
void moveDown1Row(Tab *TT, int down, int top) {
    // start from down (move from down) :v
    char **text = TT->text;
    for(int y=down; y >= top; y--) {
        for(int x = 0; x < MAX_COLS; x++) {
            text[y+1][x] = text[y][x];
        }
    }

    moveDownStatus(TT, down, top);
    removeCache1Row(TT, top, 0);    
}

// setelah nurunin baris, status isNewline juga perlu turun
void moveDownStatus(Tab *TT, int down, int top) {
    bool *NL = TT->isNewLine;
    for(int i = down; i >= top; i--) {
        if(i == MAX_ROWS-1) return;

        if(NL[i] == true) {
            NL[i+1] = true;
            NL[i] = false;            
        }
    }
}
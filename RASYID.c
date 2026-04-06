#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"
#include "CALLISTA.h"
#include "RAMA.h"
Editor E; 

// OTHERS

// cek apakah CTRL ditekan
int isCtrlPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

// cek apakah ALT ditekan
int isAltPressed() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000);
}

// FREE

void freeTab(Tab *TT) {
    for(int i = 0; i < MAX_ROWS; i++) {
        free(TT->text[i]);
    }
    free(TT->text);
    free(TT->isNewLine);
}

// RESET

void resetTab(Tab *TT) {
    memset(TT, 0, sizeof(Tab));
}

// SET

void setStructEditor() {
    for(int i = 0; i < MAX_TABS; i++) resetTab(&E.tabs[i]);
    E.curr_tab = 0;
    E.n_tabs = 0;
    E.pos_Y = 0;
    E.pos_X = 0;
}

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
   system("cls");
}

void clearRows(int start, int end, int width) {
    for(int r = start; r <= end; r++) {
        printf("\033[%d;1H", r);
        printf("%*s", width, "");
    }
}

// RENDER

void renderHeader() {
    printf("\033[1;1H");   // pindah ke baris 1
    printf("\033[K"); 

    printf("Text Editor Doa ibu  ");
    for(int i = 0; i < E.n_tabs; i++) {
        if (i == E.curr_tab) printf("[Tab *%d] ", i +1);
        else printf("[Tab %d] ", i+1);
    }

    printf("\033[2;1H");
    printf("\n");

    fflush(stdout);
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

void addTab() {
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

    E.curr_tab = E.n_tabs;
    (E.n_tabs)++;
}

void deleteTab(int idx) {
    if(idx != 0 || E.tabs[idx].text[0][0] != '\0' || E.n_tabs > 0) {
        Tab *TT = &E.tabs[idx];

        if(E.n_tabs == 1 && idx == 0) {
            for(int i=0; i<MAX_ROWS; i++){
                for(int j=0; j<MAX_COLS; j++)
                    TT->text[i][j] = '\0';  
            }

            for(int i = 0; i < MAX_ROWS; i++) TT->isNewLine[i] = false;
            TT->isNewLine[0] = true;
            TT->cursor_y = 0;
            TT->cursor_x = 0;
            TT->filename[0] = '\0';

            return;
        }

        // free + reset dulu
        freeTab(TT);
        resetTab(&E.tabs[idx]);

        // geser
        for(int i = idx; i < E.n_tabs - 1; i++) {
            E.tabs[i] = E.tabs[i+1];
        }

        if(idx < E.n_tabs - 1) {
            Tab *LastTab = &E.tabs[E.n_tabs - 1];
            resetTab(LastTab);
        } else if(idx > 0) {
            E.curr_tab--;
        }

        if(E.n_tabs>1) E.n_tabs--;
    }
}

void swicthTab(Tab **TT, int tabLoc) {
    if(tabLoc >= 0 && tabLoc < E.n_tabs) {
        clearScreen();

        *TT = &E.tabs[tabLoc];
        E.curr_tab = tabLoc;

        renderHeader();
        redrawText(*TT);
    } 
}

// INPUT HANDLER

void inputCharHandler(Tab *TT, int c) {
    switch (c) { 
        case  1 : { // Ctrl + A
            swicthTab(&TT, E.curr_tab - 1);
            break;
        }

        case  4 : { // Ctrl + D
            swicthTab(&TT, E.curr_tab + 1);
            break;
        }
        case 5: {// Ctrl + E 
            deleteLine(TT);
            break;
        }

        case 8: {// 
            findTailAfterCursor(TT, TT->cursor_y, TT->cursor_x);
            if(E.pos_Y >= 0) {
                delete(TT, TT->cursor_y, TT->cursor_x);
                redrawText(TT);
            }

            break;
        }

        // Enter
        case  13: {
            if(!TT->isNewLine[MAX_ROWS-1]) {
                newline(TT);
                redrawText(TT);
            }

            break;
        }

        case  14: {
            addTab();
            swicthTab(&TT, E.curr_tab);
            break;
        }

        case 19: // Kode ASCII untuk Ctrl + S
            saveFile();
            break;

        case 23: // Kode ASCII untuk Ctrl + W (Simpan Sebagai)
            saveFileAs();
            break;

        // ESC -> keluar text editor
        case  27: {
            for(int i=0; i<E.n_tabs; i++) freeTab(&E.tabs[i]);
            clearScreen();
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

            if(tolower(c) == 'd') {
                if (isAltPressed()) {
                    deleteTab(E.curr_tab);
                    swicthTab(&TT, E.curr_tab);
                }
            }

            if (c >= '1' && c <= '5') {
                if (isAltPressed()) {
                    int tab = (c - '0') - 1;
                    swicthTab(&TT, tab);
                }
            }

            // add character
            if (c >= 32 && c <= 126) {
                if (!isAltPressed()) {
                    findTailAfterCursor(TT, TT->cursor_y, TT->cursor_x);
                    if(E.pos_Y >= 0 && !TT->isNewLine[MAX_ROWS-1]) {
                        insert(TT, TT->cursor_y, TT->cursor_x, c);
                        redrawText(TT);
                    }
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

    // CASE 3: FULL BUFFER (ga ada space lagi)
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

int findNTXInRow(Tab *TT, int y) {
    char **text = TT->text;
    for(int x = 0; x < MAX_COLS -1; x++) {
        if(text[y][x] == '\0') return x;
    }

    return -1;
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
            moveDown1Row(TT, cursor_y+1, lastRow);
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

// MERGE

void merge2Rows(Tab *TT, int cursor_y, int cursor_x) {
    int pos_Y = E.pos_Y;
    int pos_X = E.pos_X;
    char **text = TT->text;
    int rowNL = -1;

    int merged_Y = cursor_y - 1;
    int merged_X = findNTXInRow(TT, merged_Y);
    int init_merged_Y = merged_Y;
    int init_merged_X = merged_X;

    if(merged_X >= 0){
        // swap character
        for(int y = cursor_y; y <= pos_Y; y++) {
            int limit_x = (y == pos_Y) ? pos_X : MAX_COLS - 2;

            for(int x = cursor_x; x <= limit_x; x++) {
                if(merged_X == MAX_COLS - 1) {
                    merged_X = 0;
                    merged_Y++;    
                }

                text[merged_Y][merged_X] = text[y][x];

                merged_X++;
            }

            cursor_x = 0;
        }

        TT->isNewLine[cursor_y] = false;

        if(merged_Y == pos_Y - 1) {
            removeCache1Row(TT, merged_Y, merged_X);
            removeCache1Row(TT, pos_Y, 0);

            if((rowNL = findRowNLFromTop(TT, merged_Y + 1)) >= 0) {
                int lastRow = findLastRowFromDown(TT, merged_Y);
                moveUp1Row(TT, rowNL, lastRow);
            }
        } else {
            removeCache1Row(TT, merged_Y, merged_X);
        }

        TT->cursor_x = init_merged_X;
        TT->cursor_y = init_merged_Y;
    }
}


// INSERT

void insert(Tab *TT, int cursor_y, int cursor_x, int c) {
    int pos_X =  E.pos_X;
    int pos_Y = E.pos_Y;
    char **text = TT->text;
    int rowNL = -1;
    
    // Jika posisi karakter berada di karakter kosong, maka tidak diswap
    if(text[cursor_y][cursor_x] != '\0' || (cursor_x == MAX_COLS-1 && text[cursor_y+1][0] !=0 && !TT->isNewLine[cursor_y+1])) {
        swipeRight(TT, cursor_y, cursor_x);
    } else if (cursor_x == MAX_COLS-1) {

        // jika kursor berada di kolom MAX_COLS - 1 dan dibawahnya adalah baris independent (isNewline = true),
        // maka semua baris dibawah baris ini turun sebanyak 1 baris
        if((rowNL = findRowNLFromTop(TT, cursor_y)) >= 0){
            int lastRow = findLastRowFromDown(TT, cursor_y);
            if(lastRow == MAX_ROWS-1) return;
            // Note: move mulai dari last row
            moveDown1Row(TT, rowNL, lastRow);
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

// DELETE

void delete(Tab *TT, int cursor_y, int cursor_x) {
    char **text = TT->text;

    if (cursor_y == 0 && cursor_x == 0) return;

    int rowNL = -1;
    int isMerge = 0;

    if(cursor_x <= 0) {
        if(TT->isNewLine[cursor_y]) {
            if(TT->isNewLine[cursor_y - 1] && text[cursor_y - 1][0] == '\0') {
                int lastRow = findLastRowFromDown(TT, cursor_y);
                moveUp1Row(TT, cursor_y, lastRow);

                TT->cursor_y--;
                return;
            } else if(text[cursor_y][0] == '\0' && !TT->isNewLine[cursor_y]){
                int x = -1;
                TT->cursor_y = cursor_y - 1;
                
                if((x = findNTXInRow(TT, TT->cursor_y)) >= 0) {
                    TT->cursor_x = x;
                } else {
                    TT->cursor_x = MAX_COLS - 1;
                }

                TT->isNewLine[cursor_y] = false;
                int lastRow = findLastRowFromDown(TT, cursor_y);
                moveUp1Row(TT, cursor_y + 1, lastRow);

                return;
                
            } else if (text[cursor_y - 1][MAX_COLS - 2] != '\0') {
                TT->isNewLine[cursor_y] = false;

                return;
            } else isMerge = 1;
        }
    }

    if(isMerge < 1) {
        if(TT->cursor_x <= 0) {
            // ini tandain
            TT->cursor_x = MAX_COLS-2;
            TT->cursor_y--;
            
        } else TT->cursor_x--;
        

        // Delete karakter
        text[TT->cursor_y][TT->cursor_x] = '\0';

        if(text[cursor_y][cursor_x] != '\0' || (cursor_x == MAX_COLS-1 && text[cursor_y+1][0] !=0 && !TT->isNewLine[cursor_y+1])) {
            swipeleft(TT, cursor_y, cursor_x);
        } else if(TT->cursor_x <= 0 && !TT->isNewLine[TT->cursor_y]) {
            TT->cursor_x = MAX_COLS-1;
            TT->cursor_y--;

            if((rowNL = findRowNLFromTop(TT, cursor_y + 1)) >= 0) {
                int lastRow = findLastRowFromDown(TT, cursor_y);
                moveUp1Row(TT, rowNL, lastRow);
            }                   
        }

    } else {
        merge2Rows(TT, cursor_y, cursor_x);
    }    
}

// REMOVE

void removeCache1Row(Tab *TT, int row, int col) {
    char **text = TT->text;
    for(int x = col; x < MAX_COLS-1; x++) {
        if (text[row][x] == '\0') break;
        text[row][x] = '\0';
    }
}

// SWIPE

void swipeleft(Tab *TT, int cursor_y, int cursor_x) {
    int pos_Y = E.pos_Y;
    int pos_X = E.pos_X;
    char **text = TT->text;

    int needMoveUp = 0;
    int rowNL = -1;

    for(int y = cursor_y; y <= pos_Y; y++) {
        int limit_x = (y == pos_Y) ? pos_X : MAX_COLS - 1;

        for(int x = cursor_x; x <= limit_x; x++) {

            if(x == MAX_COLS-1) {
                text[y][MAX_COLS-2] = text[y+1][0];

                if(y == pos_Y - 1) {
                    needMoveUp = 1;
                }

            } else if(x == 0) {
                text[y-1][MAX_COLS-2] = text[y][0];

                if(y == pos_Y) {
                    needMoveUp = 1;
                }
            } else {
                text[y][x-1] = text[y][x];
            }

            if(y == pos_Y - 1) {
                needMoveUp = 1;
            }
        }

        cursor_x = 1;
    }

    // hapus tail
    text[pos_Y][pos_X] = '\0';

    // EKSEKUSI SEKALI DI AKHIR
    if(needMoveUp && pos_X == 0) {
        rowNL = findRowNLFromTop(TT, pos_Y);
        if(rowNL >= 0) {
            int lastRow = findLastRowFromDown(TT, pos_Y);
            moveUp1Row(TT, rowNL, lastRow);
        }
    }
}

void swipeRight(Tab *TT, int cursor_y, int cursor_x) {
    int pos_X =  E.pos_X;
    int pos_Y = E.pos_Y;
    char **text = TT->text;
    int rowNL = -1;

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
                    moveDown1Row(TT, rowNL, lastRow);
                }
                text[y+1][0] = text[y][x];
            }else if (x+1 < MAX_COLS-1) {
                text[y][x+1] = text[y][x];
            }
        }

        pos_X = MAX_COLS-2;
    }
}

// MOVE

// turun 1 baris
void moveDown1Row(Tab *TT, int top, int bottom) {
    // start from bottom (move from bottom) :v
    if(bottom == MAX_ROWS-1) return;

    char **text = TT->text;
    for(int y=bottom; y >= top; y--) {
        for(int x = 0; x < MAX_COLS; x++) {
            text[y+1][x] = text[y][x];
        }
    }

    moveDownStatus(TT, top, bottom);
    removeCache1Row(TT, top, 0);    
}

void moveDownStatus(Tab *TT, int top, int bottom) {
    if(bottom == MAX_ROWS-1) return;

    bool *NL = TT->isNewLine;

    for(int i = bottom; i >= top; i--) {
        NL[i+1] = NL[i];
    }

    NL[top] = false;
}

void moveUp1Row(Tab *TT, int top, int bottom) {
    if(top == 0) return;

    char **text = TT->text;
    for(int y=top; y <= bottom; y++) {
        for(int x = 0; x < MAX_COLS; x++) {
            text[y-1][x] = text[y][x];
        }
    }

    moveUpStatus(TT, top, bottom);
    removeCache1Row(TT, bottom, 0);    
}

void moveUpStatus(Tab *TT, int top, int bottom) {
    if(top == 0) return;

    bool *NL = TT->isNewLine;

    for(int i = top; i <= bottom; i++) {
        NL[i-1] = NL[i];   // copy semua, bukan cuma true
    }

    NL[bottom] = false; // clear terakhir
}
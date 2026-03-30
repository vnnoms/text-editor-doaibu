#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"  
#include "RAMA.h"

void clearSpecificRow(Tab *TT, int baris) {
    for (int kolom = 0; kolom < MAX_COLS; kolom++) {
        TT->text[baris][kolom] = '\0';
    }
}

void shiftRowsUp(Tab *TT, int start_row) {
    int lastRow = findLastRowFromDown(TT, start_row);

    for (int y = start_row; y < lastRow; y++) {
        strcpy(TT->text[y], TT->text[y + 1]); 
        TT->isNewLine[y] = TT->isNewLine[y + 1];
    }
    
    clearSpecificRow(TT, lastRow);
    TT->isNewLine[lastRow] = false;
}

void deleteLine(Tab *TT) {
    int curr_y = TT->cursor_y;
    clearSpecificRow(TT, curr_y);
    shiftRowsUp(TT, curr_y);
    TT->cursor_x = 0;
    redrawText(TT);
}
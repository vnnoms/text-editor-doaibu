#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"  
#include "RAMA.h"

void newline(Tab *TT) {
    if (TT->cursor_y < MAX_ROWS - 1) {
        
        for (int i = MAX_ROWS - 1; i > TT->cursor_y + 1; i--) {
            strcpy(TT->text[i], TT->text[i-1]);
        }

        strcpy(TT->text[TT->cursor_y + 1], &TT->text[TT->cursor_y][TT->cursor_x]);
        TT->text[TT->cursor_y][TT->cursor_x] = '\0';

        TT->cursor_y++;
        TT->cursor_x = 0;

	redrawInsertText(TT); 
    }
}
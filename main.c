#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"

int main() {
    setStructEditor();
    addTab();
    // Tab *TT = &E.tabs[E.curr_tab];
    
    clearScreen();
    renderHeader();

    while(1) {
        Tab *TT = &E.tabs[E.curr_tab];
        
        int c = _getch();

        if (c == 0 || c == 224) {
            c = _getch();
            arrowKeyHandler(TT, c);
            int a = TT->cursor_y-SCREEN_HEIGHT+1;
            if(a<0) a=0;

            // selalu hide cursor dulu sebelum render text ke cli 
            // lalu show lagi cursornya
            hideCursor();
            renderScroll(TT, a);
            moveCursor(TT->cursor_y-a,TT->cursor_x);
            showCursor();
        } else {
            inputCharHandler(TT, c);
        }
    }

    return 0;
}
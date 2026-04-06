#ifndef RASYID_h
#define RASYID_h

#include "CONFIG.h"

// cursor 
void hideCursor();
void showCursor();
void moveCursor(int row, int col);

// clear
void clearScreen();
void clearRows(int start, int end, int width);

// render
void renderHeader();
void renderScroll(Tab *TT, int a);

// tab
void newTab();
void swicthTab(Tab *TT, int tabLoc);

// input handler
void inputCharHandler(Tab *TT, int c);
void arrowKeyHandler(Tab *TT, int c);

// find and set Null Terminator possision
void setPosNTInsert(int y, int x);
void setPosNTDel(int y, int x);

void findPosNT(Tab *TT, void (*setPosNT)(int, int));
void setPosToNTRow(Tab *TT,int y, char c);

// insert char
void insert(Tab *TT, int c);
// void redrawInsertText(Tab *TT);
void redrawText(TT);

#endif
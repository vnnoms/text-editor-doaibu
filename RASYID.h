#ifndef RASYID_h
#define RASYID_h

#include "CONFIG.h"

// OTHERS
int isCtrlPressed();
int isAltPressed();

// FREE
void freeTab(Tab *TT);

// RESET
void resetTab(Tab *TT);

// SET
void setStructEditor();

// CURSOR
void hideCursor();
void showCursor();
void moveCursor(int row, int col);

// SET CURSOR
void setPosToNTRow(Tab *TT,int y, char c);

// CLEAR
void clearScreen();
void clearRows(int start, int end, int width);

// RENDER 
void renderHeader();
void renderScroll(Tab *TT, int a);

// REDRAW 
void redrawText(Tab *TT);

// TAB
void addTab();
void deleteTab(int idx);
void swicthTab(Tab **TT, int tabLoc);

// INPUT HANDLER
void inputCharHandler(Tab *TT, int c);
void arrowKeyHandler(Tab *TT, int c);

// FIND
void findTailAfterCursor(Tab *TT,int cursor_y,int cursor_x);
int findRowNLFromTop(Tab *TT, int cursor_y); // NL = New Line hehw
int findLastRowFromDown(Tab *TT, int cursor_y);
int findNTXInRow(Tab *TT, int y);

// NEWLINE
void newline(Tab *TT);

// MERGE
void merge2Rows(Tab *TT, int cursor_y, int cursor_x);

// INSERT
void insert(Tab *TT,int cursor_y,int cursor_x ,int c);

// DELETE
void delete(Tab *TT, int cursor_y, int cursor_x);

// REMOVE
void removeCache1Row(Tab *TT, int row, int col);

// SWIPE
void swipeleft(Tab *TT, int cursor_y, int cursor_x);
void swipeRight(Tab *TT, int cursor_y, int cursor_x);

// MOVE
void moveDown1Row(Tab *TT, int top, int bottom);
void moveDownStatus(Tab *TT, int top, int bottom);

void moveUp1Row(Tab *TT, int top, int bottom);
void moveUpStatus(Tab *TT, int top, int bottom);


#endif





#ifndef CONFIG_h
#define CONFIG_h

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

// config var global 
#define MAX_ROWS 100
#define MAX_COLS 6
#define MAX_TABS 5
#define MAX_PATH 260
#define SCREEN_HEIGHT 5

// define tab
typedef struct {
    char **text;
    // int total_char;
    int cursor_y;
    int cursor_x;
    char filename[MAX_PATH];
} Tab;

// define globar struct 
typedef struct {
    Tab tabs[MAX_TABS];
    int curr_tab;
    int n_tabs;
    int pos_Y;
    int pos_X;
} Editor;
 
extern Editor E;     

#endif
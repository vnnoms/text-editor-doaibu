#ifndef RAMA_h
#define RAMA_h
#include "CONFIG.h"

// fungsi untuk fitur newline
void clearSpecificRow(Tab *TT, int row);         
void shiftRowsUp(Tab *TT, int start_row);      
void deleteLine(Tab *TT);

bool tulisDataKeFile(Tab *TT, const char *jalur_file);
bool tampilkanDialogSimpan(char *alamat_tujuan);
                      
#endif
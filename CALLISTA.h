#ifndef CALLISTA_H
#define CALLISTA_H
#include "CONFIG.h"


bool openFileDialog(char *alamat_tujuan);
bool bacaFileKeTab(Tab *TT, const char *lokasi_file);
void bukaFile(Tab *TT);
void quitEditor();
char* cariAbaikanCase(char *teks, char *kataCari);
void findText(Tab *TT);

#endif

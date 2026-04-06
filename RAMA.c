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

bool tulisDataKeFile(Tab *TT, const char *jalur_file) {
    FILE *berkas = fopen(jalur_file, "w");
    if (berkas == NULL) return false;

    int baris_terakhir = findLastRowFromDown(TT, 0);

    for (int i = 0; i <= baris_terakhir; i++) {
        fputs(TT->text[i], berkas);

        if (i < baris_terakhir && TT->isNewLine[i + 1]) {
            fputc('\n', berkas);
        }
    }

    fclose(berkas);
    return true;
}

bool tampilkanDialogSimpan(char *alamat_tujuan) {
    OPENFILENAME jendela_simpan;
    char nama_file_pilihan[MAX_PATH] = {0};
    ZeroMemory(&jendela_simpan, sizeof(jendela_simpan));
    
    jendela_simpan.lStructSize = sizeof(jendela_simpan);
    jendela_simpan.hwndOwner = GetConsoleWindow();
    jendela_simpan.lpstrFile = nama_file_pilihan;
    jendela_simpan.nMaxFile = sizeof(nama_file_pilihan);
    jendela_simpan.lpstrFilter = "File Teks (*.txt)\0*.txt\0";
    jendela_simpan.nFilterIndex = 1;
    jendela_simpan.lpstrDefExt = "txt";
    jendela_simpan.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileName(&jendela_simpan)) {
        strcpy(alamat_tujuan, nama_file_pilihan);
        return true;
    }
    return false;
}

void saveFile() {
    Tab *tab_aktif = &E.tabs[E.curr_tab];

    if (tab_aktif->filename[0] == '\0') {
        saveFileAs();
        return;
    }

    if (tulisDataKeFile(tab_aktif, tab_aktif->filename)) {
        MessageBox(NULL, "File Berhasil Disimpan!", "Notepad Doa Ibu", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(NULL, "Gagal Menulis File!", "Error", MB_OK | MB_ICONERROR);
    }
}

void saveFileAs() {
    Tab *tab_aktif = &E.tabs[E.curr_tab];
    char alamat_baru[MAX_PATH];

    if (tampilkanDialogSimpan(alamat_baru)) {
        strcpy(tab_aktif->filename, alamat_baru);
        saveFile(); 
    }
    clearScreen();
    renderHeader();
    redrawText(tab_aktif);
}

int countTotalChars(Tab *TT) {
    int count = 0;
    int barisTerakhir = findLastRowFromDown(TT, 0);

    for (int i = 0; i <= barisTerakhir; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            if (TT->text[i][j] == '\0') break; 
            count++;
        }
    }
    return count;
}

int countTotalWords(Tab *TT) {
    int count = 0;
    bool inWord = false;
    int lastRow = findLastRowFromDown(TT, 0); 

    for (int i = 0; i <= lastRow; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            char c = TT->text[i][j];
            if (c == '\0') break; 
            if (isspace(c)) {
                inWord = false;
            } else if (!inWord) {
                inWord = true;
                count++;
            }
        }
    }
    return count;
}


int countCurrentRowChars(Tab *TT) {
    int count = 0;
    int row = TT->cursor_y; 

    for (int j = 0; j < MAX_COLS; j++) {
        if (TT->text[row][j] == '\0') break; 
        count++;
    }
    return count;
}

int countActiveLines(Tab *TT) {
    int barisTerakhir = findLastRowFromDown(TT, 0);
    
    if (barisTerakhir == 0 && TT->text[0][0] == '\0' && !TT->isNewLine[0]) {
        return 0;
    }
    
    return barisTerakhir + 1;
}

void renderFooter(Tab *TT) {
    char statsBuffer[256];
    snprintf(statsBuffer, sizeof(statsBuffer),
    "Tab: %d | Chars: %d | Words: %d | Lines: %d | Col: %d",E.curr_tab + 1,countTotalChars(TT),countTotalWords(TT),countActiveLines(TT),TT->cursor_x);
    printf("\033[s");
    printf("\033[%d;1H\033[K%s", 3 + SCREEN_HEIGHT + 1, statsBuffer);
    printf("\033[u");
    fflush(stdout);
}
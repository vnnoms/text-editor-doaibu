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

    // Bersihkan memori struktur data
    ZeroMemory(&jendela_simpan, sizeof(jendela_simpan));
    
    // Konfigurasi teknis jendela dialog Windows
    jendela_simpan.lStructSize = sizeof(jendela_simpan);
    jendela_simpan.hwndOwner = GetConsoleWindow();
    jendela_simpan.lpstrFile = nama_file_pilihan;
    jendela_simpan.nMaxFile = sizeof(nama_file_pilihan);
    jendela_simpan.lpstrFilter = "File Teks (*.txt)\0*.txt\0Semua File (*.*)\0*.*\0";
    jendela_simpan.nFilterIndex = 1;
    jendela_simpan.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    // Eksekusi pemanggilan jendela dialog
    if (GetSaveFileName(&jendela_simpan)) {
        
        // Pastikan format .txt terpasang
        if (strstr(nama_file_pilihan, ".txt") == NULL && strstr(nama_file_pilihan, ".TXT") == NULL) {
            strcat(nama_file_pilihan, ".txt");
        }
        
        // Salin hasil pilihan ke parameter output
        strcpy(alamat_tujuan, nama_file_pilihan);
        return true;
    }
    
    return false;
}
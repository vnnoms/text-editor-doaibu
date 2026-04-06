#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include <ctype.h>
#include "CONFIG.h"
#include "RAMA.h"
#include "RASYID.h"

#define MAX_BARIS 100
#define MAX_KARAKTER 255
extern char bufferTeks[MAX_BARIS][MAX_KARAKTER];
extern int jumlahBaris;

//Open File Dialog untuk memilih file teks
bool openFileDialog(char *alamat_tujuan) {
    OPENFILENAME ofn;
    char szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        printf("\n[Sistem] File yang dipilih: %s\n", ofn.lpstrFile);
        strcpy(alamat_tujuan, ofn.lpstrFile);
        return true;
    } else {
        printf("\n[Sistem] Batal memilih file.\n");
        return false;
    }
}

bool bacaFileKeTab(Tab *TT, const char *lokasi_file) {
    FILE *f = fopen(lokasi_file, "r");
    if (f == NULL) return false;

    // Bersihkan dulu seluruh isi Tab sebelum diisi data baru
    for (int i = 0; i < MAX_ROWS; i++) {
        clearSpecificRow(TT, i);
        TT->isNewLine[i] = false;
    }

    char c;
    int baris = 0;
    int kolom = 0;
    
    // Baris pertama selalu dianggap awal paragraf baru
    TT->isNewLine[0] = true;

    while ((c = fgetc(f)) != EOF && baris < MAX_ROWS) {
        if (c == '\n') {
            // Jika bertemu '\n', pindah ke baris baru dan tandai sebagai paragraf baru
            baris++;
            kolom = 0;
            if (baris < MAX_ROWS) {
                TT->isNewLine[baris] = true;
            }
        } else {
            // Jika kolom penuh, otomatis pindah baris tapi BUKAN paragraf baru (Word Wrap)
            if (kolom >= MAX_COLS - 1) {
                baris++;
                kolom = 0;
                if (baris < MAX_ROWS) {
                    TT->isNewLine[baris] = false; // Ini sambungan, bukan Enter
                }
            }
            
            if (baris < MAX_ROWS) {
                TT->text[baris][kolom] = c;
                kolom++;
                TT->text[baris][kolom] = '\0'; // Selalu jaga ujung teks dengan null
            }
        }
    }

    fclose(f);
    return true;
}

void bukaFile() {
    Tab *tabAktif = &E.tabs[E.curr_tab];
    char jalur_baru[MAX_PATH];

    // 1. Munculkan jendela pilih file
    if (openFileDialog(jalur_baru)) {
        // 2. Baca isinya ke dalam memori
        if (bacaFileKeTab(tabAktif, jalur_baru)) {
            // 3. Simpan lokasinya agar nanti tinggal 'Save' saja (bukan Save As)
            strcpy(tabAktif->filename, jalur_baru);
            
            // 4. Reset kursor ke awal dokumen
            tabAktif->cursor_x = 0;
            tabAktif->cursor_y = 0;
            
            MessageBox(NULL, "File Berhasil Dimuat!", "Notepad Doa Ibu", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBox(NULL, "Gagal membaca file!", "Error", MB_OK | MB_ICONERROR);
        }
    }

    // Segarkan tampilan layar
    clearScreen();
    renderHeader();
    redrawText(tabAktif);
}

void quitEditor() {
    char confirm;
    printf("\nApakah kamu yakin ingin keluar? (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        for(int i=0; i<E.n_tabs; i++) freeTab(&E.tabs[i]);
        clearScreen();
        printf("\nTerima kasih telah menggunakan Text Editor Doa Ibu!\n");
        exit(0); 
    }
}
char* cariAbaikanCase(char *teks, char *kataCari) {
    if (!*kataCari) return teks;
    for (; *teks; ++teks) {
        if (tolower(*teks) == tolower(*kataCari)) {
            char *h = teks, *n = kataCari;
            while (*h && *n && tolower(*h) == tolower(*n)) {
                ++h; ++n;
            }
            if (!*n) return teks; 
        }
    }
    return NULL; 
}

void findText(Tab *TT) {
    char cari[100];
    
    printf(">> Masukkan kata yang dicari: ");
    scanf("%s", cari); 

    int cariLen = strlen(cari);
    int ketemuTotal = 0;
    const char *BG_YELLOW = "\033[43;30m"; 
    const char *RESET = "\033[0m";

    printf("\n--- Hasil Pencarian ---\n");

    for (int i = 0; i < MAX_ROWS; i++) {
        char *barisSaatIni = TT->text[i];
        char *posisiMatch;
        if (cariAbaikanCase(barisSaatIni, cari) != NULL) {
            printf("Baris %d: ", i + 1);
            char *cetakPtr = barisSaatIni;

            while ((posisiMatch = cariAbaikanCase(cetakPtr, cari)) != NULL) {
                ketemuTotal++; 
                int panjangTeksBiasa = posisiMatch - cetakPtr;
                printf("%.*s", panjangTeksBiasa, cetakPtr);
                printf("%s%.*s%s", BG_YELLOW, cariLen, posisiMatch, RESET);
                cetakPtr = posisiMatch + cariLen;
            }
            printf("%s\n", cetakPtr);
        }
    }
    if (ketemuTotal == 0) {
        printf("[!] Teks '%s' tidak ditemukan.\n", cari);
    } else {
        printf("\n[i] Selesai! Ditemukan %d kecocokan.\n", ketemuTotal);
    }
}

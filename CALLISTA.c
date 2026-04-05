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
void openFileDialog() {
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
    } else {
        printf("\n[Sistem] Batal memilih file.\n");
    }
}

void quitEditor() {
    char confirm;
    printf("\nApakah kamu yakin ingin keluar? (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
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

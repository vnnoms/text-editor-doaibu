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

void findText(Tab *TT) { //buat dia connected ke fitur replace teks. logika salah satu fitur kalau udah find dan mau replace gak perlu find lagi.
    //higlight ketika di baris yang akan diedit, dan apabila sudah di edit langsung find kalimat sebelumnya yang telah ditemukan dan telah diganti 
    //akan tidak terdeteksi lagi oleh mesin pencari. 
    char cari[100];
    printf("\n>> Masukkan kata utuh yang dicari: ");
    scanf(" %[^\n]", cari);

    int cariLen = strlen(cari);
    int ketemuTotal = 0;
    const char *BG_YELLOW = "\033[43;30m"; 
    const char *RESET = "\033[0m";

    printf("\n--- Hasil Pencarian ---\n");

    for (int i = 0; i < MAX_ROWS; i++) {
        char *barisSaatIni = TT->text[i]; 
        char *posisiMatch;
        int barisIniAdaMatch = 0;
        char *tempPtr = barisSaatIni;
        while ((tempPtr = strstr(tempPtr, cari)) != NULL) {
            int hurufSebelumnyaAman = (tempPtr == barisSaatIni) || !isalnum(*(tempPtr - 1));
            int hurufSetelahnyaAman = (*(tempPtr + cariLen) == '\0') || (*(tempPtr + cariLen) == '\n') || !isalnum(*(tempPtr + cariLen));

            if (hurufSebelumnyaAman && hurufSetelahnyaAman) {
                barisIniAdaMatch = 1;
                ketemuTotal++;
            }
            tempPtr += cariLen; 
        }
        if (barisIniAdaMatch) {
            printf("Baris %d: ", i + 1);

            char *cetakPtr = barisSaatIni;
            while ((posisiMatch = strstr(cetakPtr, cari)) != NULL) {
                int hurufSebelumnyaAman = (posisiMatch == barisSaatIni) || !isalnum(*(posisiMatch - 1));
                int hurufSetelahnyaAman = (*(posisiMatch + cariLen) == '\0') || (*(posisiMatch + cariLen) == '\n') || !isalnum(*(posisiMatch + cariLen));

                if (hurufSebelumnyaAman && hurufSetelahnyaAman) {
                    int panjangTeksBiasa = posisiMatch - cetakPtr;
                    printf("%.*s", panjangTeksBiasa, cetakPtr);
                    printf("%s%s%s", BG_YELLOW, cari, RESET);
                    cetakPtr = posisiMatch + cariLen;
                } else {
                    int panjangTeksBiasa = (posisiMatch - cetakPtr) + 1;
                    printf("%.*s", panjangTeksBiasa, cetakPtr);
                    cetakPtr = posisiMatch + 1;
                }
            }
            printf("%s\n", cetakPtr);
        }
    }

    if (ketemuTotal == 0) {
        printf("[!] Kata utuh '%s' tidak ditemukan dalam dokumen.\n", cari);
    } else {
        printf("\n[i] Selesai! Ditemukan %d kecocokan untuk kata '%s'.\n", ketemuTotal, cari);
    }
}

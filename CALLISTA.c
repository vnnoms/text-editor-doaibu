#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include "RAMA.h"

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
    printf("\nApakah kamu yakin ingin keluar? Pastikan perubahan sudah di-save. (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        printf("\nTerima kasih telah menggunakan Text Editor Doa Ibu!\n");
        exit(0); 
}
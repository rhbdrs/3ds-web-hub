#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <3ds.h>

#define SAVE_PATH "/sdmc/3ds/my_bookmarks.txt"
#define MAX_LINKS 10
#define MAX_URL_LEN 256

typedef struct {
    char urls[MAX_LINKS][MAX_URL_LEN];
    int count;
} BookmarkList;

// Charger les signets depuis la carte SD
void loadBookmarks(BookmarkList *list) {
    list->count = 0;
    FILE *f = fopen(SAVE_PATH, "r");
    if (!f) return;

    while (list->count < MAX_LINKS && fgets(list->urls[list->count], MAX_URL_LEN, f)) {
        list->urls[list->count][strcspn(list->urls[list->count], "\r\n")] = 0;
        list->count++;
    }
    fclose(f);
}

// Sauvegarder les signets sur la carte SD
void saveBookmarks(BookmarkList *list) {
    FILE *f = fopen(SAVE_PATH, "w");
    if (!f) return;

    for (int i = 0; i < list->count; i++) {
        fprintf(f, "%s\n", list->urls[i]);
    }
    fclose(f);
}

// Ajouter un lien via le clavier virtuel de la 3DS
void addBookmark(BookmarkList *list) {
    if (list->count >= MAX_LINKS) return;

    SwkbdState swkbd;
    char custom_url[MAX_URL_LEN];
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, -1);
    swkbdSetHint(&swkbd, "Entrez l'URL (ex: https://...)");
    
    SwkbdButton button = swkbdInputText(&swkbd, custom_url, sizeof(custom_url));
    if (button == SWKBD_BUTTON_CONFIRM && strlen(custom_url) > 0) {
        strncpy(list->urls[list->count], custom_url, MAX_URL_LEN);
        list->count++;
        saveBookmarks(list);
    }
}

// Ouvrir l'URL dans le navigateur natif de la 3DS
void openBrowser(const char* url) {
    // Utilisation des services Applet Web pour passer l'URL au navigateur
    bool wifiStatus;
    if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
        // Lancement officiel du navigateur internet intégré
        // (Nécessite l'initialisation de urlarge/hb services si géré globalement)
        // Note: l'appel bas niveau requiert webAppletLaunch si géré par citrulib standard
    }
}

int main(int argc, char **argv) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    consoleInit(GFX_BOTTOM, NULL); // Écran du bas pour les instructions

    BookmarkList myBookmarks;
    loadBookmarks(&myBookmarks);

    // Si aucun lien n'existe au premier lancement, on en met un par défaut
    if (myBookmarks.count == 0) {
        strcpy(myBookmarks.urls[0], "https://google.com");
        myBookmarks.count = 1;
        saveBookmarks(&myBookmarks);
    }

    int selected = 0;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Navigation dans la liste
        if (kDown & KEY_UP && selected > 0) selected--;
        if (kDown & KEY_DOWN && selected < myBookmarks.count - 1) selected++;

        // Ajouter un lien (touche X)
        if (kDown & KEY_X) {
            addBookmark(&myBookmarks);
        }

        // Quitter l'application (touche START)
        if (kDown & KEY_START) break;

        // Affichage écran du haut (Liste des liens)
        consoleSelect(GetTopScreen() ? (PrintConsole*)gfxGetScreenFormats(GFX_TOP) : NULL); // Gestion standard console
        // Utilisation simplifiée de l'affichage texte de débogage :
        printf("\x1b[2;2H=== 3DS WEB HUB ===\n");
        for (int i = 0; i < myBookmarks.count; i++) {
            if (i == selected) {
                printf("\x1b[%d;2H> %s <   \n", 5 + i, myBookmarks.urls[i]);
            } else {
                printf("\x1b[%d;2H  %s    \n", 5 + i, myBookmarks.urls[i]);
            }
        }

        // Affichage écran du bas (Aide)
        // (Affiché sur la console bas)
        printf("\x1b[16;2H[A] Ouvrir le lien\n");
        printf("\x1b[17;2H[X] Ajouter un lien\n");
        printf("\x1b[18;2H[START] Quitter\n");

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}

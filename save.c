#include <stdio.h>
#include <string.h>
#include "save.h"

#define SAVE_MAGIC "CYCO"
#define SAVE_VERSION 1

int save_game(const GameState *state, const char *filename) {
    FILE *file;
    int version = SAVE_VERSION;

    file = fopen(filename, "wb");
    if (file == NULL) {
        return 0;
    }

    if (fwrite(SAVE_MAGIC, sizeof(char), 4, file) != 4) {
        fclose(file);
        return 0;
    }

    if (fwrite(&version, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    if (fwrite(state, sizeof(GameState), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int load_game(GameState *state, const char *filename) {
    FILE *file;
    char magic[5];
    int version;

    file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }

    if (fread(magic, sizeof(char), 4, file) != 4) {
        fclose(file);
        return 0;
    }
    magic[4] = '\0';

    if (strcmp(magic, SAVE_MAGIC) != 0) {
        fclose(file);
        return 0;
    }

    if (fread(&version, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    if (version != SAVE_VERSION) {
        fclose(file);
        return 0;
    }

    if (fread(state, sizeof(GameState), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int save_exists(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return 1;
}

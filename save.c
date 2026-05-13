#include <stdio.h>
#include "save.h"
#include "constants.h"

/*
    Sauvegarde la partie dans un fichier texte.

    Le fichier contiendra :
    - le nombre de joueurs
    - le joueur actuel
    - le numéro du tour
    - le contenu du plateau
*/
int save_game(const Game *game) {
    FILE *file;
    int row;
    int column;

    file = fopen(SAVE_FILE, "w");

    if (file == NULL) {
        printf("Erreur : impossible de sauvegarder la partie.\n");
        return FALSE;
    }

    fprintf(file, "%d %d %d\n",
            game->player_count,
            game->current_player,
            game->turn_number);

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            fputc(game->board[row][column], file);
        }

        fputc('\n', file);
    }

    fclose(file);

    printf("Partie sauvegardee dans %s.\n", SAVE_FILE);

    return TRUE;
}

/*
    Charge une partie depuis le fichier save.txt.
*/
int load_game(Game *game) {
    FILE *file;
    int row;
    int column;
    int character;

    file = fopen(SAVE_FILE, "r");

    if (file == NULL) {
        printf("Aucune sauvegarde trouvee.\n");
        return FALSE;
    }

    if (fscanf(file, "%d %d %d\n",
               &game->player_count,
               &game->current_player,
               &game->turn_number) != 3) {
        fclose(file);
        printf("Sauvegarde invalide.\n");
        return FALSE;
    }

    if (game->player_count < MIN_PLAYERS || game->player_count > MAX_PLAYERS) {
        fclose(file);
        printf("Sauvegarde invalide : nombre de joueurs incorrect.\n");
        return FALSE;
    }

    if (game->current_player < 1 || game->current_player > game->player_count) {
        fclose(file);
        printf("Sauvegarde invalide : joueur actuel incorrect.\n");
        return FALSE;
    }

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            character = fgetc(file);

            if (character == EOF ||
                (character != EMPTY_CELL &&
                 character != BLOCK_CELL &&
                 character != 'X' &&
                 character != 'O' &&
                 character != 'A')) {
                fclose(file);
                printf("Sauvegarde invalide : contenu du plateau incorrect.\n");
                return FALSE;
            }

            game->board[row][column] = (char) character;
        }

        character = fgetc(file);

        if (character != '\n' && character != EOF) {
            fclose(file);
            printf("Sauvegarde invalide : format du plateau incorrect.\n");
            return FALSE;
        }
    }

    fclose(file);

    printf("Partie restauree avec succes.\n");

    return TRUE;
}

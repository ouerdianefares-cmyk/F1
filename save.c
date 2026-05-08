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

    /*
        On ouvre le fichier en mode écriture "w".

        Si le fichier n’existe pas, il est créé.
        S’il existe déjà, son ancien contenu est remplacé.
    */
    file = fopen(SAVE_FILE, "w");

    if (file == NULL) {
        printf("Erreur : impossible de sauvegarder la partie.\n");
        return FALSE;
    }

    /*
        Première ligne du fichier :
        nombre de joueurs, joueur actuel, numéro du tour.
    */
    fprintf(file, "%d %d %d\n",
            game->player_count,
            game->current_player,
            game->turn_number);

    /*
        Ensuite, on écrit toutes les cases du plateau.
    */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            fputc(game->board[row][column], file);
        }

        /*
            Retour à la ligne après chaque ligne du plateau.
        */
        fputc('\n', file);
    }

    /*
        On ferme le fichier.
    */
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
    char character;

    /*
        On ouvre le fichier en lecture "r".
    */
    file = fopen(SAVE_FILE, "r");

    if (file == NULL) {
        printf("Aucune sauvegarde trouvee.\n");
        return FALSE;
    }

    /*
        On lit les informations principales de la sauvegarde.
    */
    if (fscanf(file, "%d %d %d\n",
               &game->player_count,
               &game->current_player,
               &game->turn_number) != 3) {
        fclose(file);
        printf("Sauvegarde invalide.\n");
        return FALSE;
    }

    /*
        On vérifie que les informations lues sont correctes.
    */
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

    /*
        On lit le plateau case par case.
    */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            character = fgetc(file);

            /*
                On vérifie que chaque caractère lu est autorisé.
            */
            if (character != EMPTY_CELL &&
                character != BLOCK_CELL &&
                character != 'X' &&
                character != 'O' &&
                character != 'A') {
                fclose(file);
                printf("Sauvegarde invalide : contenu du plateau incorrect.\n");
                return FALSE;
            }

            game->board[row][column] = character;
        }

        /*
            On lit le retour à la ligne après chaque ligne du plateau.
        */
        fgetc(file);
    }

    fclose(file);

    printf("Partie restauree avec succes.\n");

    return TRUE;
}

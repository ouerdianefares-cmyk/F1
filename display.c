#include <stdio.h>
#include "display.h"
#include "constants.h"

/*
    Efface l’écran du terminal.

    Les codes \033[2J et \033[H sont des codes ANSI :
    - \033[2J efface l’écran
    - \033[H remet le curseur en haut à gauche
*/
void clear_screen(void) {
    printf("\033[2J");
    printf("\033[H");
}

/*
    Met le programme en pause.

    getchar() attend que l’utilisateur appuie sur Entrée.
*/
void pause_screen(void) {
    printf("\nAppuyez sur ENTREE pour continuer...");
    getchar();
}

/*
    Donne un symbole à chaque joueur.

    Joueur 1 : X
    Joueur 2 : O
    Joueur 3 : A
*/
char get_player_symbol(int player) {
    if (player == 1) {
        return 'X';
    }

    if (player == 2) {
        return 'O';
    }

    return 'A';
}

/*
    Affiche le plateau du jeu dans le terminal.
*/
void display_game(const Game *game) {
    int row;
    int column;

    printf("\n");
    printf("      CY-CONNECT\n");

    /*
        Affichage des numéros de colonnes.
    */
    printf("   ");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf(" %d", column + 1);
    }

    printf("\n   ");

    /*
        Ligne de séparation du haut.
    */
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("--");
    }

    printf("-\n");

    /*
        Affichage de chaque case du plateau.
    */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        printf(" %d |", row + 1);

        for (column = 0; column < BOARD_WIDTH; column++) {
            printf(" %c", game->board[row][column]);
        }

        printf(" |\n");
    }

    /*
        Ligne de séparation du bas.
    */
    printf("   ");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("--");
    }

    printf("-\n");

    /*
        Informations sur la partie.
    */
    printf("\nJoueur actuel : %d (%c)\n",
           game->current_player,
           get_player_symbol(game->current_player));

    printf("Tour : %d\n", game->turn_number);
}

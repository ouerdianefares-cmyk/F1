#include <stdio.h>

#include "display.h"
#include "constants.h"


/* ================================
   Codes de style pour le terminal
   ================================ */

/* Remet le texte normal */
#define RESET   "\033[0m"

/* Texte en gras */
#define BOLD    "\033[1m"

/* Texte plus discret */
#define DIM     "\033[2m"


/* ================================
   Couleurs du texte
   ================================ */

/* Couleur rouge : utilisée pour le joueur 1 */
#define RED     "\033[91m"

/* Couleur bleue : utilisée pour le joueur 2 */
#define BLUE    "\033[96m"

/* Couleur jaune : utilisée pour le joueur 3 */
#define YELLOW  "\033[93m"

/* Couleur verte */
#define GREEN   "\033[92m"

/* Couleur violette : utilisée pour afficher le pivot */
#define PURPLE  "\033[95m"

/* Couleur grise : utilisée pour les blocs */
#define GREY    "\033[90m"

/* Couleur blanche */
#define WHITE   "\033[97m"


/* ================================
   Couleur de fond
   ================================ */

/* Fond vert : utilisé pour montrer les cases gagnantes */
#define BG_WIN  "\033[42m"


/* ================================
   Nettoyage de l'écran
   ================================ */

void clear_screen(void) {
    /* Efface tout le contenu affiché dans le terminal */
    printf("\033[2J");

    /* Replace le curseur en haut à gauche du terminal */
    printf("\033[H");
}


/* ================================
   Pause dans le jeu
   ================================ */

void pause_screen(void) {
    /* Affiche un message et attend que le joueur appuie sur ENTREE */
    printf("\nAppuyez sur ENTREE pour continuer...");
    getchar();
}


/* ================================
   Symbole des joueurs
   ================================ */

char get_player_symbol(int player) {
    /* Le joueur 1 utilise le symbole X */
    if (player == 1) {
        return 'X';
    }

    /* Le joueur 2 utilise le symbole O */
    if (player == 2) {
        return 'O';
    }

    /* Le joueur 3 utilise le symbole A */
    return 'A';
}


/* ================================
   Affichage du titre du jeu
   ================================ */

static void print_title(void) {
    /* Affiche le nom du jeu avec une couleur et un encadrement */
    printf(BOLD BLUE "\n");
    printf("   ╔══════════════════════════════╗\n");
    printf("   ║          CY-CONNECT          ║\n");
    printf("   ╚══════════════════════════════╝\n");
    printf(RESET);
}


/* ================================
   Affichage d'une case du plateau
   ================================ */

static void print_cell_content(char cell) {
    /*
       Cette fonction affiche le contenu d'une case.
       Selon le caractère contenu dans la case, on affiche
       un symbole différent avec une couleur différente.
    */

    if (cell == EMPTY_CELL) {
        /* Case vide */
        printf(DIM " · " RESET);
    } else if (cell == BLOCK_CELL) {
        /* Case bloquée */
        printf(GREY BOLD " ▓ " RESET);
    } else if (cell == 'X') {
        /* Pion du joueur 1 */
        printf(RED BOLD " ✖ " RESET);
    } else if (cell == 'O') {
        /* Pion du joueur 2 */
        printf(BLUE BOLD " ◉ " RESET);
    } else if (cell == 'A') {
        /* Pion du joueur 3 */
        printf(YELLOW BOLD " ▲ " RESET);
    } else {
        /* Affichage normal si le caractère est inconnu */
        printf(" %c ", cell);
    }
}


/* ================================
   Affichage avancé du jeu
   ================================ */

static void display_game_advanced(const Game *game,
                                  int has_pivot,
                                  Position pivot,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    int row;
    int column;
    int is_winning_cell;

    /*
       Cette fonction affiche le plateau complet.

       Elle peut aussi afficher :
       - le pivot de rotation avec la lettre P
       - les cases gagnantes avec un fond vert

       Paramètres :
       - game : contient le plateau et les informations du jeu
       - has_pivot : TRUE si on veut afficher le pivot
       - pivot : position du pivot à afficher
       - winning_cells : tableau indiquant les cases gagnantes
    */

    print_title();

    /* Affiche les numéros des colonnes en haut du plateau */
    printf("\n      ");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf(" %d ", column + 1);
    }

    /* Affiche le bord supérieur du plateau */
    printf("\n    ╔");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("═══");
    }
    printf("╗\n");

    /* Parcourt toutes les lignes du plateau */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        /* Affiche le numéro de la ligne à gauche */
        printf(" %d  ║", row + 1);

        /* Parcourt toutes les colonnes de la ligne */
        for (column = 0; column < BOARD_WIDTH; column++) {
            is_winning_cell = FALSE;

            /*
               Si winning_cells existe et que la case actuelle
               fait partie de l'alignement gagnant, alors on la marque.
            */
            if (winning_cells != NULL && winning_cells[row][column]) {
                is_winning_cell = TRUE;
            }

            if (has_pivot && row == pivot.row && column == pivot.column) {
                /*
                   Si la case actuelle est le pivot,
                   on affiche P en violet.
                */
                printf(PURPLE BOLD " P " RESET);
            } else if (is_winning_cell) {
                /*
                   Si la case fait partie de la victoire,
                   on l'affiche avec un fond vert.
                */
                printf(BG_WIN WHITE BOLD);
                print_cell_content(game->board[row][column]);
                printf(RESET);
            } else {
                /*
                   Sinon, on affiche simplement le contenu normal de la case.
                */
                print_cell_content(game->board[row][column]);
            }
        }

        /* Bord droit du plateau */
        printf("║\n");
    }

    /* Affiche le bord inférieur du plateau */
    printf("    ╚");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("═══");
    }
    printf("╝\n");

    /*
       Affiche le joueur qui doit jouer.
       Exemple : Joueur actuel : 1 (X)
    */
    printf("\n%sJoueur actuel :%s %d (%c)\n",
           BOLD,
           RESET,
           game->current_player,
           get_player_symbol(game->current_player));

    /* Affiche le numéro du tour actuel */
    printf("%sTour :%s %d\n", BOLD, RESET, game->turn_number);

    /*
       Affiche les commandes disponibles :
       - choisir une colonne
       - sauvegarder
       - quitter
    */
    printf("\nCommandes : choisissez une colonne 1-%d | S = sauvegarder | Q = sauvegarder et quitter\n",
           BOARD_WIDTH);
}


/* ================================
   Affichage normal du jeu
   ================================ */

void display_game(const Game *game) {
    Position fake_pivot;

    /*
       On crée un faux pivot, car ici on ne veut pas afficher de pivot.
       Les valeurs -1 indiquent une position impossible dans le plateau.
    */
    fake_pivot.row = -1;
    fake_pivot.column = -1;

    /* Affiche le jeu sans pivot et sans cases gagnantes */
    display_game_advanced(game, FALSE, fake_pivot, NULL);
}


/* ================================
   Affichage du jeu avec pivot
   ================================ */

void display_game_with_pivot(const Game *game, Position pivot) {
    /*
       Affiche le plateau en montrant la position du pivot.
       Le pivot apparaît avec la lettre P.
    */
    display_game_advanced(game, TRUE, pivot, NULL);
}


/* ================================
   Affichage du jeu avec cases gagnantes
   ================================ */

void display_game_with_winning_cells(const Game *game,
                                     int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    Position fake_pivot;

    /*
       On crée un faux pivot, car ici on veut afficher
       les cases gagnantes, pas le pivot.
    */
    fake_pivot.row = -1;
    fake_pivot.column = -1;

    /*
       Affiche le plateau avec les cases de l'alignement gagnant
       mises en évidence.
    */
    display_game_advanced(game, FALSE, fake_pivot, winning_cells);
}

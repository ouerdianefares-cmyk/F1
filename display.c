#include <stdio.h> /* Permet d'utiliser printf et getchar */

#include "display.h"   /* Contient les prototypes des fonctions d'affichage */
#include "constants.h" /* Contient les constantes du jeu : BOARD_WIDTH, EMPTY_CELL, TRUE, FALSE... */


/* Codes de style pour le terminal */

/* Remet le texte normal */
#define RESET "\033[0m" /* Annule les couleurs et les styles */

/* Texte en gras */
#define BOLD "\033[1m" /* Active le texte en gras */

/* Texte plus discret */
#define DIM "\033[2m" /* Rend le texte moins visible */


/* Couleurs du texte */

/* Couleur rouge : utilisée pour le joueur 1 */
#define RED "\033[91m" /* Code couleur rouge clair */

/* Couleur bleue : utilisée pour le joueur 2 */
#define BLUE "\033[96m" /* Code couleur bleu clair */

/* Couleur jaune : utilisée pour le joueur 3 */
#define YELLOW "\033[93m" /* Code couleur jaune */

/* Couleur verte */
#define GREEN "\033[92m" /* Code couleur vert */

/* Couleur violette : utilisée pour afficher le pivot */
#define PURPLE "\033[95m" /* Code couleur violet */

/* Couleur grise : utilisée pour les blocs */
#define GREY "\033[90m" /* Code couleur gris */

/* Couleur blanche */
#define WHITE "\033[97m" /* Code couleur blanc */


/* Couleur de fond */

/* Fond vert : utilisé pour montrer les cases gagnantes */
#define BG_WIN "\033[42m" /* Code couleur pour mettre un fond vert */


/* Nettoyage de l'écran */

void clear_screen(void) { /* Fonction qui nettoie l'écran du terminal */
    /* Efface tout le contenu affiché dans le terminal */
    printf("\033[2J"); /* Supprime l'affichage actuel */

    /* Replace le curseur en haut à gauche du terminal */
    printf("\033[H"); /* Replace le curseur au début de l'écran */
}


/* Pause dans le jeu */

void pause_screen(void) { /* Fonction qui met le jeu en pause */
    /* Affiche un message et attend que le joueur appuie sur ENTREE */
    printf("\nAppuyez sur ENTREE pour continuer..."); /* Message affiché au joueur */
    getchar(); /* Attend que le joueur appuie sur Entrée */
}


/* Symbole des joueurs */

char get_player_symbol(int player) { /* Retourne le symbole du joueur demandé */
    /* Le joueur 1 utilise le symbole X */
    if (player == 1) { /* Si le joueur est le joueur 1 */
        return 'X'; /* On retourne le symbole X */
    }

    /* Le joueur 2 utilise le symbole O */
    if (player == 2) { /* Si le joueur est le joueur 2 */
        return 'O'; /* On retourne le symbole O */
    }

    /* Le joueur 3 utilise le symbole A */
    return 'A'; /* Sinon, on retourne le symbole A pour le joueur 3 */
}


/* Affichage du titre du jeu */

static void print_title(void) { /* Fonction interne qui affiche le titre du jeu */
    /* Affiche le nom du jeu avec une couleur et un encadrement */
    printf(BOLD BLUE "\n"); /* Active le gras et le bleu */
    printf("   ╔══════════════════════════════╗\n"); /* Affiche le haut du cadre */
    printf("   ║          CY-CONNECT          ║\n"); /* Affiche le nom du jeu */
    printf("   ╚══════════════════════════════╝\n"); /* Affiche le bas du cadre */
    printf(RESET); /* Réinitialise le style du texte */
}


/* Affichage d'une case du plateau */

static void print_cell_content(char cell) { /* Affiche le contenu d'une case */
    /*
       Cette fonction affiche le contenu d'une case.
       Selon le caractère contenu dans la case, on affiche
       un symbole différent avec une couleur différente.
    */

    if (cell == EMPTY_CELL) { /* Si la case est vide */
        /* Case vide */
        printf(DIM " · " RESET); /* Affiche un petit point discret */
    } else if (cell == BLOCK_CELL) { /* Si la case contient un bloc */
        /* Case bloquée */
        printf(GREY BOLD " ▓ " RESET); /* Affiche un bloc gris */
    } else if (cell == 'X') { /* Si la case contient le pion du joueur 1 */
        /* Pion du joueur 1 */
        printf(RED BOLD " ✖ " RESET); /* Affiche une croix rouge */
    } else if (cell == 'O') { /* Si la case contient le pion du joueur 2 */
        /* Pion du joueur 2 */
        printf(BLUE BOLD " ◉ " RESET); /* Affiche un rond bleu */
    } else if (cell == 'A') { /* Si la case contient le pion du joueur 3 */
        /* Pion du joueur 3 */
        printf(YELLOW BOLD " ▲ " RESET); /* Affiche un triangle jaune */
    } else { /* Si le contenu de la case est inconnu */
        /* Affichage normal si le caractère est inconnu */
        printf(" %c ", cell); /* Affiche directement le caractère */
    }
}


/* Affichage avancé du jeu */

static void display_game_advanced(const Game *game, /* Pointeur vers la partie à afficher */
                                  int has_pivot, /* Indique si on doit afficher le pivot */
                                  Position pivot, /* Position du pivot à afficher */
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) { /* Tableau des cases gagnantes */
    int row; /* Variable pour parcourir les lignes */
    int column; /* Variable pour parcourir les colonnes */
    int is_winning_cell; /* Indique si la case actuelle est gagnante */

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

    print_title(); /* Affiche le titre du jeu */

    /* Affiche les numéros des colonnes en haut du plateau */
    printf("\n      "); /* Ajoute un espace avant les numéros de colonnes */
    for (column = 0; column < BOARD_WIDTH; column++) { /* Parcourt toutes les colonnes */
        printf(" %d ", column + 1); /* Affiche le numéro de la colonne */
    }

    /* Affiche le bord supérieur du plateau */
    printf("\n    ╔"); /* Affiche le coin supérieur gauche */
    for (column = 0; column < BOARD_WIDTH; column++) { /* Parcourt les colonnes pour dessiner le bord */
        printf("═══"); /* Affiche une partie horizontale du cadre */
    }
    printf("╗\n"); /* Affiche le coin supérieur droit */

    /* Parcourt toutes les lignes du plateau */
    for (row = 0; row < BOARD_HEIGHT; row++) { /* Boucle sur chaque ligne */
        /* Affiche le numéro de la ligne à gauche */
        printf(" %d  ║", row + 1); /* Affiche le numéro de ligne et le bord gauche */

        /* Parcourt toutes les colonnes de la ligne */
        for (column = 0; column < BOARD_WIDTH; column++) { /* Boucle sur chaque colonne */
            is_winning_cell = FALSE; /* Au départ, la case n'est pas gagnante */

            /*
               Si winning_cells existe et que la case actuelle
               fait partie de l'alignement gagnant, alors on la marque.
            */
            if (winning_cells != NULL && winning_cells[row][column]) { /* Vérifie si la case fait partie d'une victoire */
                is_winning_cell = TRUE; /* Marque la case comme gagnante */
            }

            if (has_pivot && row == pivot.row && column == pivot.column) { /* Vérifie si la case actuelle est le pivot */
                /*
                   Si la case actuelle est le pivot,
                   on affiche P en violet.
                */
                printf(PURPLE BOLD " P " RESET); /* Affiche le pivot avec la lettre P */
            } else if (is_winning_cell) { /* Sinon, vérifie si la case est gagnante */
                /*
                   Si la case fait partie de la victoire,
                   on l'affiche avec un fond vert.
                */
                printf(BG_WIN WHITE BOLD); /* Active le fond vert, le blanc et le gras */
                print_cell_content(game->board[row][column]); /* Affiche le contenu de la case */
                printf(RESET); /* Réinitialise le style */
            } else { /* Sinon, la case est normale */
                /*
                   Sinon, on affiche simplement le contenu normal de la case.
                */
                print_cell_content(game->board[row][column]); /* Affiche la case normalement */
            }
        }

        /* Bord droit du plateau */
        printf("║\n"); /* Affiche le bord droit de la ligne */
    }

    /* Affiche le bord inférieur du plateau */
    printf("    ╚"); /* Affiche le coin inférieur gauche */
    for (column = 0; column < BOARD_WIDTH; column++) { /* Parcourt les colonnes pour dessiner le bas */
        printf("═══"); /* Affiche une partie horizontale du cadre */
    }
    printf("╝\n"); /* Affiche le coin inférieur droit */

    /*
       Affiche le joueur qui doit jouer.
       Exemple : Joueur actuel : 1 (X)
    */
    printf("\n%sJoueur actuel :%s %d (%c)\n", /* Affiche le joueur actuel */
           BOLD, /* Active le gras pour le texte */
           RESET, /* Désactive le gras après le texte */
           game->current_player, /* Numéro du joueur actuel */
           get_player_symbol(game->current_player)); /* Symbole du joueur actuel */

    /* Affiche le numéro du tour actuel */
    printf("%sTour :%s %d\n", BOLD, RESET, game->turn_number); /* Affiche le tour actuel */

    /*
       Affiche les commandes disponibles :
       - choisir une colonne
       - sauvegarder
       - quitter
    */
    printf("\nCommandes : choisissez une colonne 1-%d | S = sauvegarder | Q = sauvegarder et quitter\n",
           BOARD_WIDTH); /* Affiche les commandes disponibles */
}


/* Affichage normal du jeu */

void display_game(const Game *game) { /* Affiche le plateau sans pivot et sans cases gagnantes */
    Position fake_pivot; /* Crée une position de pivot fictive */

    /*
       On crée un faux pivot, car ici on ne veut pas afficher de pivot.
       Les valeurs -1 indiquent une position impossible dans le plateau.
    */
    fake_pivot.row = -1; /* Ligne invalide pour ne pas afficher de pivot */
    fake_pivot.column = -1; /* Colonne invalide pour ne pas afficher de pivot */

    /* Affiche le jeu sans pivot et sans cases gagnantes */
    display_game_advanced(game, FALSE, fake_pivot, NULL); /* Appelle l'affichage avancé en mode normal */
}


/* Affichage du jeu avec pivot */

void display_game_with_pivot(const Game *game, Position pivot) { /* Affiche le plateau avec le pivot visible */
    /*
       Affiche le plateau en montrant la position du pivot.
       Le pivot apparaît avec la lettre P.
    */
    display_game_advanced(game, TRUE, pivot, NULL); /* Appelle l'affichage avancé avec le pivot */
}


/* Affichage du jeu avec cases gagnantes */

void display_game_with_winning_cells(const Game *game, /* Partie à afficher */
                                     int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) { /* Cases gagnantes à surligner */
    Position fake_pivot; /* Crée un faux pivot */

    /*
       On crée un faux pivot, car ici on veut afficher
       les cases gagnantes, pas le pivot.
    */
    fake_pivot.row = -1; /* Ligne invalide pour ne pas afficher de pivot */
    fake_pivot.column = -1; /* Colonne invalide pour ne pas afficher de pivot */

    /*
       Affiche le plateau avec les cases de l'alignement gagnant
       mises en évidence.
    */
    display_game_advanced(game, FALSE, fake_pivot, winning_cells); /* Affiche le jeu avec les cases gagnantes */
}

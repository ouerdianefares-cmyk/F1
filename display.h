#ifndef DISPLAY_H
#define DISPLAY_H

/*
    display.h

    Ce fichier contient les prototypes des fonctions d'affichage.

    Un prototype sert à dire au compilateur :
    "Cette fonction existe, elle sera codée dans un autre fichier .c".

    Ici, les fonctions seront codées dans display.c.
*/

#include "types.h"

/*
    Efface l'écran du terminal.
*/
void clear_screen(void);

/*
    Met une pause dans le programme.
    L'utilisateur doit appuyer sur ENTREE pour continuer.
*/
void pause_screen(void);

/*
    Retourne le symbole d'un joueur.

    Exemple :
    joueur 1 -> 'X'
    joueur 2 -> 'O'
    joueur 3 -> 'A'
*/
char get_player_symbol(int player);

/*
    Affiche le plateau de jeu dans le terminal.
*/
void display_game(const Game *game);

#endif

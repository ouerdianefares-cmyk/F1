#ifndef TYPES_H
#define TYPES_H

#include "constants.h"

/*
    Ce fichier contient les structures du projet.

    Une structure permet de regrouper plusieurs variables ensemble.
*/

/*
    Structure Game :

    Elle représente toute une partie.
*/
typedef struct {
    /*
        Plateau de jeu.

        C’est un tableau à deux dimensions :
        - BOARD_HEIGHT lignes
        - BOARD_WIDTH colonnes

        Exemple :
        board[0][0] = première ligne, première colonne
        board[5][7] = dernière ligne, dernière colonne
    */
    char board[BOARD_HEIGHT][BOARD_WIDTH];

    /* Nombre de joueurs dans la partie : 2 ou 3 */
    int player_count;

    /* Numéro du joueur qui doit jouer actuellement */
    int current_player;

    /* Numéro du tour actuel */
    int turn_number;

} Game;

/*
    Structure Position :

    Elle sert à stocker une position dans le plateau.
*/
typedef struct {
    /* Ligne de la position */
    int row;

    /* Colonne de la position */
    int column;

} Position;

#endif

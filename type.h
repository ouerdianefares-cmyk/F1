#ifndef TYPES_H /* Évite que ce fichier soit inclus plusieurs fois */
#define TYPES_H /* Définit TYPES_H pour activer la protection d'inclusion */

#include "constants.h" /* Contient BOARD_HEIGHT et BOARD_WIDTH utilisés pour la taille du plateau */

/*
    Ce fichier contient les structures du projet.

    Une structure permet de regrouper plusieurs variables ensemble.

    Ici, on utilise des structures pour représenter :
    - une partie complète avec Game
    - une position dans le plateau avec Position
*/


/* STRUCTURE GAME */

/*
    Structure Game :

    Elle représente toute une partie.

    Cette structure contient toutes les informations nécessaires
    pour savoir où en est le jeu :
    - le plateau
    - le nombre de joueurs
    - le joueur actuel
    - le numéro du tour
*/
typedef struct {
    /*
        Plateau de jeu.

        C'est un tableau à deux dimensions :
        - BOARD_HEIGHT lignes
        - BOARD_WIDTH colonnes

        Dans ton jeu :
        - BOARD_HEIGHT vaut 6
        - BOARD_WIDTH vaut 8

        Exemple :
        board[0][0] = première ligne, première colonne
        board[5][7] = dernière ligne, dernière colonne

        Chaque case contient un caractère :
        - '.' pour une case vide
        - '#' pour un bloc
        - 'X' pour le joueur 1
        - 'O' pour le joueur 2
        - 'A' pour le joueur 3
    */
    char board[BOARD_HEIGHT][BOARD_WIDTH];

    /*
        Nombre de joueurs dans la partie.

        Il peut valoir :
        - 2 pour une partie à deux joueurs
        - 3 pour une partie à trois joueurs
    */
    int player_count;

    /*
        Numéro du joueur qui doit jouer actuellement.

        Exemple :
        - 1 signifie que c'est au joueur 1 de jouer
        - 2 signifie que c'est au joueur 2 de jouer
        - 3 signifie que c'est au joueur 3 de jouer
    */
    int current_player;

    /*
        Numéro du tour actuel.

        Cette valeur augmente à chaque fois
        qu'on passe au joueur suivant.
    */
    int turn_number;

} Game;


/* STRUCTURE POSITION */

/*
    Structure Position :

    Elle sert à stocker une position dans le plateau.

    Une position est composée de :
    - une ligne
    - une colonne

    Cette structure est utile pour :
    - mémoriser où une pièce est tombée
    - mémoriser où se trouve le pivot de rotation
*/
typedef struct {
    /*
        Ligne de la position.

        Attention :
        dans le code C, les lignes commencent à 0.

        Exemple :
        - row = 0 correspond à la ligne 1 affichée au joueur
        - row = 5 correspond à la ligne 6 affichée au joueur
    */
    int row;

    /*
        Colonne de la position.

        Attention :
        dans le code C, les colonnes commencent à 0.

        Exemple :
        - column = 0 correspond à la colonne 1 affichée au joueur
        - column = 7 correspond à la colonne 8 affichée au joueur
    */
    int column;

} Position;


#endif /* TYPES_H */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* ================================
   Dimensions du plateau de jeu
   ================================ */

/* Nombre de colonnes du plateau */
#define BOARD_WIDTH 8

/* Nombre de lignes du plateau */
#define BOARD_HEIGHT 6


/* ================================
   Nombre de joueurs
   ================================ */

/* Nombre minimum de joueurs autorisés */
#define MIN_PLAYERS 2

/* Nombre maximum de joueurs autorisés */
#define MAX_PLAYERS 3


/* ================================
   Condition de victoire
   ================================ */

/* Nombre de pions identiques à aligner pour gagner */
#define ALIGNMENT_LENGTH 5


/* ================================
   Symboles utilisés dans la grille
   ================================ */

/* Case vide du plateau */
#define EMPTY_CELL '.'

/* Case bloquée du plateau */
#define BLOCK_CELL '#'


/* ================================
   Sauvegarde
   ================================ */

/* Nom du fichier utilisé pour sauvegarder la partie */
#define SAVE_FILE "save.txt"


/* ================================
   Sens de rotation
   ================================ */

/* Rotation vers la gauche */
#define ROTATE_LEFT -1

/* Rotation vers la droite */
#define ROTATE_RIGHT 1


/* ================================
   Valeurs booléennes
   ================================ */

/* Représente vrai */
#define TRUE 1

/* Représente faux */
#define FALSE 0


/* ================================
   Saisie utilisateur
   ================================ */

/* Taille maximale d'une entrée tapée par l'utilisateur */
#define INPUT_SIZE 128


/* ================================
   Actions possibles pendant une saisie
   ================================ */

/* Aucune action spéciale */
#define ACTION_NONE 0

/* Le joueur veut sauvegarder la partie */
#define ACTION_SAVE 1

/* Le joueur veut quitter la partie */
#define ACTION_QUIT 2


#endif /* CONSTANTS_H */

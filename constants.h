#define CONSTANTS_H

/*
    Ce fichier contient toutes les constantes du projet.

    Une constante est une valeur fixe qui ne change pas pendant le programme.
    On utilise #define pour éviter d'écrire des nombres directement dans le code.
*/

/* Largeur du plateau : 8 colonnes */
#define BOARD_WIDTH 8

/* Hauteur du plateau : 6 lignes */
#define BOARD_HEIGHT 6

/* Nombre minimum et maximum de joueurs */
#define MIN_PLAYERS 2
#define MAX_PLAYERS 3

/* Nombre de pièces à aligner pour gagner */
#define ALIGNMENT_LENGTH 5

/* Symbole pour une case vide */
#define EMPTY_CELL '.'

/* Symbole pour une case bloquée / indestructible */
#define BLOCK_CELL '#'

/* Nom du fichier utilisé pour sauvegarder une partie */
#define SAVE_FILE "save.txt"

/* Valeurs utilisées pour indiquer le sens de rotation */
#define ROTATE_LEFT -1
#define ROTATE_RIGHT 1

/* En C, il n’existe pas forcément TRUE/FALSE par défaut */
#define TRUE 1
#define FALSE 0

/* Taille maximale d’une saisie clavier */
#define INPUT_SIZE 128

#endif

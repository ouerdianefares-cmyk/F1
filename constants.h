#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
    Fichier constants.h

    Ce fichier regroupe toutes les constantes du jeu.
    Une constante est une valeur fixe utilisée dans plusieurs fichiers.

    Exemple :
    au lieu d'écrire directement 8 partout dans le code,
    on écrit BOARD_WIDTH.
    C'est plus clair et plus facile à modifier.
*/


/* TAILLE DU PLATEAU */

/* Largeur du plateau : 8 colonnes */
#define BOARD_WIDTH 8

/* Hauteur du plateau : 6 lignes */
#define BOARD_HEIGHT 6


/* JOUEURS */

/* Nombre minimum de joueurs pour lancer une partie */
#define MIN_PLAYERS 2

/* Nombre maximum de joueurs autorisés */
#define MAX_PLAYERS 3


/* VICTOIRE */

/*
    Nombre de pions à aligner pour gagner.

    Le joueur gagne s'il aligne 5 pions :
    - horizontalement
    - verticalement
    - en diagonale
*/
#define ALIGNMENT_LENGTH 5


/* CONTENU DES CASES */

/* Symbole utilisé pour représenter une case vide */
#define EMPTY_CELL '.'

/*
    Symbole utilisé pour représenter une case bloquée.

    Une case bloquée ne peut pas recevoir de pion.
    Elle sert d'obstacle dans le plateau.
*/
#define BLOCK_CELL '#'


/* SAUVEGARDE */

/* Nom du fichier dans lequel la partie est sauvegardée */
#define SAVE_FILE "save.txt"


/* ROTATION */

/*
    Valeur utilisée pour une rotation vers la gauche.

    Le nombre -1 permet de différencier clairement
    la gauche de la droite dans le code.
*/
#define ROTATE_LEFT -1

/*
    Valeur utilisée pour une rotation vers la droite.

    Le nombre 1 représente le sens opposé à ROTATE_LEFT.
*/
#define ROTATE_RIGHT 1


/* BOOLÉENS */

/*
    En langage C, il n'y a pas toujours de type booléen simple
    selon la façon dont on code.

    On utilise donc :
    - TRUE pour dire vrai
    - FALSE pour dire faux
*/
#define TRUE 1

#define FALSE 0


/* SAISIE UTILISATEUR */

/*
    Taille maximale du texte que l'utilisateur peut entrer.

    Exemple :
    quand le joueur tape une colonne, S ou Q,
    le programme stocke la saisie dans un tableau de taille INPUT_SIZE.
*/
#define INPUT_SIZE 128


/* ACTIONS POSSIBLES PENDANT UNE SAISIE */

/* Le joueur n'a pas demandé d'action spéciale */
#define ACTION_NONE 0

/* Le joueur a tapé S pour sauvegarder */
#define ACTION_SAVE 1

/* Le joueur a tapé Q pour sauvegarder et quitter */
#define ACTION_QUIT 2


#endif /* CONSTANTS_H */

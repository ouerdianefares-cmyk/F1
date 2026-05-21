#ifndef GAME_H /* Empêche d'inclure plusieurs fois ce fichier */
#define GAME_H /* Définit GAME_H pour activer la protection d'inclusion */

/*
    game.h

    Ce fichier contient les prototypes des fonctions principales du jeu.

    Un prototype permet d'annoncer qu'une fonction existe,
    même si son code complet se trouve dans game.c.

    Les règles du jeu sont codées dans game.c.

    main.c utilise ces fonctions pour :
    - initialiser une partie
    - jouer les tours
    - changer de joueur
    - vérifier si le plateau est plein
    - vérifier s'il y a un gagnant
*/

#include "type.h" /* Contient les structures Game et Position */


/* INITIALISATION DU JEU */

/*
    Initialise une nouvelle partie.

    Cette fonction prépare la structure Game au début du jeu.

    Paramètres :
    - game : adresse de la partie à initialiser
    - player_count : nombre de joueurs, 2 ou 3

    Elle fixe notamment :
    - le nombre de joueurs
    - le joueur qui commence
    - le numéro du premier tour
    - le plateau de départ
*/
void initialize_game(Game *game, int player_count);


/* TOUR DE JEU */

/*
    Joue un tour complet pour le joueur actuel.

    Pendant un tour :
    - le jeu impose une taille de rotation : 3x3 ou 5x5
    - le joueur choisit une colonne
    - sa pièce tombe dans cette colonne
    - le joueur choisit un pivot
    - le joueur choisit un sens de rotation
    - la zone choisie tourne
    - la gravité est appliquée sur le plateau

    Retourne :
    - TRUE si le tour s'est bien déroulé
    - FALSE si le joueur a sauvegardé et quitté
*/
int play_turn(Game *game);


/* CHANGEMENT DE JOUEUR */

/*
    Passe au joueur suivant.

    Exemple avec 2 joueurs :
    - joueur 1 devient joueur 2
    - joueur 2 revient au joueur 1

    Exemple avec 3 joueurs :
    - joueur 1 devient joueur 2
    - joueur 2 devient joueur 3
    - joueur 3 revient au joueur 1
*/
void change_player(Game *game);


/* PLATEAU PLEIN */

/*
    Vérifie si le plateau est plein.

    Le plateau est considéré plein si aucune colonne
    ne peut recevoir une nouvelle pièce.

    Retourne :
    - TRUE si plus aucune colonne n'est jouable
    - FALSE s'il reste au moins une colonne jouable
*/
int is_board_full(const Game *game);


/* RECHERCHE DES GAGNANTS */

/*
    Vérifie s'il y a un ou plusieurs gagnants.

    Comme une rotation peut modifier plusieurs alignements,
    il est possible d'avoir plusieurs gagnants en même temps.

    Paramètres :
    - game : partie actuelle
    - winners : tableau dans lequel on stocke les numéros des joueurs gagnants

    Retourne :
    - 0 s'il n'y a aucun gagnant
    - 1 s'il y a un seul gagnant
    - 2 ou 3 s'il y a plusieurs gagnants
*/
int check_winners(const Game *game, int winners[]);


/* CASES GAGNANTES */

/*
    Remplit le tableau winning_cells avec les cases gagnantes.

    Ce tableau sert surtout à l'affichage :
    les cases qui appartiennent à un alignement gagnant
    pourront être surlignées dans display.c.

    Exemple :
    si winning_cells[2][4] vaut TRUE,
    alors la case correspondante sera affichée comme gagnante.

    Paramètres :
    - game : partie actuelle
    - winning_cells : tableau qui indique les cases gagnantes

    Retourne :
    - TRUE si au moins un alignement gagnant est trouvé
    - FALSE sinon
*/
int get_winning_cells(const Game *game,
                      int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);


#endif /* GAME_H */

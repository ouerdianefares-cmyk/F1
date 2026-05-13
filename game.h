#ifndef GAME_H
#define GAME_H

/*
    game.h

    Ce fichier contient les prototypes des fonctions principales du jeu.

    Les règles du jeu sont codées dans game.c.

    main.c utilise ces fonctions pour lancer une partie,
    jouer les tours, changer de joueur et vérifier les gagnants.
*/

#include "type.h"

/*
    Initialise une nouvelle partie.

    Paramètres :
    - game : la partie à initialiser
    - player_count : nombre de joueurs, 2 ou 3
*/
void initialize_game(Game *game, int player_count);

/*
    Joue un tour complet.

    Pendant un tour :
    - le joueur choisit une colonne
    - sa pièce tombe
    - le joueur choisit un pivot
    - la zone tourne
    - la gravité est appliquée

    Retourne TRUE si le tour s'est bien déroulé.
*/
int play_turn(Game *game);

/*
    Passe au joueur suivant.

    Exemple :
    joueur 1 -> joueur 2
    joueur 2 -> joueur 3 si la partie a 3 joueurs
    sinon joueur 2 -> joueur 1
*/
void change_player(Game *game);

/*
    Vérifie si le plateau est plein.

    Retourne :
    - TRUE si plus aucune colonne n'est jouable
    - FALSE s'il reste au moins une colonne jouable
*/
int is_board_full(const Game *game);

/*
    Vérifie s'il y a un ou plusieurs gagnants.

    Paramètres :
    - game : la partie actuelle
    - winners : tableau dans lequel on stocke les joueurs gagnants

    Retourne :
    - 0 s'il n'y a aucun gagnant
    - 1 s'il y a un seul gagnant
    - 2 ou 3 s'il y a plusieurs gagnants en même temps
*/
int check_winners(const Game *game, int winners[]);

#endif

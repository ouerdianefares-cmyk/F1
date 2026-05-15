#ifndef DISPLAY_H
#define DISPLAY_H

/*
   On inclut type.h car ce fichier utilise les types :
   - Game
   - Position

   Ces types doivent être connus pour pouvoir écrire
   les prototypes des fonctions d'affichage.
*/
#include "type.h"


/* ================================
   Fonctions générales d'affichage
   ================================ */

/*
   Efface l'écran du terminal.

   Utile pour rafraîchir l'affichage du jeu
   à chaque nouveau tour.
*/
void clear_screen(void);

/*
   Met le programme en pause.

   Le joueur doit appuyer sur ENTREE
   pour continuer.
*/
void pause_screen(void);

/*
   Retourne le symbole du joueur.

   Exemple :
   - joueur 1 : X
   - joueur 2 : O
   - joueur 3 : A
*/
char get_player_symbol(int player);


/* ================================
   Affichage du plateau
   ================================ */

/*
   Affiche le plateau de jeu normalement.

   Cette fonction affiche :
   - la grille
   - les pions des joueurs
   - le joueur actuel
   - le numéro du tour
   - les commandes disponibles
*/
void display_game(const Game *game);


/*
   Affiche le plateau avec un pivot marqué par P.

   Le pivot est le centre du carré qui va tourner.
   Cette fonction est utile pour montrer au joueur
   quelle case il a choisie comme centre de rotation.
*/
void display_game_with_pivot(const Game *game, Position pivot);


/*
   Affiche le plateau avec les cases gagnantes surlignées.

   winning_cells est un tableau qui indique quelles cases
   font partie de l'alignement gagnant.

   Exemple :
   si winning_cells[2][4] vaut TRUE,
   alors la case ligne 3 colonne 5 sera surlignée.
*/
void display_game_with_winning_cells(const Game *game,
                                     int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);


#endif /* DISPLAY_H */

#ifndef SAVE_H
#define SAVE_H

/*
    save.h

    Ce fichier contient les prototypes des fonctions
    qui servent à sauvegarder et charger une partie.

    Les fonctions seront codées dans save.c.
*/

#include "type.h"

/*
    Sauvegarde la partie actuelle dans un fichier.

    Paramètre :
    - game : la partie à sauvegarder

    Retourne :
    - TRUE si la sauvegarde a réussi
    - FALSE si la sauvegarde a échoué
*/
int save_game(const Game *game);

/*
    Charge une partie depuis le fichier de sauvegarde.

    Paramètre :
    - game : la variable dans laquelle on va mettre la partie chargée

    Retourne :
    - TRUE si le chargement a réussi
    - FALSE si aucun fichier n'existe ou si la sauvegarde est invalide
*/
int load_game(Game *game);

#endif

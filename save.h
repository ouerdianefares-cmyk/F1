#ifndef SAVE_H /* Évite que ce fichier soit inclus plusieurs fois */
#define SAVE_H /* Définit SAVE_H pour activer la protection d'inclusion */

/*
    save.h

    Ce fichier contient les prototypes des fonctions
    qui servent à sauvegarder et charger une partie.

    Un prototype annonce qu'une fonction existe,
    même si son code complet se trouve dans save.c.

    Les fonctions seront codées dans save.c.
*/

#include "type.h" /* Contient la structure Game utilisée par les fonctions */


/* SAUVEGARDE DE LA PARTIE */

/*
    Sauvegarde la partie actuelle dans un fichier.

    Cette fonction permet au joueur d'arrêter la partie
    et de la reprendre plus tard.

    Paramètre :
    - game : adresse de la partie à sauvegarder

    La sauvegarde contient notamment :
    - le nombre de joueurs
    - le joueur actuel
    - le numéro du tour
    - le contenu du plateau

    Retourne :
    - TRUE si la sauvegarde a réussi
    - FALSE si la sauvegarde a échoué
*/
int save_game(const Game *game);


/* CHARGEMENT DE LA PARTIE */

/*
    Charge une partie depuis le fichier de sauvegarde.

    Cette fonction lit le fichier de sauvegarde
    puis reconstruit la partie dans la variable game.

    Paramètre :
    - game : adresse de la variable dans laquelle on va mettre la partie chargée

    La fonction vérifie aussi que la sauvegarde est valide,
    pour éviter de charger un fichier incorrect ou modifié.

    Retourne :
    - TRUE si le chargement a réussi
    - FALSE si aucun fichier n'existe ou si la sauvegarde est invalide
*/
int load_game(Game *game);


#endif /* SAVE_H */

#include <stdio.h>      /* Permet d'utiliser FILE, fopen, fprintf, fgetc, fputc et fclose */
#include "save.h"       /* Contient les prototypes de save_game et load_game */
#include "constants.h"  /* Contient SAVE_FILE, TRUE, FALSE, tailles du plateau et symboles */


/* SAUVEGARDE DE LA PARTIE */

/*
    Cette fonction sauvegarde l'état actuel de la partie dans un fichier texte.

    L'objectif est de pouvoir arrêter une partie puis la reprendre plus tard.

    Le fichier de sauvegarde contient :
    - le nombre de joueurs
    - le joueur qui doit jouer
    - le numéro du tour
    - le contenu complet du plateau
*/
int save_game(const Game *game)
{
    FILE *file; /* Pointeur vers le fichier de sauvegarde */
    int row; /* Sert à parcourir les lignes du plateau */
    int column; /* Sert à parcourir les colonnes du plateau */

    /*
        On ouvre le fichier en mode écriture "w".

        Si le fichier n'existe pas, il est créé.
        S'il existe déjà, son ancien contenu est remplacé.

        SAVE_FILE correspond au nom du fichier défini dans constants.h.
    */
    file = fopen(SAVE_FILE, "w");

    if (file == NULL) /* Vérifie si l'ouverture du fichier a échoué */
    {
        printf("Erreur : impossible de sauvegarder la partie.\n");
        return FALSE; /* La sauvegarde n'a pas fonctionné */
    }

    /*
        Première ligne du fichier :
        - nombre de joueurs
        - joueur actuel
        - numéro du tour

        Ces informations permettent de reprendre la partie
        exactement au bon moment.
    */
    fprintf(file, "%d %d %d\n",
            game->player_count,
            game->current_player,
            game->turn_number);

    /*
        Ensuite, on sauvegarde le plateau ligne par ligne.

        Chaque case est écrite sous forme d'un caractère :
        - '.' pour une case vide
        - '#' pour un bloc
        - 'X', 'O' ou 'A' pour les joueurs
    */
    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            fputc(game->board[row][column], file); /* Écrit une case dans le fichier */
        }

        /*
            Après chaque ligne du plateau,
            on écrit un retour à la ligne pour garder un fichier lisible.
        */
        fputc('\n', file);
    }

    /*
        On ferme le fichier après l'écriture.

        C'est important, car cela garantit que les données
        sont bien enregistrées sur le disque.
    */
    fclose(file);

    printf("Partie sauvegardee dans %s.\n", SAVE_FILE);

    return TRUE; /* La sauvegarde a réussi */
}


/* CHARGEMENT DE LA PARTIE */

/*
    Cette fonction charge une partie depuis le fichier de sauvegarde.

    Elle reconstruit la structure Game à partir du contenu du fichier.

    Elle vérifie aussi que la sauvegarde est correcte
    pour éviter de charger une partie invalide ou corrompue.
*/
int load_game(Game *game)
{
    FILE *file; /* Pointeur vers le fichier de sauvegarde */
    int row; /* Sert à parcourir les lignes du plateau */
    int column; /* Sert à parcourir les colonnes du plateau */
    char character; /* Stocke chaque caractère lu dans le fichier */

    /*
        On ouvre le fichier en mode lecture "r".

        Si le fichier n'existe pas,
        cela veut dire qu'aucune partie n'a encore été sauvegardée.
    */
    file = fopen(SAVE_FILE, "r");

    if (file == NULL) /* Vérifie si le fichier n'a pas pu être ouvert */
    {
        printf("Aucune sauvegarde trouvee.\n");
        return FALSE; /* Le chargement échoue */
    }

    /*
        On lit la première ligne du fichier.

        Elle doit contenir exactement trois valeurs :
        - le nombre de joueurs
        - le joueur actuel
        - le numéro du tour

        fscanf retourne le nombre d'éléments lus correctement.
        Ici, il doit retourner 3.
    */
    if (fscanf(file, "%d %d %d\n",
               &game->player_count,
               &game->current_player,
               &game->turn_number) != 3)
    {
        fclose(file); /* Ferme le fichier avant de quitter */
        printf("Sauvegarde invalide.\n");
        return FALSE; /* La sauvegarde ne respecte pas le format attendu */
    }

    /*
        Vérifie que le nombre de joueurs lu dans le fichier est valide.

        Cela évite par exemple de charger une sauvegarde
        avec 0 joueur ou 10 joueurs.
    */
    if (game->player_count < MIN_PLAYERS || game->player_count > MAX_PLAYERS)
    {
        fclose(file); /* Ferme le fichier avant de quitter */
        printf("Sauvegarde invalide : nombre de joueurs incorrect.\n");
        return FALSE;
    }

    /*
        Vérifie que le joueur actuel existe vraiment.

        Exemple :
        si la partie contient 2 joueurs,
        le joueur actuel doit être 1 ou 2.
    */
    if (game->current_player < 1 || game->current_player > game->player_count)
    {
        fclose(file); /* Ferme le fichier avant de quitter */
        printf("Sauvegarde invalide : joueur actuel incorrect.\n");
        return FALSE;
    }

    /*
        On lit maintenant le plateau case par case.

        Le fichier doit contenir BOARD_HEIGHT lignes,
        chacune avec BOARD_WIDTH caractères.
    */
    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes du plateau */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            character = fgetc(file); /* Lit un caractère du fichier */

            /*
                On vérifie que chaque caractère lu correspond
                à un contenu autorisé du plateau.

                Cela protège le programme contre une sauvegarde modifiée
                ou abîmée.
            */
            if (character != EMPTY_CELL &&
                character != BLOCK_CELL &&
                character != 'X' &&
                character != 'O' &&
                character != 'A')
            {
                fclose(file); /* Ferme le fichier avant de quitter */
                printf("Sauvegarde invalide : contenu du plateau incorrect.\n");
                return FALSE;
            }

            game->board[row][column] = character; /* Copie la case lue dans le plateau */
        }

        /*
            Après chaque ligne du plateau,
            le fichier contient un retour à la ligne.

            On le lit simplement pour passer à la ligne suivante.
        */
        fgetc(file);
    }

    fclose(file); /* Ferme le fichier après lecture complète */

    printf("Partie restauree avec succes.\n");

    return TRUE; /* Le chargement a réussi */
}

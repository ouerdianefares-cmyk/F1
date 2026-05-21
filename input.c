#include <stdio.h>   /* Permet d'utiliser printf, fgets et stdin */
#include <string.h>  /* Permet d'utiliser strcmp pour comparer du texte */

#include "input.h"      /* Contient les prototypes des fonctions de saisie */
#include "constants.h"  /* Contient INPUT_SIZE, TRUE, ACTION_SAVE, ACTION_QUIT... */


/* SUPPRESSION DU RETOUR À LA LIGNE */

/*
   Quand l'utilisateur écrit quelque chose puis appuie sur Entrée,
   fgets récupère aussi le caractère '\n'.

   Exemple :
   si le joueur tape S puis Entrée,
   la chaîne récupérée est "S\n".

   Pour comparer correctement le texte avec "S" ou "Q",
   on doit supprimer ce retour à la ligne.
*/
static void remove_newline(char text[])
{
    int i; /* Sert à parcourir chaque caractère de la chaîne */

    for (i = 0; text[i] != '\0'; i++) /* Parcourt la chaîne jusqu'à la fin */
    {
        if (text[i] == '\n') /* Vérifie si le caractère actuel est un retour à la ligne */
        {
            text[i] = '\0'; /* Remplace '\n' par la fin de chaîne */
            return; /* Arrête la fonction dès que le retour à la ligne est supprimé */
        }
    }
}


/* VÉRIFICATION DE LA COMMANDE SAUVEGARDER */

/*
   Cette fonction vérifie si le joueur veut sauvegarder.

   Le joueur peut taper :
   - S en majuscule
   - s en minuscule

   strcmp compare deux chaînes de caractères.
   Si les deux textes sont identiques, strcmp retourne 0.
*/
static int is_save_command(const char text[])
{
    return strcmp(text, "s") == 0 || strcmp(text, "S") == 0;
}


/* VÉRIFICATION DE LA COMMANDE QUITTER */

/*
   Cette fonction vérifie si le joueur veut quitter.

   Dans ce jeu, Q signifie :
   sauvegarder la partie puis revenir au menu.

   Le joueur peut taper :
   - Q en majuscule
   - q en minuscule
*/
static int is_quit_command(const char text[])
{
    return strcmp(text, "q") == 0 || strcmp(text, "Q") == 0;
}


/* LECTURE D'UN NOMBRE SIMPLE */

/*
   Cette fonction demande un nombre entier à l'utilisateur.

   Elle vérifie trois choses :
   1. la saisie a bien été lue
   2. la saisie est bien un nombre
   3. le nombre est compris entre minimum et maximum

   Tant que la saisie n'est pas correcte,
   la fonction redemande une valeur.
*/
int read_integer(const char *message, int minimum, int maximum)
{
    char input[INPUT_SIZE]; /* Tableau qui stocke ce que l'utilisateur tape */
    int value; /* Nombre obtenu après conversion de la saisie */
    int result; /* Résultat de sscanf : indique si la conversion a réussi */

    while (TRUE) /* Boucle infinie contrôlée par les return et continue */
    {
        printf("%s", message); /* Affiche le message de demande */

        /*
           fgets lit une ligne complète tapée au clavier.

           INPUT_SIZE limite la taille de la saisie,
           ce qui évite de dépasser la taille du tableau input.
        */
        if (fgets(input, INPUT_SIZE, stdin) == NULL)
        {
            printf("Erreur de lecture. Reessayez.\n");
            continue; /* Recommence la saisie */
        }

        /*
           sscanf essaie de transformer le texte en entier.

           Exemple :
           - "4" devient le nombre 4
           - "abc" ne peut pas être transformé en entier

           Si result vaut 1, la conversion a fonctionné.
        */
        result = sscanf(input, "%d", &value);

        if (result != 1) /* Si l'utilisateur n'a pas entré un nombre valide */
        {
            printf("Veuillez entrer un nombre valide.\n");
            continue; /* Redemande une valeur */
        }

        if (value < minimum || value > maximum) /* Vérifie les limites autorisées */
        {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue; /* Redemande une valeur */
        }

        return value; /* La valeur est correcte, on la retourne */
    }
}


/* LECTURE D'UN NOMBRE OU D'UNE SAUVEGARDE */

/*
   Cette fonction permet de lire :
   - un nombre normal
   - ou la commande S pour sauvegarder

   Elle repose sur read_integer_or_action,
   qui sait déjà gérer S et Q.

   Ici, on récupère surtout l'information :
   est-ce que le joueur a demandé une sauvegarde ?
*/
int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested)
{
    int action; /* Stocke l'action détectée : aucune, sauvegarde ou quitter */
    int value; /* Stocke le nombre lu si l'utilisateur entre un nombre */

    value = read_integer_or_action(message, minimum, maximum, &action); /* Lit un nombre ou une action */

    if (action == ACTION_SAVE) /* Si le joueur a tapé S */
    {
        *save_requested = TRUE; /* Indique au programme qu'une sauvegarde est demandée */
    }
    else
    {
        *save_requested = FALSE; /* Sinon, aucune sauvegarde n'a été demandée */
    }

    return value; /* Retourne le nombre lu, ou 0 si le joueur a tapé S ou Q */
}


/* LECTURE D'UN NOMBRE OU D'UNE ACTION */

/*
   Cette fonction est la plus complète pour gérer les saisies.

   Le joueur peut entrer :
   - un nombre entre minimum et maximum
   - S pour sauvegarder
   - Q pour sauvegarder et quitter

   Le paramètre action permet de prévenir le reste du programme
   de ce que le joueur a choisi.

   Valeurs possibles de action :
   - ACTION_NONE : le joueur a entré un nombre
   - ACTION_SAVE : le joueur a tapé S
   - ACTION_QUIT : le joueur a tapé Q
*/
int read_integer_or_action(const char *message,
                           int minimum,
                           int maximum,
                           int *action)
{
    char input[INPUT_SIZE]; /* Stocke la saisie complète de l'utilisateur */
    int value; /* Stocke le nombre après conversion */
    int result; /* Indique si sscanf a réussi à lire un entier */

    while (TRUE) /* Répète la saisie jusqu'à obtenir une entrée correcte */
    {
        *action = ACTION_NONE; /* Par défaut, aucune action spéciale n'est demandée */

        printf("\n%s\n", message); /* Affiche le message principal */
        printf("Entrez une valeur entre %d et %d.\n", minimum, maximum); /* Affiche les limites */
        printf("S = sauvegarder | Q = sauvegarder et quitter\n> "); /* Affiche les commandes possibles */

        /*
           Lecture de la saisie utilisateur.

           On utilise fgets plutôt que scanf,
           car fgets permet de lire toute la ligne et évite certains bugs de saisie.
        */
        if (fgets(input, INPUT_SIZE, stdin) == NULL)
        {
            printf("Erreur de lecture. Reessayez.\n");
            continue; /* Redemande une saisie */
        }

        remove_newline(input); /* Supprime le retour à la ligne ajouté par fgets */

        if (is_save_command(input)) /* Vérifie si le joueur a tapé S ou s */
        {
            *action = ACTION_SAVE; /* Signale une demande de sauvegarde */
            return 0; /* Pas de nombre à retourner dans ce cas */
        }

        if (is_quit_command(input)) /* Vérifie si le joueur a tapé Q ou q */
        {
            *action = ACTION_QUIT; /* Signale une demande de sauvegarde puis quitter */
            return 0; /* Pas de nombre à retourner dans ce cas */
        }

        /*
           Si la saisie n'est ni S ni Q,
           on essaie de la convertir en nombre entier.
        */
        result = sscanf(input, "%d", &value);

        if (result != 1) /* Si la conversion échoue */
        {
            printf("Veuillez entrer un nombre valide.\n");
            continue; /* Redemande une saisie */
        }

        if (value < minimum || value > maximum) /* Vérifie que le nombre est dans l'intervalle autorisé */
        {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue; /* Redemande une saisie */
        }

        return value; /* La saisie est correcte, on retourne le nombre */
    }
}

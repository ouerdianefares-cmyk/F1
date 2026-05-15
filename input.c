#include <stdio.h>
#include <string.h>

#include "input.h"
#include "constants.h"


/* ============================================================
   SUPPRESSION DU RETOUR À LA LIGNE
   ============================================================ */

/*
   Quand on utilise fgets(), le texte récupéré contient souvent
   le caractère '\n' à la fin.

   Exemple :
   si l'utilisateur tape S puis ENTREE,
   fgets récupère : "S\n"

   Cette fonction transforme "S\n" en "S".
*/
static void remove_newline(char text[]) {
    int i;

    /* On parcourt le texte caractère par caractère */
    for (i = 0; text[i] != '\0'; i++) {
        /* Si on trouve le retour à la ligne */
        if (text[i] == '\n') {
            /* On le remplace par la fin de chaîne */
            text[i] = '\0';
            return;
        }
    }
}


/* ============================================================
   VÉRIFICATION DE LA COMMANDE SAUVEGARDER
   ============================================================ */

/*
   Vérifie si le joueur a tapé S ou s.

   Si oui, cela veut dire :
   le joueur veut sauvegarder la partie.
*/
static int is_save_command(const char text[]) {
    return strcmp(text, "s") == 0 || strcmp(text, "S") == 0;
}


/* ============================================================
   VÉRIFICATION DE LA COMMANDE QUITTER
   ============================================================ */

/*
   Vérifie si le joueur a tapé Q ou q.

   Si oui, cela veut dire :
   le joueur veut sauvegarder puis quitter.
*/
static int is_quit_command(const char text[]) {
    return strcmp(text, "q") == 0 || strcmp(text, "Q") == 0;
}


/* ============================================================
   LECTURE D'UN NOMBRE SIMPLE
   ============================================================ */

/*
   Demande un nombre au joueur.

   Cette fonction oblige le joueur à entrer :
   - un nombre valide
   - compris entre minimum et maximum

   Exemple :
   read_integer("Choisissez une colonne : ", 1, 8)

   Le joueur devra entrer un nombre entre 1 et 8.
*/
int read_integer(const char *message, int minimum, int maximum) {
    char input[INPUT_SIZE];
    int value;
    int result;

    /*
       Boucle infinie :
       on continue tant que l'utilisateur
       ne donne pas une valeur correcte.
    */
    while (TRUE) {
        /* Affiche le message donné en paramètre */
        printf("%s", message);

        /*
           Lit ce que l'utilisateur tape au clavier.
           stdin signifie : entrée clavier.
        */
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
        }

        /*
           sscanf essaie de transformer le texte en nombre entier.

           Exemple :
           input = "4"
           value devient 4

           result vaut 1 si la conversion a réussi.
        */
        result = sscanf(input, "%d", &value);

        /* Si l'utilisateur n'a pas entré un nombre */
        if (result != 1) {
            printf("Veuillez entrer un nombre valide.\n");
            continue;
        }

        /* Si le nombre est trop petit ou trop grand */
        if (value < minimum || value > maximum) {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue;
        }

        /* Si tout est bon, on retourne la valeur */
        return value;
    }
}


/* ============================================================
   LECTURE D'UN NOMBRE OU SAUVEGARDE
   ============================================================ */

/*
   Cette fonction permet de lire :
   - soit un nombre
   - soit la commande S pour sauvegarder

   Elle utilise read_integer_or_action(),
   mais elle ne regarde vraiment que l'action ACTION_SAVE.

   save_requested devient :
   - TRUE si le joueur a demandé une sauvegarde
   - FALSE sinon
*/
int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested) {
    int action;
    int value;

    /*
       On appelle la fonction plus complète,
       capable de détecter S et Q.
    */
    value = read_integer_or_action(message, minimum, maximum, &action);

    /*
       Si l'action détectée est sauvegarder,
       on met save_requested à TRUE.
    */
    if (action == ACTION_SAVE) {
        *save_requested = TRUE;
    } else {
        *save_requested = FALSE;
    }

    /*
       Si le joueur a tapé S, value vaut 0.
       Sinon, value contient le nombre entré.
    */
    return value;
}


/* ============================================================
   LECTURE D'UN NOMBRE OU D'UNE ACTION
   ============================================================ */

/*
   Fonction la plus complète pour la saisie.

   Le joueur peut entrer :
   - un nombre entre minimum et maximum
   - S pour sauvegarder
   - Q pour sauvegarder et quitter

   La fonction retourne :
   - le nombre choisi si l'utilisateur entre un nombre
   - 0 si l'utilisateur tape S ou Q

   Le paramètre action indique ce que l'utilisateur a fait :
   - ACTION_NONE : il a entré un nombre
   - ACTION_SAVE : il a tapé S
   - ACTION_QUIT : il a tapé Q
*/
int read_integer_or_action(const char *message,
                           int minimum,
                           int maximum,
                           int *action) {
    char input[INPUT_SIZE];
    int value;
    int result;

    /*
       On répète la demande tant que l'utilisateur
       n'entre pas quelque chose de correct.
    */
    while (TRUE) {
        /*
           Au début de chaque saisie,
           on considère qu'aucune action spéciale
           n'a été demandée.
        */
        *action = ACTION_NONE;

        /*
           Affiche les consignes au joueur.
        */
        printf("\n%s\n", message);
        printf("Entrez une valeur entre %d et %d.\n", minimum, maximum);
        printf("S = sauvegarder | Q = sauvegarder et quitter\n> ");

        /*
           Lit la saisie complète du joueur.
        */
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
        }

        /*
           Supprime le '\n' à la fin de la saisie.
           Exemple : "S\n" devient "S".
        */
        remove_newline(input);

        /*
           Si le joueur tape S ou s,
           on indique une action de sauvegarde.
        */
        if (is_save_command(input)) {
            *action = ACTION_SAVE;
            return 0;
        }

        /*
           Si le joueur tape Q ou q,
           on indique une action de sauvegarde + quitter.
        */
        if (is_quit_command(input)) {
            *action = ACTION_QUIT;
            return 0;
        }

        /*
           Si ce n'est pas S ou Q,
           on essaie de lire un nombre entier.
        */
        result = sscanf(input, "%d", &value);

        /*
           Si la conversion échoue,
           cela veut dire que l'utilisateur n'a pas tapé un nombre.
        */
        if (result != 1) {
            printf("Veuillez entrer un nombre valide.\n");
            continue;
        }

        /*
           Vérifie que le nombre est dans les limites autorisées.
        */
        if (value < minimum || value > maximum) {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue;
        }

        /*
           Si tout est correct, on retourne le nombre.
           action reste à ACTION_NONE.
        */
        return value;
    }
}

#include <stdio.h>
#include <string.h>
#include "input.h"
#include "constants.h"

/*
    fgets garde souvent le caractère '\n' à la fin de la chaîne.

    Exemple :
    si l’utilisateur tape "S", fgets peut stocker "S\n".

    Cette fonction supprime ce '\n'.
*/
static void remove_newline(char text[]) {
    int i;

    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            text[i] = '\0';
            return;
        }
    }
}

/*
    Vérifie si l’utilisateur a tapé S ou s.

    Cela sert à demander une sauvegarde.
*/
static int is_save_command(const char text[]) {
    return strcmp(text, "s") == 0 || strcmp(text, "S") == 0;
}

/*
    Demande un nombre entier à l’utilisateur.

    La fonction refuse :
    - les lettres
    - les nombres trop petits
    - les nombres trop grands

    Elle recommence tant que la saisie est incorrecte.
*/
int read_integer(const char *message, int minimum, int maximum) {
    char input[INPUT_SIZE];
    int value;
    int result;

    while (TRUE) {
        printf("%s", message);

        /*
            fgets lit une ligne complète au clavier.
        */
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
        }

        /*
            sscanf essaye de transformer le texte en entier.
            Si result vaut 1, la conversion a réussi.
        */
        result = sscanf(input, "%d", &value);

        if (result != 1) {
            printf("Veuillez entrer un nombre valide.\n");
            continue;
        }

        /*
            On vérifie que le nombre est dans l’intervalle autorisé.
        */
        if (value < minimum || value > maximum) {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue;
        }

        return value;
    }
}

/*
    Même principe que read_integer,
    mais l’utilisateur peut aussi taper S pour sauvegarder.
*/
int read_integer_or_save(const char *message, int minimum, int maximum, int *save_requested) {
    char input[INPUT_SIZE];
    int value;
    int result;

    /*
        Au départ, aucune sauvegarde n’est demandée.
    */
    *save_requested = FALSE;

    while (TRUE) {
        printf("%s\n", message);
        printf("Entrez une valeur entre %d et %d, ou S pour sauvegarder.\n> ",
               minimum,
               maximum);

        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
        }

        remove_newline(input);

        /*
            Si l’utilisateur tape S,
            on indique au programme qu’il veut sauvegarder.
        */
        if (is_save_command(input)) {
            *save_requested = TRUE;
            return 0;
        }

        result = sscanf(input, "%d", &value);

        if (result != 1) {
            printf("Veuillez entrer un nombre valide.\n");
            continue;
        }

        if (value < minimum || value > maximum) {
            printf("Valeur incorrecte. Entrez un nombre entre %d et %d.\n",
                   minimum,
                   maximum);
            continue;
        }

        return value;
    }
}

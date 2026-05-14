#include <stdio.h>
#include <string.h>

#include "input.h"
#include "constants.h"

static void remove_newline(char text[]) {
    int i;

    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            text[i] = '\0';
            return;
        }
    }
}

static int is_save_command(const char text[]) {
    return strcmp(text, "s") == 0 || strcmp(text, "S") == 0;
}

static int is_quit_command(const char text[]) {
    return strcmp(text, "q") == 0 || strcmp(text, "Q") == 0;
}

int read_integer(const char *message, int minimum, int maximum) {
    char input[INPUT_SIZE];
    int value;
    int result;

    while (TRUE) {
        printf("%s", message);

        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
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

int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested) {
    int action;
    int value;

    value = read_integer_or_action(message, minimum, maximum, &action);

    if (action == ACTION_SAVE) {
        *save_requested = TRUE;
    } else {
        *save_requested = FALSE;
    }

    return value;
}

int read_integer_or_action(const char *message,
                           int minimum,
                           int maximum,
                           int *action) {
    char input[INPUT_SIZE];
    int value;
    int result;

    while (TRUE) {
        *action = ACTION_NONE;

        printf("\n%s\n", message);
        printf("Entrez une valeur entre %d et %d.\n", minimum, maximum);
        printf("S = sauvegarder | Q = sauvegarder et quitter\n> ");

        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("Erreur de lecture. Reessayez.\n");
            continue;
        }

        remove_newline(input);

        if (is_save_command(input)) {
            *action = ACTION_SAVE;
            return 0;
        }

        if (is_quit_command(input)) {
            *action = ACTION_QUIT;
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

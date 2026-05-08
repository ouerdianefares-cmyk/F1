#ifndef INPUT_H
#define INPUT_H

/*
    input.h

    Ce fichier contient les prototypes des fonctions
    qui servent à lire les choix de l'utilisateur au clavier.

    Les fonctions seront codées dans input.c.
*/

/*
    Demande à l'utilisateur d'entrer un nombre entier.

    Paramètres :
    - message : le texte affiché à l'utilisateur
    - minimum : valeur minimale acceptée
    - maximum : valeur maximale acceptée

    Exemple :
    read_integer("Votre choix : ", 1, 3);

    L'utilisateur devra entrer 1, 2 ou 3.
*/
int read_integer(const char *message, int minimum, int maximum);

/*
    Demande à l'utilisateur d'entrer un nombre entier,
    mais permet aussi de taper S pour sauvegarder.

    Paramètres :
    - message : le texte affiché à l'utilisateur
    - minimum : valeur minimale acceptée
    - maximum : valeur maximale acceptée
    - save_requested : variable qui indiquera si l'utilisateur a tapé S

    Si l'utilisateur tape S :
    - la fonction met save_requested à TRUE
    - la sauvegarde sera faite ailleurs dans le programme
*/
int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested);

#endif

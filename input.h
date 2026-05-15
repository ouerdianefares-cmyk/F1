#ifndef INPUT_H
#define INPUT_H


/* ============================================================
   FONCTIONS DE SAISIE AU CLAVIER
   ============================================================ */

/*
   Ce fichier contient les prototypes des fonctions
   qui permettent de lire ce que l'utilisateur tape au clavier.

   Ces fonctions servent à éviter que le joueur entre
   n'importe quoi dans le programme.
*/


/* ============================================================
   LECTURE D'UN NOMBRE SIMPLE
   ============================================================ */

/*
   Demande à l'utilisateur d'entrer un nombre entier.

   Paramètres :
   - message : texte affiché pour demander la valeur
   - minimum : plus petite valeur autorisée
   - maximum : plus grande valeur autorisée

   La fonction vérifie que :
   - l'utilisateur entre bien un nombre
   - le nombre est entre minimum et maximum

   Exemple :
   read_integer("Choisissez une colonne : ", 1, 8);

   Ici, l'utilisateur devra entrer un nombre entre 1 et 8.
*/
int read_integer(const char *message, int minimum, int maximum);


/* ============================================================
   LECTURE D'UN NOMBRE OU D'UNE SAUVEGARDE
   ============================================================ */

/*
   Demande à l'utilisateur d'entrer :
   - soit un nombre entier
   - soit S pour sauvegarder

   Paramètres :
   - message : texte affiché à l'utilisateur
   - minimum : plus petite valeur autorisée
   - maximum : plus grande valeur autorisée
   - save_requested : indique si le joueur a demandé une sauvegarde

   save_requested vaudra :
   - TRUE si le joueur tape S
   - FALSE si le joueur tape un nombre

   La fonction retourne :
   - le nombre choisi si l'utilisateur tape un nombre
   - 0 si l'utilisateur tape S
*/
int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested);


/* ============================================================
   LECTURE D'UN NOMBRE OU D'UNE ACTION
   ============================================================ */

/*
   Demande à l'utilisateur d'entrer :
   - soit un nombre entier
   - soit S pour sauvegarder
   - soit Q pour sauvegarder et quitter

   Paramètres :
   - message : texte affiché à l'utilisateur
   - minimum : plus petite valeur autorisée
   - maximum : plus grande valeur autorisée
   - action : indique l'action choisie par le joueur

   action peut recevoir :
   - ACTION_NONE : le joueur a entré un nombre normal
   - ACTION_SAVE : le joueur a tapé S
   - ACTION_QUIT : le joueur a tapé Q

   La fonction retourne :
   - le nombre choisi si l'utilisateur tape un nombre
   - 0 si l'utilisateur tape S ou Q

   Les valeurs ACTION_NONE, ACTION_SAVE et ACTION_QUIT
   sont définies dans constants.h.
*/
int read_integer_or_action(const char *message,
                           int minimum,
                           int maximum,
                           int *action);


#endif /* INPUT_H */

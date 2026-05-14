#ifndef INPUT_H
#define INPUT_H

int read_integer(const char *message, int minimum, int maximum);

int read_integer_or_save(const char *message,
                         int minimum,
                         int maximum,
                         int *save_requested);

/*
    Nouvelle fonction :
    - nombre normal : ACTION_NONE
    - S : sauvegarder
    - Q : sauvegarder et quitter
*/
int read_integer_or_action(const char *message,
                           int minimum,
                           int maximum,
                           int *action);

#endif /* INPUT_H */

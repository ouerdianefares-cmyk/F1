#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "display.h"
#include "input.h"
#include "save.h"
#include "constants.h"


/* ============================================================
   PROTOTYPES DES FONCTIONS INTERNES
   ============================================================ */

/*
   Ces fonctions sont utilisées uniquement dans ce fichier game.c.

   Le mot-clé static signifie :
   - la fonction est privée
   - elle ne peut pas être appelée depuis un autre fichier
   - elle sert seulement à organiser le code ici
*/
static void initialize_board(Game *game);
static int is_inside_board(int row, int column);
static int is_column_playable(const Game *game, int column);
static Position drop_piece(Game *game, int column, char symbol);
static int ask_playable_column(Game *game);
static int ask_rotation_size(void);
static int is_valid_pivot(int row, int column, int size);
static int zone_contains_position(Position pivot, int size, Position position);
static Position ask_pivot(Game *game, int size, Position piece_position);
static int ask_rotation_direction(Game *game);
static void rotate_zone(Game *game, Position pivot, int size, int direction);
static void apply_gravity_in_zone(Game *game, Position pivot, int size);
static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol);
static int check_player_win(const Game *game, char symbol);

static int mark_winning_direction(const Game *game,
                                  int row,
                                  int column,
                                  int delta_row,
                                  int delta_column,
                                  char symbol,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);


/* ============================================================
   INITIALISATION DU JEU
   ============================================================ */

/*
   Initialise une nouvelle partie.

   Cette fonction prépare :
   - le nombre de joueurs
   - le joueur qui commence
   - le numéro du tour
   - le plateau vide
*/
void initialize_game(Game *game, int player_count) {
    game->player_count = player_count;
    game->current_player = 1;
    game->turn_number = 1;

    initialize_board(game);
}


/*
   Initialise le plateau.

   Étape 1 :
   Toutes les cases deviennent vides avec EMPTY_CELL.

   Étape 2 :
   On place les blocs # dans les quatre coins du plateau.
*/
static void initialize_board(Game *game) {
    int row;
    int column;

    /* Parcourt toutes les lignes */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        /* Parcourt toutes les colonnes */
        for (column = 0; column < BOARD_WIDTH; column++) {
            game->board[row][column] = EMPTY_CELL;
        }
    }

    /* Place les blocs dans les coins */
    game->board[0][0] = BLOCK_CELL;
    game->board[0][BOARD_WIDTH - 1] = BLOCK_CELL;
    game->board[BOARD_HEIGHT - 1][0] = BLOCK_CELL;
    game->board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] = BLOCK_CELL;
}


/* ============================================================
   VÉRIFICATIONS DE BASE
   ============================================================ */

/*
   Vérifie si une case est bien dans le plateau.

   Exemple :
   - ligne 0, colonne 0 : valide
   - ligne -1 : invalide
   - colonne 8 : invalide, car les colonnes vont de 0 à 7
*/
static int is_inside_board(int row, int column) {
    return row >= 0 &&
           row < BOARD_HEIGHT &&
           column >= 0 &&
           column < BOARD_WIDTH;
}


/*
   Vérifie si un joueur peut jouer dans une colonne.

   Une colonne est jouable seulement si :
   - elle existe dans le plateau
   - la case tout en haut est vide

   Si la case du haut n'est pas vide,
   cela veut dire que la colonne est pleine ou bloquée.
*/
static int is_column_playable(const Game *game, int column) {
    if (column < 0 || column >= BOARD_WIDTH) {
        return FALSE;
    }

    return game->board[0][column] == EMPTY_CELL;
}


/* ============================================================
   CHUTE D'UNE PIÈCE
   ============================================================ */

/*
   Fait tomber une pièce dans une colonne.

   Comme dans Puissance 4 :
   - le joueur choisit une colonne
   - la pièce descend jusqu'à la première case disponible
   - la pièce s'arrête avant un pion, un bloc ou le bas du plateau

   La fonction retourne la position finale de la pièce.
*/
static Position drop_piece(Game *game, int column, char symbol) {
    Position position;
    int row;

    /*
       Par défaut, on met row à -1.
       Cela permet de savoir si la pièce n'a pas pu être placée.
    */
    position.row = -1;
    position.column = column;

    /* Si la colonne n'est pas jouable, on retourne une position invalide */
    if (!is_column_playable(game, column)) {
        return position;
    }

    /*
       La pièce commence tout en haut,
       puis elle descend tant que la case en dessous est vide.
    */
    row = 0;

    while (row + 1 < BOARD_HEIGHT &&
           game->board[row + 1][column] == EMPTY_CELL) {
        row++;
    }

    /* On place le symbole du joueur dans la case finale */
    game->board[row][column] = symbol;

    /* On garde la ligne où la pièce est tombée */
    position.row = row;

    return position;
}


/* ============================================================
   DEMANDE DE LA COLONNE AU JOUEUR
   ============================================================ */

/*
   Demande au joueur dans quelle colonne il veut jouer.

   Le joueur peut :
   - entrer un nombre entre 1 et 8
   - taper S pour sauvegarder
   - taper Q pour sauvegarder et quitter

   Attention :
   Dans l'affichage, les colonnes vont de 1 à 8.
   Dans le code, les colonnes vont de 0 à 7.
   Donc on fait column-- après la saisie.
*/
static int ask_playable_column(Game *game) {
    int column;
    int action;

    while (TRUE) {
        column = read_integer_or_action(
            "Choisissez une colonne pour placer votre piece.",
            1,
            BOARD_WIDTH,
            &action
        );

        /* Si le joueur tape S, on sauvegarde et on continue */
        if (action == ACTION_SAVE) {
            save_game(game);
            continue;
        }

        /* Si le joueur tape Q, on sauvegarde et on quitte le tour */
        if (action == ACTION_QUIT) {
            save_game(game);
            return -1;
        }

        /*
           Le joueur entre une colonne entre 1 et 8.
           Le tableau C commence à 0.
           Exemple :
           - colonne 1 devient 0
           - colonne 4 devient 3
           - colonne 8 devient 7
        */
        column--;

        /* Si la colonne est pleine ou bloquée, on redemande */
        if (!is_column_playable(game, column)) {
            printf("Cette colonne est bloquee ou pleine. Choisissez une autre colonne.\n");
            continue;
        }

        return column;
    }
}


/* ============================================================
   TAILLE DE ROTATION
   ============================================================ */

/*
   Choisit automatiquement la taille de rotation.

   La rotation peut être :
   - 3 x 3
   - 5 x 5

   rand() % 2 donne soit 0, soit 1.
   Donc on utilise ça pour choisir entre 3 et 5.
*/
static int ask_rotation_size(void) {
    if (rand() % 2 == 0) {
        return 3;
    }

    return 5;
}


/* ============================================================
   VALIDATION DU PIVOT
   ============================================================ */

/*
   Vérifie si un pivot est valide.

   Le pivot est le centre du carré qui va tourner.

   Pour une rotation 3x3 :
   - radius = 1
   - le carré prend 1 case autour du pivot

   Pour une rotation 5x5 :
   - radius = 2
   - le carré prend 2 cases autour du pivot

   Le pivot est valide seulement si le carré entier
   reste dans le plateau.
*/
static int is_valid_pivot(int row, int column, int size) {
    int radius;

    radius = size / 2;

    return row - radius >= 0 &&
           row + radius < BOARD_HEIGHT &&
           column - radius >= 0 &&
           column + radius < BOARD_WIDTH;
}


/*
   Vérifie si la zone de rotation contient la pièce jouée.

   Règle importante du jeu :
   Le carré qu'on fait tourner doit obligatoirement contenir
   la pièce qui vient d'être placée.
*/
static int zone_contains_position(Position pivot, int size, Position position) {
    int radius;

    radius = size / 2;

    return position.row >= pivot.row - radius &&
           position.row <= pivot.row + radius &&
           position.column >= pivot.column - radius &&
           position.column <= pivot.column + radius;
}


/* ============================================================
   DEMANDE DU PIVOT AU JOUEUR
   ============================================================ */

/*
   Demande au joueur de choisir le pivot.

   Le pivot correspond au centre du carré qui va tourner.

   Le joueur entre :
   - une ligne
   - une colonne

   Exemple :
   Si le joueur choisit ligne 5 colonne 4 en 3x3,
   le carré tourné sera autour de cette case.

   Attention :
   Le joueur voit les lignes et colonnes de 1 à 6 / 1 à 8.
   Le code utilise les indices de 0 à 5 / 0 à 7.
   Donc on fait pivot.row-- et pivot.column--.
*/
static Position ask_pivot(Game *game, int size, Position piece_position) {
    Position pivot;
    int action;

    while (TRUE) {
        pivot.row = read_integer_or_action(
            "Choisissez la ligne du pivot.",
            1,
            BOARD_HEIGHT,
            &action
        );

        /* Sauvegarde simple */
        if (action == ACTION_SAVE) {
            save_game(game);
            continue;
        }

        /* Sauvegarde puis retour au menu */
        if (action == ACTION_QUIT) {
            save_game(game);
            pivot.row = -1;
            pivot.column = -1;
            return pivot;
        }

        pivot.column = read_integer_or_action(
            "Choisissez la colonne du pivot.",
            1,
            BOARD_WIDTH,
            &action
        );

        /* Sauvegarde simple */
        if (action == ACTION_SAVE) {
            save_game(game);
            continue;
        }

        /* Sauvegarde puis retour au menu */
        if (action == ACTION_QUIT) {
            save_game(game);
            pivot.row = -1;
            pivot.column = -1;
            return pivot;
        }

        /*
           Conversion :
           L'utilisateur tape 1, 2, 3...
           Le tableau utilise 0, 1, 2...
        */
        pivot.row--;
        pivot.column--;

        /*
           Première vérification :
           Le carré autour du pivot doit rester dans le plateau.
        */
        if (!is_valid_pivot(pivot.row, pivot.column, size)) {
            printf("Pivot invalide : la zone sortirait du plateau.\n");
            continue;
        }

        /*
           Deuxième vérification :
           La zone de rotation doit contenir la pièce insérée.
        */
        if (!zone_contains_position(pivot, size, piece_position)) {
            printf("Pivot invalide : la zone doit contenir la piece inseree.\n");
            continue;
        }

        return pivot;
    }
}


/* ============================================================
   DEMANDE DU SENS DE ROTATION
   ============================================================ */

/*
   Demande au joueur dans quel sens tourner la zone.

   Le joueur tape :
   - 1 pour tourner à gauche
   - 2 pour tourner à droite

   La fonction retourne :
   - ROTATE_LEFT si le joueur choisit gauche
   - ROTATE_RIGHT si le joueur choisit droite
*/
static int ask_rotation_direction(Game *game) {
    int choice;
    int action;

    while (TRUE) {
        choice = read_integer_or_action(
            "Sens de rotation : 1 = gauche, 2 = droite.",
            1,
            2,
            &action
        );

        /* S = sauvegarder */
        if (action == ACTION_SAVE) {
            save_game(game);
            continue;
        }

        /* Q = sauvegarder et quitter */
        if (action == ACTION_QUIT) {
            save_game(game);
            return 0;
        }

        if (choice == 1) {
            return ROTATE_LEFT;
        }

        return ROTATE_RIGHT;
    }
}


/* ============================================================
   ROTATION D'UNE ZONE
   ============================================================ */

/*
   Fait tourner une zone carrée de 90 degrés.

   La zone peut être :
   - 3 x 3
   - 5 x 5

   Fonctionnement :
   1. On copie d'abord la zone dans un tableau temporaire.
   2. On remet les valeurs dans le plateau dans un ordre différent.
   3. L'ordre dépend du sens de rotation.

   temporary sert à ne pas écraser les cases pendant la rotation.
*/
static void rotate_zone(Game *game, Position pivot, int size, int direction) {
    char temporary[5][5];
    int radius;
    int row;
    int column;
    int source_row;
    int source_column;
    int local_row;
    int local_column;

    radius = size / 2;

    /*
       Copie de la zone du plateau dans temporary.

       On transforme les coordonnées du plateau
       en coordonnées locales dans le carré.
    */
    for (row = 0; row < size; row++) {
        for (column = 0; column < size; column++) {
            source_row = pivot.row - radius + row;
            source_column = pivot.column - radius + column;

            temporary[row][column] = game->board[source_row][source_column];
        }
    }

    /*
       Réécriture de la zone tournée dans le plateau.
    */
    for (row = 0; row < size; row++) {
        for (column = 0; column < size; column++) {
            source_row = pivot.row - radius + row;
            source_column = pivot.column - radius + column;

            /*
               Rotation à droite :
               la case destination prend une ancienne case
               venant de la ligne/colonne calculée.
            */
            if (direction == ROTATE_RIGHT) {
                local_row = size - 1 - column;
                local_column = row;
            } else {
                /*
                   Rotation à gauche.
                */
                local_row = column;
                local_column = size - 1 - row;
            }

            game->board[source_row][source_column] = temporary[local_row][local_column];
        }
    }
}


/* ============================================================
   GRAVITÉ DANS LA ZONE TOURNÉE
   ============================================================ */

/*
   Applique la gravité dans la zone qui vient d'être tournée.

   Après une rotation, certaines pièces peuvent se retrouver
   "en l'air".

   Cette fonction les fait tomber vers le bas,
   mais seulement dans la zone de rotation.

   Les blocs # ne bougent pas.
   Ils bloquent aussi la chute des pièces.
*/
static void apply_gravity_in_zone(Game *game, Position pivot, int size) {
    int radius;
    int column;
    int row;
    int write_row;
    int top;
    int bottom;
    int left;
    int right;

    radius = size / 2;

    /* Calcule les limites de la zone */
    top = pivot.row - radius;
    bottom = pivot.row + radius;
    left = pivot.column - radius;
    right = pivot.column + radius;

    /*
       On traite chaque colonne de la zone.
       Les pièces tombent colonne par colonne.
    */
    for (column = left; column <= right; column++) {
        /*
           write_row indique l'endroit où la prochaine pièce
           doit tomber.
        */
        write_row = bottom;

        /*
           On part du bas vers le haut.
        */
        for (row = bottom; row >= top; row--) {
            if (game->board[row][column] == BLOCK_CELL) {
                /*
                   Si on rencontre un bloc,
                   les pièces au-dessus ne peuvent pas passer à travers.
                */
                write_row = row - 1;
            } else if (game->board[row][column] != EMPTY_CELL) {
                /*
                   Si on trouve une pièce,
                   on la déplace vers write_row si possible.
                */
                if (row != write_row) {
                    game->board[write_row][column] = game->board[row][column];
                    game->board[row][column] = EMPTY_CELL;
                }

                /*
                   La prochaine pièce tombera une case plus haut.
                */
                write_row--;
            }
        }
    }
}


/* ============================================================
   TOUR COMPLET D'UN JOUEUR
   ============================================================ */

/*
   Joue un tour complet.

   Déroulement :
   1. On affiche le plateau.
   2. Le jeu choisit une rotation 3x3 ou 5x5.
   3. Le joueur choisit une colonne.
   4. La pièce tombe.
   5. Le joueur choisit le pivot.
   6. Le joueur choisit gauche ou droite.
   7. La zone tourne.
   8. La gravité est appliquée.

   Retourne :
   - TRUE si le tour s'est terminé normalement
   - FALSE si le joueur a sauvegardé et quitté
*/
int play_turn(Game *game) {
    int column;
    int size;
    int direction;
    char symbol;
    Position piece_position;
    Position pivot;

    clear_screen();
    display_game(game);

    printf("\nTour du joueur %d.\n", game->current_player);

    /* Choisit aléatoirement 3x3 ou 5x5 */
    size = ask_rotation_size();
    printf("Taille de rotation imposee : %d x %d\n", size, size);

    /* Demande la colonne au joueur */
    column = ask_playable_column(game);

    if (column == -1) {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    /* Récupère le symbole du joueur actuel */
    symbol = get_player_symbol(game->current_player);

    /* Fait tomber la pièce dans la colonne choisie */
    piece_position = drop_piece(game, column, symbol);

    clear_screen();
    printf("Etape 1 : piece inseree puis tombee.\n");
    display_game(game);
    pause_screen();

    /* Demande le pivot de rotation */
    pivot = ask_pivot(game, size, piece_position);

    if (pivot.row == -1 || pivot.column == -1) {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    clear_screen();
    printf("Pivot choisi : ligne %d, colonne %d.\n", pivot.row + 1, pivot.column + 1);
    display_game_with_pivot(game, pivot);
    pause_screen();

    /* Demande le sens de rotation */
    direction = ask_rotation_direction(game);

    if (direction == 0) {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    /* Tourne la zone */
    rotate_zone(game, pivot, size, direction);

    clear_screen();
    printf("Etape 2 : zone tournee.\n");
    display_game_with_pivot(game, pivot);
    pause_screen();

    /* Applique la gravité après la rotation */
    apply_gravity_in_zone(game, pivot, size);

    clear_screen();
    printf("Etape 3 : gravite appliquee.\n");
    display_game(game);
    pause_screen();

    return TRUE;
}


/* ============================================================
   CHANGEMENT DE JOUEUR
   ============================================================ */

/*
   Passe au joueur suivant.

   Exemple avec 2 joueurs :
   - joueur 1 devient joueur 2
   - joueur 2 redevient joueur 1

   À chaque changement de joueur,
   on augmente aussi le numéro du tour.
*/
void change_player(Game *game) {
    game->current_player++;

    if (game->current_player > game->player_count) {
        game->current_player = 1;
    }

    game->turn_number++;
}


/* ============================================================
   PLATEAU PLEIN
   ============================================================ */

/*
   Vérifie si le plateau est plein.

   Si au moins une colonne est encore jouable,
   le plateau n'est pas plein.

   Si aucune colonne n'est jouable,
   la partie peut se terminer sur une égalité.
*/
int is_board_full(const Game *game) {
    int column;

    for (column = 0; column < BOARD_WIDTH; column++) {
        if (is_column_playable(game, column)) {
            return FALSE;
        }
    }

    return TRUE;
}


/* ============================================================
   VÉRIFICATION DES ALIGNEMENTS
   ============================================================ */

/*
   Vérifie s'il y a un alignement dans une direction précise.

   Paramètres importants :
   - delta_row : déplacement vertical
   - delta_column : déplacement horizontal

   Exemples :
   - delta_row = 0, delta_column = 1
     vérifie vers la droite

   - delta_row = 1, delta_column = 0
     vérifie vers le bas

   - delta_row = 1, delta_column = 1
     vérifie en diagonale bas-droite

   - delta_row = 1, delta_column = -1
     vérifie en diagonale bas-gauche
*/
static int check_direction(const Game *game,
                           int row,
                           int column,
                           int delta_row,
                           int delta_column,
                           char symbol) {
    int count;
    int current_row;
    int current_column;

    count = 0;
    current_row = row;
    current_column = column;

    /*
       On avance dans la direction donnée
       tant qu'on reste dans le plateau
       et tant qu'on trouve le même symbole.
    */
    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol) {
        count++;

        /*
           Si on atteint 5 pions alignés,
           le joueur a gagné.
        */
        if (count >= ALIGNMENT_LENGTH) {
            return TRUE;
        }

        current_row += delta_row;
        current_column += delta_column;
    }

    return FALSE;
}


/*
   Vérifie si un joueur précis a gagné.

   Pour chaque case du plateau :
   - si la case contient le symbole du joueur
   - on regarde dans les 4 directions possibles
*/
static int check_player_win(const Game *game, char symbol) {
    int row;
    int column;

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            if (game->board[row][column] == symbol) {
                /* Horizontal */
                if (check_direction(game, row, column, 0, 1, symbol)) {
                    return TRUE;
                }

                /* Vertical */
                if (check_direction(game, row, column, 1, 0, symbol)) {
                    return TRUE;
                }

                /* Diagonale descendante */
                if (check_direction(game, row, column, 1, 1, symbol)) {
                    return TRUE;
                }

                /* Diagonale montante */
                if (check_direction(game, row, column, 1, -1, symbol)) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}


/* ============================================================
   RECHERCHE DES GAGNANTS
   ============================================================ */

/*
   Cherche tous les joueurs qui ont gagné.

   Le tableau winners reçoit les numéros des joueurs gagnants.

   Exemple :
   winners[0] = 1 veut dire que le joueur 1 a gagné.

   La fonction retourne le nombre de gagnants.
*/
int check_winners(const Game *game, int winners[]) {
    int player;
    int winner_count;
    char symbol;

    winner_count = 0;

    for (player = 1; player <= game->player_count; player++) {
        symbol = get_player_symbol(player);

        if (check_player_win(game, symbol)) {
            winners[winner_count] = player;
            winner_count++;
        }
    }

    return winner_count;
}


/* ============================================================
   MARQUAGE DES CASES GAGNANTES
   ============================================================ */

/*
   Marque les cases qui composent un alignement gagnant.

   Cette fonction sert pour l'affichage :
   les cases gagnantes pourront être affichées avec une couleur spéciale.
*/
static int mark_winning_direction(const Game *game,
                                  int row,
                                  int column,
                                  int delta_row,
                                  int delta_column,
                                  char symbol,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    int count;
    int current_row;
    int current_column;
    int i;

    count = 0;
    current_row = row;
    current_column = column;

    /*
       On compte les pions identiques dans une direction.
    */
    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol) {
        count++;

        /*
           Si on trouve 5 pions alignés,
           on revient au début et on marque les 5 cases.
        */
        if (count >= ALIGNMENT_LENGTH) {
            current_row = row;
            current_column = column;

            for (i = 0; i < ALIGNMENT_LENGTH; i++) {
                winning_cells[current_row][current_column] = TRUE;
                current_row += delta_row;
                current_column += delta_column;
            }

            return TRUE;
        }

        current_row += delta_row;
        current_column += delta_column;
    }

    return FALSE;
}


/*
   Remplit le tableau winning_cells avec les cases gagnantes.

   Au début, toutes les cases sont mises à FALSE.

   Ensuite, pour chaque joueur, on cherche les alignements.
   Si un alignement est trouvé, les cases gagnantes passent à TRUE.

   Retourne :
   - TRUE si au moins un alignement gagnant est trouvé
   - FALSE sinon
*/
int get_winning_cells(const Game *game,
                      int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    int row;
    int column;
    int player;
    int found;
    char symbol;

    found = FALSE;

    /*
       Initialisation :
       aucune case n'est gagnante au départ.
    */
    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (column = 0; column < BOARD_WIDTH; column++) {
            winning_cells[row][column] = FALSE;
        }
    }

    /*
       On vérifie les alignements pour chaque joueur.
    */
    for (player = 1; player <= game->player_count; player++) {
        symbol = get_player_symbol(player);

        for (row = 0; row < BOARD_HEIGHT; row++) {
            for (column = 0; column < BOARD_WIDTH; column++) {
                if (game->board[row][column] == symbol) {
                    /* Horizontal */
                    if (mark_winning_direction(game, row, column, 0, 1, symbol, winning_cells)) {
                        found = TRUE;
                    }

                    /* Vertical */
                    if (mark_winning_direction(game, row, column, 1, 0, symbol, winning_cells)) {
                        found = TRUE;
                    }

                    /* Diagonale descendante */
                    if (mark_winning_direction(game, row, column, 1, 1, symbol, winning_cells)) {
                        found = TRUE;
                    }

                    /* Diagonale montante */
                    if (mark_winning_direction(game, row, column, 1, -1, symbol, winning_cells)) {
                        found = TRUE;
                    }
                }
            }
        }
    }

    return found;
}

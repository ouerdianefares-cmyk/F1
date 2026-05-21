#include <stdio.h>  /* Permet d'utiliser printf */
#include <stdlib.h> /* Permet d'utiliser rand */
#include <time.h>   /* Permet d'utiliser le temps pour l'aléatoire */

#include "game.h"      /* Contient les prototypes liés au jeu */
#include "display.h"   /* Contient les fonctions d'affichage */
#include "input.h"     /* Contient les fonctions de saisie */
#include "save.h"      /* Contient les fonctions de sauvegarde */
#include "constants.h" /* Contient les constantes du jeu */


/* PROTOTYPES DES FONCTIONS INTERNES */

/* Initialise le plateau */
static void initialize_board(Game *game);

/* Vérifie si une position est dans le plateau */
static int is_inside_board(int row, int column);

/* Vérifie si une colonne peut recevoir une pièce */
static int is_column_playable(const Game *game, int column);

/* Fait tomber une pièce dans une colonne */
static Position drop_piece(Game *game, int column, char symbol);

/* Demande une colonne jouable au joueur */
static int ask_playable_column(Game *game);

/* Choisit une taille de rotation : 3 ou 5 */
static int ask_rotation_size(void);

/* Vérifie si le pivot ne fait pas sortir la zone du plateau */
static int is_valid_pivot(int row, int column, int size);

/* Vérifie si la zone de rotation contient la pièce jouée */
static int zone_contains_position(Position pivot, int size, Position position);

/* Demande un pivot valide au joueur */
static Position ask_pivot(Game *game, int size, Position piece_position);

/* Demande le sens de rotation */
static int ask_rotation_direction(Game *game);

/* Tourne une zone du plateau */
static void rotate_zone(Game *game, Position pivot, int size, int direction);

/* Applique la gravité sur tout le plateau */
static void apply_gravity(Game *game);

/* Vérifie un alignement dans une direction */
static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol);

/* Vérifie si un joueur a gagné */
static int check_player_win(const Game *game, char symbol);

/* Marque les cases gagnantes */
static int mark_winning_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);


/* INITIALISATION DU JEU */

void initialize_game(Game *game, int player_count) /* Initialise une nouvelle partie */
{
    game->player_count = player_count; /* Stocke le nombre de joueurs */
    game->current_player = 1; /* Le joueur 1 commence */
    game->turn_number = 1; /* Le premier tour commence à 1 */
    initialize_board(game); /* Initialise le plateau */
}

static void initialize_board(Game *game) /* Remplit le plateau au début */
{
    int row; /* Variable pour parcourir les lignes */
    int column; /* Variable pour parcourir les colonnes */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            game->board[row][column] = EMPTY_CELL; /* Met une case vide */
        }
    }

    game->board[0][0] = BLOCK_CELL; /* Place un bloc en haut à gauche */
    game->board[0][BOARD_WIDTH - 1] = BLOCK_CELL; /* Place un bloc en haut à droite */
    game->board[BOARD_HEIGHT - 1][0] = BLOCK_CELL; /* Place un bloc en bas à gauche */
    game->board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] = BLOCK_CELL; /* Place un bloc en bas à droite */
}


/* VÉRIFICATIONS DE BASE */

static int is_inside_board(int row, int column) /* Vérifie si une case existe dans le plateau */
{
    return row >= 0 && row < BOARD_HEIGHT && column >= 0 && column < BOARD_WIDTH; /* Retourne TRUE si la position est valide */
}

static int is_column_playable(const Game *game, int column) /* Vérifie si une colonne est jouable */
{
    if (column < 0 || column >= BOARD_WIDTH) /* Si la colonne est hors du plateau */
    {
        return FALSE; /* La colonne n'est pas jouable */
    }

    return game->board[0][column] == EMPTY_CELL; /* La colonne est jouable si la case du haut est vide */
}


/* CHUTE D'UNE PIÈCE APRÈS INSERTION */

static Position drop_piece(Game *game, int column, char symbol) /* Fait tomber une pièce dans une colonne */
{
    Position position; /* Stocke la position finale de la pièce */
    int row; /* Variable pour parcourir les lignes */

    position.row = -1; /* Valeur invalide par défaut */
    position.column = column; /* La colonne reste celle choisie */

    if (!is_column_playable(game, column)) /* Si la colonne n'est pas jouable */
    {
        return position; /* Retourne une position invalide */
    }

    row = 0; /* La pièce commence en haut */

    while (row + 1 < BOARD_HEIGHT && game->board[row + 1][column] == EMPTY_CELL) /* Tant que la case en dessous est vide */
    {
        row++; /* La pièce descend d'une ligne */
    }

    game->board[row][column] = symbol; /* Place la pièce à sa position finale */
    position.row = row; /* Enregistre la ligne finale */

    return position; /* Retourne la position de la pièce */
}


/* DEMANDE DE LA COLONNE */

static int ask_playable_column(Game *game) /* Demande au joueur une colonne valide */
{
    int column; /* Stocke la colonne choisie */
    int action; /* Stocke une action spéciale : sauvegarde ou quitter */

    while (TRUE) /* Répète tant que la saisie n'est pas valide */
    {
        column = read_integer_or_action( /* Lit un nombre ou une action */
            "Choisissez une colonne pour placer votre piece.", /* Message affiché */
            1, /* Valeur minimale */
            BOARD_WIDTH, /* Valeur maximale */
            &action /* Récupère l'action choisie */
        );

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde la partie */
            continue; /* Redemande une colonne */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde la partie */
            return -1; /* Indique qu'il faut quitter le tour */
        }

        column--; /* Convertit la colonne utilisateur en indice C */

        if (!is_column_playable(game, column)) /* Si la colonne est pleine ou bloquée */
        {
            printf("Cette colonne est bloquee ou pleine. Choisissez une autre colonne.\n"); /* Message d'erreur */
            continue; /* Redemande une colonne */
        }

        return column; /* Retourne la colonne valide */
    }
}


/* TAILLE DE ROTATION */

static int ask_rotation_size(void) /* Choisit aléatoirement 3x3 ou 5x5 */
{
    if (rand() % 2 == 0) /* Si le nombre aléatoire vaut 0 */
    {
        return 3; /* Rotation 3x3 */
    }

    return 5; /* Rotation 5x5 */
}


/* PIVOT */

static int is_valid_pivot(int row, int column, int size) /* Vérifie si le pivot est valide */
{
    int radius; /* Rayon autour du pivot */

    radius = size / 2; /* Pour 3x3 : 1, pour 5x5 : 2 */

    return row - radius >= 0 && /* Vérifie que la zone ne sort pas en haut */
           row + radius < BOARD_HEIGHT && /* Vérifie que la zone ne sort pas en bas */
           column - radius >= 0 && /* Vérifie que la zone ne sort pas à gauche */
           column + radius < BOARD_WIDTH; /* Vérifie que la zone ne sort pas à droite */
}

static int zone_contains_position(Position pivot, int size, Position position) /* Vérifie si la pièce est dans la zone */
{
    int radius; /* Rayon autour du pivot */

    radius = size / 2; /* Calcule le rayon selon la taille */

    return position.row >= pivot.row - radius && /* La pièce doit être sous ou sur le haut de la zone */
           position.row <= pivot.row + radius && /* La pièce doit être au-dessus ou sur le bas de la zone */
           position.column >= pivot.column - radius && /* La pièce doit être à droite ou sur la gauche de la zone */
           position.column <= pivot.column + radius; /* La pièce doit être à gauche ou sur la droite de la zone */
}

static Position ask_pivot(Game *game, int size, Position piece_position) /* Demande un pivot au joueur */
{
    Position pivot; /* Stocke le pivot choisi */
    int action; /* Stocke une action spéciale */

    while (TRUE) /* Répète tant que le pivot n'est pas valide */
    {
        pivot.row = read_integer_or_action( /* Demande la ligne du pivot */
            "Choisissez la ligne du pivot, c'est-a-dire le centre du carre a tourner.", /* Message affiché */
            1, /* Ligne minimale */
            BOARD_HEIGHT, /* Ligne maximale */
            &action /* Récupère l'action */
        );

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde */
            continue; /* Redemande le pivot */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde */
            pivot.row = -1; /* Pivot invalide */
            pivot.column = -1; /* Pivot invalide */
            return pivot; /* Retourne pour quitter */
        }

        pivot.column = read_integer_or_action( /* Demande la colonne du pivot */
            "Choisissez la colonne du pivot, c'est-a-dire le centre du carre a tourner.", /* Message affiché */
            1, /* Colonne minimale */
            BOARD_WIDTH, /* Colonne maximale */
            &action /* Récupère l'action */
        );

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde */
            continue; /* Redemande le pivot */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde */
            pivot.row = -1; /* Pivot invalide */
            pivot.column = -1; /* Pivot invalide */
            return pivot; /* Retourne pour quitter */
        }

        pivot.row--; /* Convertit la ligne en indice C */
        pivot.column--; /* Convertit la colonne en indice C */

        if (!is_valid_pivot(pivot.row, pivot.column, size)) /* Si la zone sort du plateau */
        {
            printf("Pivot invalide : la zone sortirait du plateau.\n"); /* Message d'erreur */
            continue; /* Redemande un pivot */
        }

        if (!zone_contains_position(pivot, size, piece_position)) /* Si la zone ne contient pas la pièce jouée */
        {
            printf("Pivot invalide : la zone doit contenir la piece inseree.\n"); /* Message d'erreur */
            continue; /* Redemande un pivot */
        }

        return pivot; /* Retourne le pivot valide */
    }
}


/* SENS DE ROTATION */

static int ask_rotation_direction(Game *game) /* Demande gauche ou droite */
{
    int choice; /* Stocke le choix du joueur */
    int action; /* Stocke une action spéciale */

    while (TRUE) /* Répète tant que la saisie n'est pas correcte */
    {
        choice = read_integer_or_action( /* Lit le choix du joueur */
            "Sens de rotation : 1 = gauche, 2 = droite.", /* Message affiché */
            1, /* Valeur minimale */
            2, /* Valeur maximale */
            &action /* Récupère l'action */
        );

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde */
            continue; /* Redemande le sens */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde */
            return 0; /* Indique qu'il faut quitter */
        }

        if (choice == 1) /* Si le joueur choisit gauche */
        {
            return ROTATE_LEFT; /* Retourne la rotation gauche */
        }

        return ROTATE_RIGHT; /* Sinon retourne la rotation droite */
    }
}


/* ROTATION DU CARRÉ */

static void rotate_zone(Game *game, Position pivot, int size, int direction) /* Tourne une zone carrée */
{
    char temporary[5][5]; /* Tableau temporaire pour stocker la zone */
    int radius; /* Rayon de la zone */
    int row; /* Variable de ligne */
    int column; /* Variable de colonne */
    int source_row; /* Ligne réelle dans le plateau */
    int source_column; /* Colonne réelle dans le plateau */
    int local_row; /* Ligne locale dans la zone */
    int local_column; /* Colonne locale dans la zone */

    radius = size / 2; /* Calcule le rayon */

    for (row = 0; row < size; row++) /* Parcourt les lignes de la zone */
    {
        for (column = 0; column < size; column++) /* Parcourt les colonnes de la zone */
        {
            source_row = pivot.row - radius + row; /* Calcule la ligne dans le plateau */
            source_column = pivot.column - radius + column; /* Calcule la colonne dans le plateau */
            temporary[row][column] = game->board[source_row][source_column]; /* Copie la case dans temporary */
        }
    }

    for (row = 0; row < size; row++) /* Parcourt les lignes de destination */
    {
        for (column = 0; column < size; column++) /* Parcourt les colonnes de destination */
        {
            source_row = pivot.row - radius + row; /* Calcule la ligne dans le plateau */
            source_column = pivot.column - radius + column; /* Calcule la colonne dans le plateau */

            /*
                Correction du sens de rotation :

                ROTATE_RIGHT = rotation vers la droite, sens horaire.
                ROTATE_LEFT  = rotation vers la gauche, sens antihoraire.
            */
            if (direction == ROTATE_RIGHT) /* Si on tourne vers la droite */
            {
                local_row = column; /* Calcule l'ancienne ligne à récupérer */
                local_column = size - 1 - row; /* Calcule l'ancienne colonne à récupérer */
            }
            else /* Sinon, on tourne vers la gauche */
            {
                local_row = size - 1 - column; /* Calcule l'ancienne ligne à récupérer */
                local_column = row; /* Calcule l'ancienne colonne à récupérer */
            }

            game->board[source_row][source_column] = temporary[local_row][local_column]; /* Replace la case tournée */
        }
    }
}


/* GRAVITÉ SUR TOUT LE PLATEAU */

static void apply_gravity(Game *game) /* Fait tomber toutes les pièces */
{
    int column; /* Variable pour parcourir les colonnes */
    int row; /* Variable pour parcourir les lignes */
    int write_row; /* Ligne où la prochaine pièce doit tomber */

    for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt chaque colonne */
    {
        write_row = BOARD_HEIGHT - 1; /* Commence en bas de la colonne */

        for (row = BOARD_HEIGHT - 1; row >= 0; row--) /* Remonte la colonne */
        {
            if (game->board[row][column] == BLOCK_CELL) /* Si on trouve un bloc */
            {
                write_row = row - 1; /* Les pièces au-dessus tomberont au-dessus du bloc */
            }
            else if (game->board[row][column] != EMPTY_CELL) /* Si on trouve une pièce */
            {
                if (row != write_row) /* Si la pièce peut tomber */
                {
                    game->board[write_row][column] = game->board[row][column]; /* Déplace la pièce vers le bas */
                    game->board[row][column] = EMPTY_CELL; /* Vide l'ancienne case */
                }

                write_row--; /* La prochaine pièce tombera une case plus haut */
            }
        }
    }
}


/* TOUR COMPLET D'UN JOUEUR */

int play_turn(Game *game) /* Joue un tour complet */
{
    int column; /* Colonne choisie */
    int size; /* Taille de rotation */
    int direction; /* Sens de rotation */
    char symbol; /* Symbole du joueur */
    Position piece_position; /* Position de la pièce jouée */
    Position pivot; /* Pivot choisi */

    clear_screen(); /* Efface l'écran */
    display_game(game); /* Affiche le plateau */

    size = ask_rotation_size(); /* Choisit 3x3 ou 5x5 */

    printf("\n================================\n"); /* Affiche le haut du bloc d'information */
    printf("Tour du joueur %d (%c)\n", /* Affiche le joueur actuel */
           game->current_player, /* Numéro du joueur */
           get_player_symbol(game->current_player)); /* Symbole du joueur */
    printf("Rotation imposee : %d x %d\n", size, size); /* Affiche la taille de rotation */
    printf("Objectif : aligner %d pions\n", ALIGNMENT_LENGTH); /* Affiche l'objectif */
    printf("================================\n"); /* Affiche le bas du bloc d'information */

    column = ask_playable_column(game); /* Demande une colonne */

    if (column == -1) /* Si le joueur veut quitter */
    {
        printf("Partie sauvegardee. Retour au menu.\n"); /* Message d'information */
        return FALSE; /* Le tour ne continue pas */
    }

    symbol = get_player_symbol(game->current_player); /* Récupère le symbole du joueur */

    piece_position = drop_piece(game, column, symbol); /* Fait tomber la pièce */

    clear_screen(); /* Efface l'écran */
    printf("Etape 1 : piece inseree puis tombee.\n"); /* Affiche l'étape */
    display_game(game); /* Affiche le plateau */
    pause_screen(); /* Attend Entrée */

    pivot = ask_pivot(game, size, piece_position); /* Demande le pivot */

    if (pivot.row == -1 || pivot.column == -1) /* Si le joueur quitte pendant le pivot */
    {
        printf("Partie sauvegardee. Retour au menu.\n"); /* Message d'information */
        return FALSE; /* Le tour s'arrête */
    }

    clear_screen(); /* Efface l'écran */
    printf("Pivot choisi : ligne %d, colonne %d.\n", pivot.row + 1, pivot.column + 1); /* Affiche le pivot */
    display_game_with_pivot(game, pivot); /* Affiche le plateau avec le pivot */
    pause_screen(); /* Attend Entrée */

    direction = ask_rotation_direction(game); /* Demande le sens */

    if (direction == 0) /* Si le joueur quitte pendant le choix du sens */
    {
        printf("Partie sauvegardee. Retour au menu.\n"); /* Message d'information */
        return FALSE; /* Le tour s'arrête */
    }

    rotate_zone(game, pivot, size, direction); /* Tourne la zone */

    clear_screen(); /* Efface l'écran */
    printf("Etape 2 : zone tournee.\n"); /* Affiche l'étape */
    display_game_with_pivot(game, pivot); /* Affiche le plateau avec pivot */
    pause_screen(); /* Attend Entrée */

    apply_gravity(game); /* Applique la gravité sur tout le plateau */

    clear_screen(); /* Efface l'écran */
    printf("Etape 3 : gravite appliquee sur tout le plateau.\n"); /* Affiche l'étape */
    display_game(game); /* Affiche le plateau final */
    pause_screen(); /* Attend Entrée */

    return TRUE; /* Le tour s'est terminé normalement */
}


/* CHANGEMENT DE JOUEUR */

void change_player(Game *game) /* Passe au joueur suivant */
{
    game->current_player++; /* Passe au joueur suivant */

    if (game->current_player > game->player_count) /* Si on dépasse le dernier joueur */
    {
        game->current_player = 1; /* On revient au joueur 1 */
    }

    game->turn_number++; /* Augmente le numéro du tour */
}


/* PLATEAU PLEIN */

int is_board_full(const Game *game) /* Vérifie si le plateau est plein */
{
    int column; /* Variable pour parcourir les colonnes */

    for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
    {
        if (is_column_playable(game, column)) /* Si une colonne est encore jouable */
        {
            return FALSE; /* Le plateau n'est pas plein */
        }
    }

    return TRUE; /* Aucune colonne n'est jouable */
}


/* VÉRIFICATION DES ALIGNEMENTS */

static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol) /* Vérifie une direction */
{
    int count; /* Compte les pions alignés */
    int current_row; /* Ligne actuelle */
    int current_column; /* Colonne actuelle */

    count = 0; /* Aucun pion compté au départ */
    current_row = row; /* Commence à la ligne donnée */
    current_column = column; /* Commence à la colonne donnée */

    while (is_inside_board(current_row, current_column) && /* Tant qu'on reste dans le plateau */
           game->board[current_row][current_column] == symbol) /* Et tant que le symbole est le bon */
    {
        count++; /* Ajoute un pion aligné */

        if (count >= ALIGNMENT_LENGTH) /* Si on a assez de pions */
        {
            return TRUE; /* Le joueur a gagné */
        }

        current_row += delta_row; /* Avance dans la direction verticale */
        current_column += delta_column; /* Avance dans la direction horizontale */
    }

    return FALSE; /* Pas d'alignement trouvé */
}

static int check_player_win(const Game *game, char symbol) /* Vérifie si un joueur a gagné */
{
    int row; /* Variable de ligne */
    int column; /* Variable de colonne */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            if (game->board[row][column] == symbol) /* Si la case contient le symbole du joueur */
            {
                if (check_direction(game, row, column, 0, 1, symbol)) /* Vérifie horizontalement */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, 0, symbol)) /* Vérifie verticalement */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, 1, symbol)) /* Vérifie diagonale bas-droite */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, -1, symbol)) /* Vérifie diagonale bas-gauche */
                {
                    return TRUE; /* Victoire trouvée */
                }
            }
        }
    }

    return FALSE; /* Le joueur n'a pas gagné */
}


/* RECHERCHE DES GAGNANTS */

int check_winners(const Game *game, int winners[]) /* Cherche tous les gagnants */
{
    int player; /* Numéro du joueur testé */
    int winner_count; /* Nombre de gagnants */
    char symbol; /* Symbole du joueur testé */

    winner_count = 0; /* Aucun gagnant au départ */

    for (player = 1; player <= game->player_count; player++) /* Parcourt tous les joueurs */
    {
        symbol = get_player_symbol(player); /* Récupère le symbole du joueur */

        if (check_player_win(game, symbol)) /* Si ce joueur a gagné */
        {
            winners[winner_count] = player; /* Ajoute le joueur au tableau des gagnants */
            winner_count++; /* Augmente le nombre de gagnants */
        }
    }

    return winner_count; /* Retourne le nombre de gagnants */
}


/* MARQUAGE DES CASES GAGNANTES */

static int mark_winning_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) /* Marque une ligne gagnante */
{
    int count; /* Compte les pions alignés */
    int current_row; /* Ligne actuelle */
    int current_column; /* Colonne actuelle */
    int i; /* Variable de boucle */

    count = 0; /* Aucun pion compté au départ */
    current_row = row; /* Commence à la ligne donnée */
    current_column = column; /* Commence à la colonne donnée */

    while (is_inside_board(current_row, current_column) && /* Tant que la position est valide */
           game->board[current_row][current_column] == symbol) /* Et que la case contient le bon symbole */
    {
        count++; /* Compte un pion */

        if (count >= ALIGNMENT_LENGTH) /* Si on trouve un alignement gagnant */
        {
            current_row = row; /* Revient au début de l'alignement */
            current_column = column; /* Revient au début de l'alignement */

            for (i = 0; i < ALIGNMENT_LENGTH; i++) /* Parcourt les cases gagnantes */
            {
                winning_cells[current_row][current_column] = TRUE; /* Marque la case comme gagnante */
                current_row += delta_row; /* Passe à la case suivante en ligne */
                current_column += delta_column; /* Passe à la case suivante en colonne */
            }

            return TRUE; /* Indique qu'un alignement a été marqué */
        }

        current_row += delta_row; /* Continue dans la direction */
        current_column += delta_column; /* Continue dans la direction */
    }

    return FALSE; /* Aucun alignement marqué */
}

int get_winning_cells(const Game *game, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) /* Remplit le tableau des cases gagnantes */
{
    int row; /* Variable de ligne */
    int column; /* Variable de colonne */
    int player; /* Variable pour parcourir les joueurs */
    int found; /* Indique si une victoire a été trouvée */
    char symbol; /* Symbole du joueur testé */

    found = FALSE; /* Aucune victoire trouvée au départ */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            winning_cells[row][column] = FALSE; /* Initialise chaque case comme non gagnante */
        }
    }

    for (player = 1; player <= game->player_count; player++) /* Parcourt tous les joueurs */
    {
        symbol = get_player_symbol(player); /* Récupère le symbole du joueur */

        for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
        {
            for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
            {
                if (game->board[row][column] == symbol) /* Si la case contient le symbole du joueur */
                {
                    if (mark_winning_direction(game, row, column, 0, 1, symbol, winning_cells)) /* Marque horizontalement */
                    {
                        found = TRUE; /* Une victoire a été trouvée */
                    }

                    if (mark_winning_direction(game, row, column, 1, 0, symbol, winning_cells)) /* Marque verticalement */
                    {
                        found = TRUE; /* Une victoire a été trouvée */
                    }

                    if (mark_winning_direction(game, row, column, 1, 1, symbol, winning_cells)) /* Marque diagonale bas-droite */
                    {
                        found = TRUE; /* Une victoire a été trouvée */
                    }

                    if (mark_winning_direction(game, row, column, 1, -1, symbol, winning_cells)) /* Marque diagonale bas-gauche */
                    {
                        found = TRUE; /* Une victoire a été trouvée */
                    }
                }
            }
        }
    }

    return found; /* Retourne TRUE si une victoire existe */
}

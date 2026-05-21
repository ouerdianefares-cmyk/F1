#include <stdio.h>   /* Permet d'utiliser printf pour afficher du texte */
#include <stdlib.h>  /* Permet d'utiliser rand pour choisir aléatoirement 3x3 ou 5x5 */
#include <time.h>    /* Peut servir à initialiser l'aléatoire avec le temps */

#include "game.h"      /* Déclare les fonctions principales du jeu */
#include "display.h"   /* Permet d'afficher le plateau, le pivot et les cases gagnantes */
#include "input.h"     /* Gère les saisies utilisateur sécurisées */
#include "save.h"      /* Permet de sauvegarder la partie */
#include "constants.h" /* Regroupe les constantes : taille du plateau, symboles, TRUE/FALSE... */


/* PROTOTYPES DES FONCTIONS INTERNES */

/*
   Les fonctions ci-dessous sont en static.
   Cela veut dire qu'elles sont privées à ce fichier game.c.
   Elles servent à organiser le code sans être appelées directement depuis main.c.
*/

static void initialize_board(Game *game); /* Prépare le plateau au début de la partie */

static int is_inside_board(int row, int column); /* Vérifie si une case existe dans le plateau */

static int is_column_playable(const Game *game, int column); /* Vérifie si une colonne peut recevoir un pion */

static Position drop_piece(Game *game, int column, char symbol); /* Fait tomber un pion dans une colonne */

static int ask_playable_column(Game *game); /* Demande au joueur une colonne valide */

static int ask_rotation_size(void); /* Choisit aléatoirement une rotation 3x3 ou 5x5 */

static int is_valid_pivot(int row, int column, int size); /* Vérifie si le carré autour du pivot reste dans le plateau */

static int zone_contains_position(Position pivot, int size, Position position); /* Vérifie si la zone de rotation contient le pion joué */

static Position ask_pivot(Game *game, int size, Position piece_position); /* Demande au joueur un pivot valide */

static int ask_rotation_direction(Game *game); /* Demande au joueur le sens de rotation */

static void rotate_zone(Game *game, Position pivot, int size, int direction); /* Tourne la zone 3x3 ou 5x5 */

static void apply_gravity(Game *game); /* Applique la gravité sur tout le plateau */

static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol); /* Vérifie un alignement dans une direction */

static int check_player_win(const Game *game, char symbol); /* Vérifie si un joueur précis a gagné */

static int mark_winning_direction(const Game *game,
                                  int row,
                                  int column,
                                  int delta_row,
                                  int delta_column,
                                  char symbol,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]); /* Marque les cases gagnantes */


/* INITIALISATION DU JEU */

/*
   Cette fonction initialise une nouvelle partie.

   Elle fixe :
   - le nombre de joueurs
   - le joueur qui commence
   - le numéro du tour
   - le plateau de départ
*/
void initialize_game(Game *game, int player_count)
{
    game->player_count = player_count; /* Stocke le nombre de joueurs choisi au menu */
    game->current_player = 1; /* Le joueur 1 commence toujours la partie */
    game->turn_number = 1; /* Le compteur de tours commence à 1 */

    initialize_board(game); /* Remplit le plateau avec des cases vides et des blocs */
}

/*
   Cette fonction prépare le plateau.

   D'abord, elle met toutes les cases en vide.
   Ensuite, elle place les blocs dans les quatre coins.
*/
static void initialize_board(Game *game)
{
    int row;    /* Sert à parcourir les lignes du plateau */
    int column; /* Sert à parcourir les colonnes du plateau */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt chaque ligne du plateau */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt chaque colonne de la ligne */
        {
            game->board[row][column] = EMPTY_CELL; /* Chaque case devient vide au début */
        }
    }

    game->board[0][0] = BLOCK_CELL; /* Place un bloc dans le coin haut gauche */
    game->board[0][BOARD_WIDTH - 1] = BLOCK_CELL; /* Place un bloc dans le coin haut droit */
    game->board[BOARD_HEIGHT - 1][0] = BLOCK_CELL; /* Place un bloc dans le coin bas gauche */
    game->board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] = BLOCK_CELL; /* Place un bloc dans le coin bas droit */
}


/* VÉRIFICATIONS DE BASE */

/*
   Cette fonction vérifie si une position est bien dans la grille.

   Important :
   en C, les indices commencent à 0.
   Donc les lignes vont de 0 à BOARD_HEIGHT - 1
   et les colonnes vont de 0 à BOARD_WIDTH - 1.
*/
static int is_inside_board(int row, int column)
{
    return row >= 0 &&
           row < BOARD_HEIGHT &&
           column >= 0 &&
           column < BOARD_WIDTH; /* Retourne TRUE si la case est dans le plateau */
}

/*
   Cette fonction vérifie si une colonne est jouable.

   Une colonne est jouable si :
   - elle existe
   - la case tout en haut est vide

   Si la case du haut est occupée, la colonne est pleine ou bloquée.
*/
static int is_column_playable(const Game *game, int column)
{
    if (column < 0 || column >= BOARD_WIDTH) /* Vérifie que la colonne existe */
    {
        return FALSE; /* Colonne invalide */
    }

    return game->board[0][column] == EMPTY_CELL; /* La colonne est jouable seulement si le haut est vide */
}


/* CHUTE D'UNE PIÈCE APRÈS INSERTION */

/*
   Cette fonction fait tomber une pièce dans une colonne.

   C'est le principe du Puissance 4 :
   le pion descend jusqu'à rencontrer :
   - le bas du plateau
   - un autre pion
   - un bloc
*/
static Position drop_piece(Game *game, int column, char symbol)
{
    Position position; /* Contient la position finale du pion */
    int row; /* Sert à faire descendre le pion ligne par ligne */

    position.row = -1; /* Valeur invalide au départ, utile si la pièce ne peut pas être placée */
    position.column = column; /* La colonne finale est celle choisie par le joueur */

    if (!is_column_playable(game, column)) /* Si la colonne est pleine ou invalide */
    {
        return position; /* On retourne une position invalide */
    }

    row = 0; /* Le pion commence en haut de la colonne */

    /*
       Tant que la case en dessous existe et qu'elle est vide,
       le pion descend d'une ligne.
    */
    while (row + 1 < BOARD_HEIGHT &&
           game->board[row + 1][column] == EMPTY_CELL)
    {
        row++; /* La pièce descend */
    }

    game->board[row][column] = symbol; /* Place le symbole du joueur à la position finale */
    position.row = row; /* Enregistre la ligne finale du pion */

    return position; /* Retourne la position du pion joué */
}


/* DEMANDE DE LA COLONNE */

/*
   Cette fonction demande au joueur dans quelle colonne il veut jouer.

   Le joueur peut :
   - entrer un nombre entre 1 et 8
   - taper S pour sauvegarder
   - taper Q pour sauvegarder et quitter

   Le joueur voit les colonnes de 1 à 8.
   Le tableau C utilise les indices de 0 à 7.
   Donc on fait column-- après la saisie.
*/
static int ask_playable_column(Game *game)
{
    int column; /* Stocke la colonne saisie par le joueur */
    int action; /* Stocke une action spéciale : sauvegarder ou quitter */

    while (TRUE) /* On répète tant que la colonne n'est pas valide */
    {
        column = read_integer_or_action(
            "Choisissez une colonne pour placer votre piece.",
            1,
            BOARD_WIDTH,
            &action
        ); /* Lit soit une colonne, soit S, soit Q */

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde la partie */
            continue; /* Le joueur reste dans la partie et choisit une colonne */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde avant de quitter */
            return -1; /* -1 indique au reste du programme qu'il faut arrêter le tour */
        }

        column--; /* Convertit la colonne visible par le joueur en indice C */

        if (!is_column_playable(game, column)) /* Vérifie que la colonne peut recevoir un pion */
        {
            printf("Cette colonne est bloquee ou pleine. Choisissez une autre colonne.\n");
            continue; /* Redemande une autre colonne */
        }

        return column; /* Retourne la colonne valide */
    }
}


/* TAILLE DE ROTATION */

/*
   Cette fonction choisit la taille de rotation imposée.

   Le joueur ne choisit pas la taille :
   le jeu impose aléatoirement soit 3x3, soit 5x5.

   rand() % 2 donne seulement deux possibilités :
   - 0
   - 1
*/
static int ask_rotation_size(void)
{
    if (rand() % 2 == 0) /* Une chance sur deux */
    {
        return 3; /* Rotation sur un carré 3x3 */
    }

    return 5; /* Sinon rotation sur un carré 5x5 */
}


/* PIVOT */

/*
   Cette fonction vérifie si le pivot est valide.

   Le pivot est le centre du carré qui va tourner.

   Pour une rotation 3x3 :
   - radius vaut 1
   - il faut 1 case autour du pivot

   Pour une rotation 5x5 :
   - radius vaut 2
   - il faut 2 cases autour du pivot

   Le carré ne doit jamais sortir du plateau.
*/
static int is_valid_pivot(int row, int column, int size)
{
    int radius; /* Distance entre le pivot et le bord du carré */

    radius = size / 2; /* Donne 1 pour 3x3 et 2 pour 5x5 */

    return row - radius >= 0 && /* Le haut du carré reste dans le plateau */
           row + radius < BOARD_HEIGHT && /* Le bas du carré reste dans le plateau */
           column - radius >= 0 && /* La gauche du carré reste dans le plateau */
           column + radius < BOARD_WIDTH; /* La droite du carré reste dans le plateau */
}

/*
   Cette fonction vérifie que la zone de rotation contient le pion joué.

   Règle importante :
   le joueur doit tourner un carré qui contient le pion qu'il vient de poser.

   Le pion n'a pas besoin d'être sur le pivot.
   Il doit seulement être à l'intérieur du carré.
*/
static int zone_contains_position(Position pivot, int size, Position position)
{
    int radius; /* Rayon de la zone autour du pivot */

    radius = size / 2; /* 1 pour 3x3, 2 pour 5x5 */

    return position.row >= pivot.row - radius && /* Le pion n'est pas au-dessus de la zone */
           position.row <= pivot.row + radius && /* Le pion n'est pas en dessous de la zone */
           position.column >= pivot.column - radius && /* Le pion n'est pas à gauche de la zone */
           position.column <= pivot.column + radius; /* Le pion n'est pas à droite de la zone */
}

/*
   Cette fonction demande au joueur de choisir le pivot.

   Le pivot doit respecter deux conditions :
   1. le carré autour du pivot doit rester dans le plateau
   2. ce carré doit contenir le pion qui vient d'être joué
*/
static Position ask_pivot(Game *game, int size, Position piece_position)
{
    Position pivot; /* Stocke la ligne et la colonne du pivot choisi */
    int action; /* Stocke S ou Q si le joueur demande une action spéciale */

    while (TRUE) /* On répète tant que le pivot n'est pas valide */
    {
        pivot.row = read_integer_or_action(
            "Choisissez la ligne du pivot, c'est-a-dire le centre du carre a tourner.",
            1,
            BOARD_HEIGHT,
            &action
        ); /* Demande la ligne du pivot */

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde la partie */
            continue; /* Redemande la ligne du pivot */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde la partie */
            pivot.row = -1; /* Ligne invalide pour signaler l'arrêt */
            pivot.column = -1; /* Colonne invalide pour signaler l'arrêt */
            return pivot; /* Retourne un pivot invalide */
        }

        pivot.column = read_integer_or_action(
            "Choisissez la colonne du pivot, c'est-a-dire le centre du carre a tourner.",
            1,
            BOARD_WIDTH,
            &action
        ); /* Demande la colonne du pivot */

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde la partie */
            continue; /* Redemande le pivot */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde la partie */
            pivot.row = -1; /* Ligne invalide */
            pivot.column = -1; /* Colonne invalide */
            return pivot; /* Retourne un pivot invalide */
        }

        pivot.row--; /* Convertit la ligne utilisateur en indice C */
        pivot.column--; /* Convertit la colonne utilisateur en indice C */

        if (!is_valid_pivot(pivot.row, pivot.column, size)) /* Vérifie que le carré ne sort pas du plateau */
        {
            printf("Pivot invalide : la zone sortirait du plateau.\n");
            continue; /* Redemande un autre pivot */
        }

        if (!zone_contains_position(pivot, size, piece_position)) /* Vérifie que la zone contient le pion joué */
        {
            printf("Pivot invalide : la zone doit contenir la piece inseree.\n");
            continue; /* Redemande un autre pivot */
        }

        return pivot; /* Le pivot est valide */
    }
}


/* SENS DE ROTATION */

/*
   Cette fonction demande au joueur le sens de rotation.

   Le joueur tape :
   - 1 pour gauche
   - 2 pour droite

   Ensuite, on retourne ROTATE_LEFT ou ROTATE_RIGHT
   pour que rotate_zone sache dans quel sens tourner.
*/
static int ask_rotation_direction(Game *game)
{
    int choice; /* Choix du joueur : 1 ou 2 */
    int action; /* Action spéciale : sauvegarder ou quitter */

    while (TRUE) /* On répète tant que la saisie n'est pas correcte */
    {
        choice = read_integer_or_action(
            "Sens de rotation : 1 = gauche, 2 = droite.",
            1,
            2,
            &action
        ); /* Lit le sens de rotation ou une action */

        if (action == ACTION_SAVE) /* Si le joueur tape S */
        {
            save_game(game); /* Sauvegarde la partie */
            continue; /* Redemande le sens */
        }

        if (action == ACTION_QUIT) /* Si le joueur tape Q */
        {
            save_game(game); /* Sauvegarde la partie */
            return 0; /* 0 indique que le tour doit s'arrêter */
        }

        if (choice == 1) /* Si le joueur choisit gauche */
        {
            return ROTATE_LEFT; /* Retourne le sens gauche */
        }

        return ROTATE_RIGHT; /* Sinon, le joueur a choisi droite */
    }
}


/* ROTATION DU CARRÉ */

/*
   Cette fonction tourne une zone carrée du plateau.

   Étapes :
   1. Copier la zone dans un tableau temporaire.
   2. Replacer les cases dans le plateau avec les coordonnées tournées.

   On utilise temporary pour éviter d'écraser les valeurs
   pendant la rotation.
*/
static void rotate_zone(Game *game, Position pivot, int size, int direction)
{
    char temporary[5][5]; /* Tableau temporaire, assez grand pour une rotation 5x5 */
    int radius; /* Distance entre le pivot et le bord du carré */
    int row; /* Ligne locale dans la zone */
    int column; /* Colonne locale dans la zone */
    int source_row; /* Ligne réelle dans le plateau */
    int source_column; /* Colonne réelle dans le plateau */
    int local_row; /* Ligne de l'ancienne case à récupérer */
    int local_column; /* Colonne de l'ancienne case à récupérer */

    radius = size / 2; /* 1 pour 3x3, 2 pour 5x5 */

    /*
       Première double boucle :
       on copie la zone du plateau dans temporary.
    */
    for (row = 0; row < size; row++) /* Parcourt les lignes de la zone */
    {
        for (column = 0; column < size; column++) /* Parcourt les colonnes de la zone */
        {
            source_row = pivot.row - radius + row; /* Convertit la ligne locale en ligne du plateau */
            source_column = pivot.column - radius + column; /* Convertit la colonne locale en colonne du plateau */

            temporary[row][column] = game->board[source_row][source_column]; /* Copie la case avant rotation */
        }
    }

    /*
       Deuxième double boucle :
       on réécrit la zone dans le plateau après rotation.
    */
    for (row = 0; row < size; row++) /* Parcourt les lignes de destination */
    {
        for (column = 0; column < size; column++) /* Parcourt les colonnes de destination */
        {
            source_row = pivot.row - radius + row; /* Ligne réelle où écrire */
            source_column = pivot.column - radius + column; /* Colonne réelle où écrire */

            /*
                Correction du sens de rotation :

                ROTATE_RIGHT = rotation vers la droite, sens horaire.
                ROTATE_LEFT  = rotation vers la gauche, sens antihoraire.

                Ici, on calcule quelle ancienne case de temporary
                doit être placée dans la nouvelle case du plateau.
            */
            if (direction == ROTATE_RIGHT) /* Cas d'une rotation vers la droite */
            {
                local_row = column; /* Ancienne ligne à récupérer */
                local_column = size - 1 - row; /* Ancienne colonne à récupérer */
            }
            else /* Cas d'une rotation vers la gauche */
            {
                local_row = size - 1 - column; /* Ancienne ligne à récupérer */
                local_column = row; /* Ancienne colonne à récupérer */
            }

            game->board[source_row][source_column] = temporary[local_row][local_column]; /* Écrit la case tournée */
        }
    }
}


/* GRAVITÉ SUR TOUT LE PLATEAU */

/*
   Cette fonction applique la gravité sur tout le plateau.

   Après une rotation, certains pions peuvent se retrouver en l'air.
   Comme dans un Puissance 4, les pions doivent tomber vers le bas.

   Les blocs ne bougent pas.
   Ils servent d'obstacles et bloquent la chute des pions.
*/
static void apply_gravity(Game *game)
{
    int column; /* Colonne actuellement traitée */
    int row; /* Ligne actuellement examinée */
    int write_row; /* Ligne où le prochain pion doit tomber */

    for (column = 0; column < BOARD_WIDTH; column++) /* On traite le plateau colonne par colonne */
    {
        write_row = BOARD_HEIGHT - 1; /* Au départ, les pions tombent tout en bas */

        for (row = BOARD_HEIGHT - 1; row >= 0; row--) /* On remonte la colonne du bas vers le haut */
        {
            if (game->board[row][column] == BLOCK_CELL) /* Si on rencontre un bloc */
            {
                write_row = row - 1; /* Les prochains pions tomberont au-dessus du bloc */
            }
            else if (game->board[row][column] != EMPTY_CELL) /* Si on rencontre un pion */
            {
                if (row != write_row) /* Si le pion peut descendre */
                {
                    game->board[write_row][column] = game->board[row][column]; /* Déplace le pion vers le bas */
                    game->board[row][column] = EMPTY_CELL; /* Vide l'ancienne position du pion */
                }

                write_row--; /* Le prochain pion tombera une case plus haut */
            }
        }
    }
}


/* TOUR COMPLET D'UN JOUEUR */

/*
   Cette fonction représente un tour complet.

   Déroulement :
   1. Le jeu impose une rotation 3x3 ou 5x5.
   2. Le joueur choisit une colonne.
   3. Son pion tombe.
   4. Il choisit un pivot valide.
   5. Il choisit gauche ou droite.
   6. La zone tourne.
   7. La gravité s'applique sur tout le plateau.
*/
int play_turn(Game *game)
{
    int column; /* Colonne où le joueur place son pion */
    int size; /* Taille de la rotation : 3 ou 5 */
    int direction; /* Sens de rotation choisi */
    char symbol; /* Symbole du joueur actuel */
    Position piece_position; /* Position du pion après sa chute */
    Position pivot; /* Pivot choisi par le joueur */

    clear_screen(); /* Nettoie l'écran pour un affichage plus lisible */
    display_game(game); /* Affiche l'état actuel du plateau */

    size = ask_rotation_size(); /* Choisit aléatoirement 3x3 ou 5x5 */

    printf("\n================================\n"); /* Début du bloc d'information */
    printf("Tour du joueur %d (%c)\n",
           game->current_player,
           get_player_symbol(game->current_player)); /* Affiche le joueur actuel et son symbole */
    printf("Rotation imposee : %d x %d\n", size, size); /* Affiche la taille de rotation imposée */
    printf("Objectif : aligner %d pions\n", ALIGNMENT_LENGTH); /* Rappelle l'objectif du jeu */
    printf("================================\n"); /* Fin du bloc d'information */

    column = ask_playable_column(game); /* Demande au joueur une colonne valide */

    if (column == -1) /* Si le joueur a choisi de sauvegarder et quitter */
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE; /* Le tour n'est pas terminé normalement */
    }

    symbol = get_player_symbol(game->current_player); /* Récupère le symbole du joueur actuel */

    piece_position = drop_piece(game, column, symbol); /* Place le pion et récupère sa position finale */

    clear_screen(); /* Nettoie l'écran */
    printf("Etape 1 : piece inseree puis tombee.\n"); /* Explique l'étape affichée */
    display_game(game); /* Affiche le plateau après la chute du pion */
    pause_screen(); /* Laisse le temps au joueur de voir le résultat */

    pivot = ask_pivot(game, size, piece_position); /* Demande un pivot valide */

    if (pivot.row == -1 || pivot.column == -1) /* Si le joueur quitte pendant le choix du pivot */
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE; /* Le tour s'arrête */
    }

    clear_screen(); /* Nettoie l'écran */
    printf("Pivot choisi : ligne %d, colonne %d.\n", pivot.row + 1, pivot.column + 1); /* Affiche le pivot en coordonnées utilisateur */
    display_game_with_pivot(game, pivot); /* Affiche le plateau avec P à l'emplacement du pivot */
    pause_screen(); /* Pause pour visualiser le pivot */

    direction = ask_rotation_direction(game); /* Demande si la rotation se fait à gauche ou à droite */

    if (direction == 0) /* Si le joueur quitte pendant le choix du sens */
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE; /* Le tour s'arrête */
    }

    rotate_zone(game, pivot, size, direction); /* Effectue la rotation du carré choisi */

    clear_screen(); /* Nettoie l'écran */
    printf("Etape 2 : zone tournee.\n"); /* Explique que la rotation vient d'être faite */
    display_game_with_pivot(game, pivot); /* Affiche le plateau après rotation avec le pivot */
    pause_screen(); /* Pause pour observer la rotation */

    apply_gravity(game); /* Applique la gravité sur tout le plateau */

    clear_screen(); /* Nettoie l'écran */
    printf("Etape 3 : gravite appliquee sur tout le plateau.\n"); /* Explique que les pièces sont retombées */
    display_game(game); /* Affiche le plateau final du tour */
    pause_screen(); /* Pause avant de passer au joueur suivant */

    return TRUE; /* Le tour s'est terminé normalement */
}


/* CHANGEMENT DE JOUEUR */

/*
   Cette fonction passe au joueur suivant.

   Si on dépasse le nombre total de joueurs,
   on revient au joueur 1.
*/
void change_player(Game *game)
{
    game->current_player++; /* Passe au joueur suivant */

    if (game->current_player > game->player_count) /* Si on dépasse le dernier joueur */
    {
        game->current_player = 1; /* Retour au joueur 1 */
    }

    game->turn_number++; /* Incrémente le numéro du tour */
}


/* PLATEAU PLEIN */

/*
   Cette fonction vérifie si le plateau est plein.

   Si au moins une colonne est encore jouable,
   alors le plateau n'est pas plein.
*/
int is_board_full(const Game *game)
{
    int column; /* Sert à tester chaque colonne */

    for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
    {
        if (is_column_playable(game, column)) /* Si une colonne peut encore recevoir un pion */
        {
            return FALSE; /* Le plateau n'est pas plein */
        }
    }

    return TRUE; /* Aucune colonne n'est jouable */
}


/* VÉRIFICATION DES ALIGNEMENTS */

/*
   Cette fonction vérifie s'il y a un alignement dans une direction.

   delta_row et delta_column indiquent la direction :
   - 0, 1  : horizontal vers la droite
   - 1, 0  : vertical vers le bas
   - 1, 1  : diagonale bas-droite
   - 1, -1 : diagonale bas-gauche
*/
static int check_direction(const Game *game,
                           int row,
                           int column,
                           int delta_row,
                           int delta_column,
                           char symbol)
{
    int count; /* Compte le nombre de pions alignés */
    int current_row; /* Ligne actuellement vérifiée */
    int current_column; /* Colonne actuellement vérifiée */

    count = 0; /* Aucun pion compté au départ */
    current_row = row; /* On commence à la ligne donnée */
    current_column = column; /* On commence à la colonne donnée */

    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol) /* Continue tant qu'on trouve le même symbole */
    {
        count++; /* Ajoute un pion à l'alignement */

        if (count >= ALIGNMENT_LENGTH) /* Si on atteint 5 pions */
        {
            return TRUE; /* Le joueur a gagné */
        }

        current_row += delta_row; /* Avance dans la direction verticale */
        current_column += delta_column; /* Avance dans la direction horizontale */
    }

    return FALSE; /* Aucun alignement gagnant dans cette direction */
}

/*
   Cette fonction vérifie si un joueur a gagné.

   Elle parcourt toutes les cases du plateau.
   À chaque pion du joueur, elle teste les 4 directions possibles.
*/
static int check_player_win(const Game *game, char symbol)
{
    int row; /* Ligne parcourue */
    int column; /* Colonne parcourue */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            if (game->board[row][column] == symbol) /* Si la case appartient au joueur */
            {
                if (check_direction(game, row, column, 0, 1, symbol)) /* Vérifie horizontalement */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, 0, symbol)) /* Vérifie verticalement */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, 1, symbol)) /* Vérifie la diagonale descendante */
                {
                    return TRUE; /* Victoire trouvée */
                }

                if (check_direction(game, row, column, 1, -1, symbol)) /* Vérifie l'autre diagonale */
                {
                    return TRUE; /* Victoire trouvée */
                }
            }
        }
    }

    return FALSE; /* Aucun alignement gagnant trouvé */
}


/* RECHERCHE DES GAGNANTS */

/*
   Cette fonction cherche tous les joueurs gagnants.

   Elle est utile car, après une rotation,
   plusieurs joueurs peuvent théoriquement obtenir un alignement.
*/
int check_winners(const Game *game, int winners[])
{
    int player; /* Numéro du joueur testé */
    int winner_count; /* Nombre de gagnants trouvés */
    char symbol; /* Symbole associé au joueur testé */

    winner_count = 0; /* Aucun gagnant au départ */

    for (player = 1; player <= game->player_count; player++) /* Teste chaque joueur */
    {
        symbol = get_player_symbol(player); /* Récupère X, O ou A */

        if (check_player_win(game, symbol)) /* Vérifie si ce joueur a un alignement */
        {
            winners[winner_count] = player; /* Ajoute le joueur dans le tableau des gagnants */
            winner_count++; /* Augmente le nombre de gagnants */
        }
    }

    return winner_count; /* Retourne le nombre total de gagnants */
}


/* MARQUAGE DES CASES GAGNANTES */

/*
   Cette fonction marque les cases d'un alignement gagnant.

   Elle ne sert pas à décider seulement qui gagne :
   elle sert aussi à l'affichage, pour surligner les cases gagnantes.
*/
static int mark_winning_direction(const Game *game,
                                  int row,
                                  int column,
                                  int delta_row,
                                  int delta_column,
                                  char symbol,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH])
{
    int count; /* Nombre de pions alignés */
    int current_row; /* Ligne actuellement vérifiée */
    int current_column; /* Colonne actuellement vérifiée */
    int i; /* Sert à marquer les cases gagnantes */

    count = 0; /* Aucun pion compté au départ */
    current_row = row; /* Départ de la vérification */
    current_column = column; /* Départ de la vérification */

    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol) /* Continue tant que le symbole est aligné */
    {
        count++; /* Ajoute un pion au compteur */

        if (count >= ALIGNMENT_LENGTH) /* Si on a trouvé 5 pions */
        {
            current_row = row; /* Revient au début de l'alignement */
            current_column = column; /* Revient au début de l'alignement */

            for (i = 0; i < ALIGNMENT_LENGTH; i++) /* Marque exactement les 5 cases gagnantes */
            {
                winning_cells[current_row][current_column] = TRUE; /* Indique que cette case doit être surlignée */
                current_row += delta_row; /* Passe à la case suivante de l'alignement */
                current_column += delta_column; /* Passe à la case suivante de l'alignement */
            }

            return TRUE; /* Un alignement a été marqué */
        }

        current_row += delta_row; /* Continue la recherche dans la direction */
        current_column += delta_column; /* Continue la recherche dans la direction */
    }

    return FALSE; /* Aucun alignement gagnant trouvé dans cette direction */
}

/*
   Cette fonction remplit le tableau winning_cells.

   Au départ, toutes les cases sont mises à FALSE.
   Ensuite, pour chaque joueur, on cherche les alignements.
   Si un alignement existe, les cases correspondantes passent à TRUE.
*/
int get_winning_cells(const Game *game,
                      int winning_cells[BOARD_HEIGHT][BOARD_WIDTH])
{
    int row; /* Ligne parcourue */
    int column; /* Colonne parcourue */
    int player; /* Joueur actuellement testé */
    int found; /* Indique si au moins une victoire a été trouvée */
    char symbol; /* Symbole du joueur testé */

    found = FALSE; /* Aucune case gagnante trouvée au départ */

    for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt toutes les lignes */
    {
        for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt toutes les colonnes */
        {
            winning_cells[row][column] = FALSE; /* Initialise chaque case comme non gagnante */
        }
    }

    for (player = 1; player <= game->player_count; player++) /* Vérifie chaque joueur */
    {
        symbol = get_player_symbol(player); /* Récupère le symbole du joueur */

        for (row = 0; row < BOARD_HEIGHT; row++) /* Parcourt les lignes du plateau */
        {
            for (column = 0; column < BOARD_WIDTH; column++) /* Parcourt les colonnes du plateau */
            {
                if (game->board[row][column] == symbol) /* Cherche les pions du joueur */
                {
                    if (mark_winning_direction(game, row, column, 0, 1, symbol, winning_cells)) /* Marque un alignement horizontal */
                    {
                        found = TRUE; /* Au moins une victoire existe */
                    }

                    if (mark_winning_direction(game, row, column, 1, 0, symbol, winning_cells)) /* Marque un alignement vertical */
                    {
                        found = TRUE; /* Au moins une victoire existe */
                    }

                    if (mark_winning_direction(game, row, column, 1, 1, symbol, winning_cells)) /* Marque une diagonale descendante */
                    {
                        found = TRUE; /* Au moins une victoire existe */
                    }

                    if (mark_winning_direction(game, row, column, 1, -1, symbol, winning_cells)) /* Marque une diagonale montante */
                    {
                        found = TRUE; /* Au moins une victoire existe */
                    }
                }
            }
        }
    }

    return found; /* TRUE si au moins une case gagnante a été trouvée */
}

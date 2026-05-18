#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "display.h"
#include "input.h"
#include "save.h"
#include "constants.h"

/* ============================================================
   PROTOTYPES DES FONCTIONS INTERNES
   ============================================================ */

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
static void apply_gravity(Game *game);
static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol);
static int check_player_win(const Game *game, char symbol);
static int mark_winning_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);

/* ============================================================
   INITIALISATION DU JEU
   ============================================================ */

void initialize_game(Game *game, int player_count)
{
    game->player_count = player_count;
    game->current_player = 1;
    game->turn_number = 1;
    initialize_board(game);
}

static void initialize_board(Game *game)
{
    int row;
    int column;

    for (row = 0; row < BOARD_HEIGHT; row++)
    {
        for (column = 0; column < BOARD_WIDTH; column++)
        {
            game->board[row][column] = EMPTY_CELL;
        }
    }

    game->board[0][0] = BLOCK_CELL;
    game->board[0][BOARD_WIDTH - 1] = BLOCK_CELL;
    game->board[BOARD_HEIGHT - 1][0] = BLOCK_CELL;
    game->board[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] = BLOCK_CELL;
}

/* ============================================================
   VÉRIFICATIONS DE BASE
   ============================================================ */

static int is_inside_board(int row, int column)
{
    return row >= 0 && row < BOARD_HEIGHT && column >= 0 && column < BOARD_WIDTH;
}

static int is_column_playable(const Game *game, int column)
{
    if (column < 0 || column >= BOARD_WIDTH)
    {
        return FALSE;
    }

    return game->board[0][column] == EMPTY_CELL;
}

/* ============================================================
   CHUTE D'UNE PIÈCE
   ============================================================ */

static Position drop_piece(Game *game, int column, char symbol)
{
    Position position;
    int row;

    position.row = -1;
    position.column = column;

    if (!is_column_playable(game, column))
    {
        return position;
    }

    row = 0;

    while (row + 1 < BOARD_HEIGHT && game->board[row + 1][column] == EMPTY_CELL)
    {
        row++;
    }

    game->board[row][column] = symbol;
    position.row = row;

    return position;
}

/* ============================================================
   DEMANDE DE LA COLONNE AU JOUEUR
   ============================================================ */

static int ask_playable_column(Game *game)
{
    int column;
    int action;

    while (TRUE)
    {
        column = read_integer_or_action(
            "Choisissez une colonne pour placer votre piece.",
            1,
            BOARD_WIDTH,
            &action
        );

        if (action == ACTION_SAVE)
        {
            save_game(game);
            continue;
        }

        if (action == ACTION_QUIT)
        {
            save_game(game);
            return -1;
        }

        column--;

        if (!is_column_playable(game, column))
        {
            printf("Cette colonne est bloquee ou pleine. Choisissez une autre colonne.\n");
            continue;
        }

        return column;
    }
}

/* ============================================================
   TAILLE DE ROTATION
   ============================================================ */

static int ask_rotation_size(void)
{
    if (rand() % 2 == 0)
    {
        return 3;
    }

    return 5;
}

/* ============================================================
   VALIDATION DU PIVOT
   ============================================================ */

static int is_valid_pivot(int row, int column, int size)
{
    int radius;

    radius = size / 2;

    return row - radius >= 0 &&
           row + radius < BOARD_HEIGHT &&
           column - radius >= 0 &&
           column + radius < BOARD_WIDTH;
}

static int zone_contains_position(Position pivot, int size, Position position)
{
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

static Position ask_pivot(Game *game, int size, Position piece_position)
{
    Position pivot;
    int action;

    while (TRUE)
    {
        pivot.row = read_integer_or_action(
            "Choisissez la ligne du pivot.",
            1,
            BOARD_HEIGHT,
            &action
        );

        if (action == ACTION_SAVE)
        {
            save_game(game);
            continue;
        }

        if (action == ACTION_QUIT)
        {
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

        if (action == ACTION_SAVE)
        {
            save_game(game);
            continue;
        }

        if (action == ACTION_QUIT)
        {
            save_game(game);
            pivot.row = -1;
            pivot.column = -1;
            return pivot;
        }

        pivot.row--;
        pivot.column--;

        if (!is_valid_pivot(pivot.row, pivot.column, size))
        {
            printf("Pivot invalide : la zone sortirait du plateau.\n");
            continue;
        }

        if (!zone_contains_position(pivot, size, piece_position))
        {
            printf("Pivot invalide : la zone doit contenir la piece inseree.\n");
            continue;
        }

        return pivot;
    }
}

/* ============================================================
   DEMANDE DU SENS DE ROTATION
   ============================================================ */

static int ask_rotation_direction(Game *game)
{
    int choice;
    int action;

    while (TRUE)
    {
        choice = read_integer_or_action(
            "Sens de rotation : 1 = gauche, 2 = droite.",
            1,
            2,
            &action
        );

        if (action == ACTION_SAVE)
        {
            save_game(game);
            continue;
        }

        if (action == ACTION_QUIT)
        {
            save_game(game);
            return 0;
        }

        if (choice == 1)
        {
            return ROTATE_LEFT;
        }

        return ROTATE_RIGHT;
    }
}

/* ============================================================
   ROTATION D'UNE ZONE
   ============================================================ */

static void rotate_zone(Game *game, Position pivot, int size, int direction)
{
    char temporary[5][5];
    int radius;
    int row;
    int column;
    int source_row;
    int source_column;
    int local_row;
    int local_column;

    radius = size / 2;

    for (row = 0; row < size; row++)
    {
        for (column = 0; column < size; column++)
        {
            source_row = pivot.row - radius + row;
            source_column = pivot.column - radius + column;
            temporary[row][column] = game->board[source_row][source_column];
        }
    }

    for (row = 0; row < size; row++)
    {
        for (column = 0; column < size; column++)
        {
            source_row = pivot.row - radius + row;
            source_column = pivot.column - radius + column;

            if (direction == ROTATE_RIGHT)
            {
                local_row = size - 1 - column;
                local_column = row;
            }
            else
            {
                local_row = column;
                local_column = size - 1 - row;
            }

            game->board[source_row][source_column] = temporary[local_row][local_column];
        }
    }
}

/* ============================================================
   GRAVITÉ SUR TOUT LE PLATEAU
   ============================================================ */

static void apply_gravity(Game *game)
{
    int column;
    int row;
    int write_row;

    for (column = 0; column < BOARD_WIDTH; column++)
    {
        write_row = BOARD_HEIGHT - 1;

        for (row = BOARD_HEIGHT - 1; row >= 0; row--)
        {
            if (game->board[row][column] == BLOCK_CELL)
            {
                write_row = row - 1;
            }
            else if (game->board[row][column] != EMPTY_CELL)
            {
                if (row != write_row)
                {
                    game->board[write_row][column] = game->board[row][column];
                    game->board[row][column] = EMPTY_CELL;
                }

                write_row--;
            }
        }
    }
}

/* ============================================================
   TOUR COMPLET D'UN JOUEUR
   ============================================================ */

int play_turn(Game *game)
{
    int column;
    int size;
    int direction;
    char symbol;
    Position piece_position;
    Position pivot;

    clear_screen();
    display_game(game);

    printf("\nTour du joueur %d.\n", game->current_player);

    size = ask_rotation_size();
    printf("Taille de rotation imposee : %d x %d\n", size, size);

    column = ask_playable_column(game);

    if (column == -1)
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    symbol = get_player_symbol(game->current_player);

    piece_position = drop_piece(game, column, symbol);

    clear_screen();
    printf("Etape 1 : piece inseree puis tombee.\n");
    display_game(game);
    pause_screen();

    pivot = ask_pivot(game, size, piece_position);

    if (pivot.row == -1 || pivot.column == -1)
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    clear_screen();
    printf("Pivot choisi : ligne %d, colonne %d.\n", pivot.row + 1, pivot.column + 1);
    display_game_with_pivot(game, pivot);
    pause_screen();

    direction = ask_rotation_direction(game);

    if (direction == 0)
    {
        printf("Partie sauvegardee. Retour au menu.\n");
        return FALSE;
    }

    rotate_zone(game, pivot, size, direction);

    clear_screen();
    printf("Etape 2 : zone tournee.\n");
    display_game_with_pivot(game, pivot);
    pause_screen();

    apply_gravity(game);

    clear_screen();
    printf("Etape 3 : gravite appliquee.\n");
    display_game(game);
    pause_screen();

    return TRUE;
}

/* ============================================================
   CHANGEMENT DE JOUEUR
   ============================================================ */

void change_player(Game *game)
{
    game->current_player++;

    if (game->current_player > game->player_count)
    {
        game->current_player = 1;
    }

    game->turn_number++;
}

/* ============================================================
   PLATEAU PLEIN
   ============================================================ */

int is_board_full(const Game *game)
{
    int column;

    for (column = 0; column < BOARD_WIDTH; column++)
    {
        if (is_column_playable(game, column))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/* ============================================================
   VÉRIFICATION DES ALIGNEMENTS
   ============================================================ */

static int check_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol)
{
    int count;
    int current_row;
    int current_column;

    count = 0;
    current_row = row;
    current_column = column;

    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol)
    {
        count++;

        if (count >= ALIGNMENT_LENGTH)
        {
            return TRUE;
        }

        current_row += delta_row;
        current_column += delta_column;
    }

    return FALSE;
}

static int check_player_win(const Game *game, char symbol)
{
    int row;
    int column;

    for (row = 0; row < BOARD_HEIGHT; row++)
    {
        for (column = 0; column < BOARD_WIDTH; column++)
        {
            if (game->board[row][column] == symbol)
            {
                if (check_direction(game, row, column, 0, 1, symbol))
                {
                    return TRUE;
                }

                if (check_direction(game, row, column, 1, 0, symbol))
                {
                    return TRUE;
                }

                if (check_direction(game, row, column, 1, 1, symbol))
                {
                    return TRUE;
                }

                if (check_direction(game, row, column, 1, -1, symbol))
                {
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

int check_winners(const Game *game, int winners[])
{
    int player;
    int winner_count;
    char symbol;

    winner_count = 0;

    for (player = 1; player <= game->player_count; player++)
    {
        symbol = get_player_symbol(player);

        if (check_player_win(game, symbol))
        {
            winners[winner_count] = player;
            winner_count++;
        }
    }

    return winner_count;
}

/* ============================================================
   MARQUAGE DES CASES GAGNANTES
   ============================================================ */

static int mark_winning_direction(const Game *game, int row, int column, int delta_row, int delta_column, char symbol, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH])
{
    int count;
    int current_row;
    int current_column;
    int i;

    count = 0;
    current_row = row;
    current_column = column;

    while (is_inside_board(current_row, current_column) &&
           game->board[current_row][current_column] == symbol)
    {
        count++;

        if (count >= ALIGNMENT_LENGTH)
        {
            current_row = row;
            current_column = column;

            for (i = 0; i < ALIGNMENT_LENGTH; i++)
            {
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

int get_winning_cells(const Game *game, int winning_cells[BOARD_HEIGHT][BOARD_WIDTH])
{
    int row;
    int column;
    int player;
    int found;
    char symbol;

    found = FALSE;

    for (row = 0; row < BOARD_HEIGHT; row++)
    {
        for (column = 0; column < BOARD_WIDTH; column++)
        {
            winning_cells[row][column] = FALSE;
        }
    }

    for (player = 1; player <= game->player_count; player++)
    {
        symbol = get_player_symbol(player);

        for (row = 0; row < BOARD_HEIGHT; row++)
        {
            for (column = 0; column < BOARD_WIDTH; column++)
            {
                if (game->board[row][column] == symbol)
                {
                    if (mark_winning_direction(game, row, column, 0, 1, symbol, winning_cells))
                    {
                        found = TRUE;
                    }

                    if (mark_winning_direction(game, row, column, 1, 0, symbol, winning_cells))
                    {
                        found = TRUE;
                    }

                    if (mark_winning_direction(game, row, column, 1, 1, symbol, winning_cells))
                    {
                        found = TRUE;
                    }

                    if (mark_winning_direction(game, row, column, 1, -1, symbol, winning_cells))
                    {
                        found = TRUE;
                    }
                }
            }
        }
    }

    return found;
}

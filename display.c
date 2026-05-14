#include <stdio.h>

#include "display.h"
#include "constants.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

#define RED     "\033[91m"
#define BLUE    "\033[96m"
#define YELLOW  "\033[93m"
#define GREEN   "\033[92m"
#define PURPLE  "\033[95m"
#define GREY    "\033[90m"
#define WHITE   "\033[97m"

#define BG_WIN  "\033[42m"

void clear_screen(void) {
    printf("\033[2J");
    printf("\033[H");
}

void pause_screen(void) {
    printf("\nAppuyez sur ENTREE pour continuer...");
    getchar();
}

char get_player_symbol(int player) {
    if (player == 1) {
        return 'X';
    }

    if (player == 2) {
        return 'O';
    }

    return 'A';
}

static void print_title(void) {
    printf(BOLD BLUE "\n");
    printf("   ╔══════════════════════════════╗\n");
    printf("   ║          CY-CONNECT          ║\n");
    printf("   ╚══════════════════════════════╝\n");
    printf(RESET);
}

static void print_cell_content(char cell) {
    if (cell == EMPTY_CELL) {
        printf(DIM " · " RESET);
    } else if (cell == BLOCK_CELL) {
        printf(GREY BOLD " ▓ " RESET);
    } else if (cell == 'X') {
        printf(RED BOLD " ✖ " RESET);
    } else if (cell == 'O') {
        printf(BLUE BOLD " ◉ " RESET);
    } else if (cell == 'A') {
        printf(YELLOW BOLD " ▲ " RESET);
    } else {
        printf(" %c ", cell);
    }
}

static void display_game_advanced(const Game *game,
                                  int has_pivot,
                                  Position pivot,
                                  int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    int row;
    int column;
    int is_winning_cell;

    print_title();

    printf("\n      ");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf(" %d ", column + 1);
    }

    printf("\n    ╔");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("═══");
    }
    printf("╗\n");

    for (row = 0; row < BOARD_HEIGHT; row++) {
        printf(" %d  ║", row + 1);

        for (column = 0; column < BOARD_WIDTH; column++) {
            is_winning_cell = FALSE;

            if (winning_cells != NULL && winning_cells[row][column]) {
                is_winning_cell = TRUE;
            }

            if (has_pivot && row == pivot.row && column == pivot.column) {
                printf(PURPLE BOLD " P " RESET);
            } else if (is_winning_cell) {
                printf(BG_WIN WHITE BOLD);
                print_cell_content(game->board[row][column]);
                printf(RESET);
            } else {
                print_cell_content(game->board[row][column]);
            }
        }

        printf("║\n");
    }

    printf("    ╚");
    for (column = 0; column < BOARD_WIDTH; column++) {
        printf("═══");
    }
    printf("╝\n");

    printf("\n%sJoueur actuel :%s %d (%c)\n",
           BOLD,
           RESET,
           game->current_player,
           get_player_symbol(game->current_player));

    printf("%sTour :%s %d\n", BOLD, RESET, game->turn_number);

    printf("\nCommandes : choisissez une colonne 1-%d | S = sauvegarder | Q = sauvegarder et quitter\n",
           BOARD_WIDTH);
}

void display_game(const Game *game) {
    Position fake_pivot;

    fake_pivot.row = -1;
    fake_pivot.column = -1;

    display_game_advanced(game, FALSE, fake_pivot, NULL);
}

void display_game_with_pivot(const Game *game, Position pivot) {
    display_game_advanced(game, TRUE, pivot, NULL);
}

void display_game_with_winning_cells(const Game *game,
                                     int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]) {
    Position fake_pivot;

    fake_pivot.row = -1;
    fake_pivot.column = -1;

    display_game_advanced(game, FALSE, fake_pivot, winning_cells);
}

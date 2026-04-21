#include <stdio.h>
#include "display.h"

void clear_screen(void) {
    printf("\033[2J\033[H");
}

void display_player_symbol(int player) {
    switch (player) {
        case CELL_EMPTY:   printf("  "); break;
        case CELL_WALL:    printf("%s%s##%s", ANSI_BOLD, ANSI_WHITE, ANSI_RESET); break;
        case CELL_PLAYER1: printf("%s%s><%s", ANSI_BOLD, ANSI_RED, ANSI_RESET); break;
        case CELL_PLAYER2: printf("%s%s()%s", ANSI_BOLD, ANSI_BLUE, ANSI_RESET); break;
        case CELL_PLAYER3: printf("%s%s{}%s", ANSI_BOLD, ANSI_GREEN, ANSI_RESET); break;
        default:           printf("??"); break;
    }
}

void display_column_numbers(void) {
    int col;
    printf("   ");
    for (col = 0; col < BOARD_WIDTH; col++) {
        printf(" %d  ", col + 1);
    }
    printf("\n");
}

void display_board(const GameState *state) {
    int row, col;

    display_column_numbers();

    printf("   +");
    for (col = 0; col < BOARD_WIDTH; col++) {
        printf("---+");
    }
    printf("\n");

    for (row = 0; row < BOARD_HEIGHT; row++) {
        printf(" %d |", row + 1);
        for (col = 0; col < BOARD_WIDTH; col++) {
            display_player_symbol(state->grid[row][col]);
            printf(" |");
        }
        printf("\n");

        printf("   +");
        for (col = 0; col < BOARD_WIDTH; col++) {
            printf("---+");
        }
        printf("\n");
    }
}

void display_board_with_zone(const GameState *state, int pivot_row, int pivot_col, int size) {
    int row, col;
    int half = size / 2;
    int zone_start_row = pivot_row - half;
    int zone_start_col = pivot_col - half;
    int zone_end_row = zone_start_row + size - 1;
    int zone_end_col = zone_start_col + size - 1;

    display_column_numbers();

    printf("   +");
    for (col = 0; col < BOARD_WIDTH; col++) {
        printf("---+");
    }
    printf("\n");

    for (row = 0; row < BOARD_HEIGHT; row++) {
        printf(" %d |", row + 1);
        for (col = 0; col < BOARD_WIDTH; col++) {
            if (row >= zone_start_row && row <= zone_end_row &&
                col >= zone_start_col && col <= zone_end_col) {
                printf("%s", ANSI_YELLOW);
                display_player_symbol(state->grid[row][col]);
                printf("%s", ANSI_RESET);
            } else {
                display_player_symbol(state->grid[row][col]);
            }
            printf(" |");
        }
        printf("\n");

        printf("   +");
        for (col = 0; col < BOARD_WIDTH; col++) {
            printf("---+");
        }
        printf("\n");
    }
}

void display_welcome(void) {
    clear_screen();
    printf("\n");
    printf("%s%s", ANSI_BOLD, ANSI_CYAN);
    printf("  ====================================\n");
    printf("  ||                                ||\n");
    printf("  ||        CY - CONNECT            ||\n");
    printf("  ||     Puissance 5 + Rotation     ||\n");
    printf("  ||                                ||\n");
    printf("  ====================================\n");
    printf("%s\n", ANSI_RESET);
    printf("  Joueurs : ");
    printf("%s%s><(Joueur 1)%s  ", ANSI_BOLD, ANSI_RED, ANSI_RESET);
    printf("%s%s()(Joueur 2)%s  ", ANSI_BOLD, ANSI_BLUE, ANSI_RESET);
    printf("%s%s{}(Joueur 3)%s\n\n", ANSI_BOLD, ANSI_GREEN, ANSI_RESET);
}

void display_menu(void) {
    printf("%s  --- Menu Principal ---%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  1. Nouvelle partie (2 joueurs)\n");
    printf("  2. Nouvelle partie (3 joueurs)\n");
    printf("  3. Charger une partie sauvegardee\n");
    printf("  4. Quitter\n\n");
    printf("  Votre choix : ");
}

void display_turn_info(const GameState *state) {
    printf("\n  %s--- Tour %d ---%s\n", ANSI_BOLD, state->turn_number, ANSI_RESET);
    printf("  Joueur %d (", state->current_player);
    display_player_symbol(state->current_player);
    printf(") doit jouer.\n");
    printf("  Taille de la zone de rotation : %s%d x %d%s\n\n",
           ANSI_BOLD, state->rotation_size, state->rotation_size, ANSI_RESET);
}

void display_result(const GameState *state) {
    printf("\n");
    printf("%s%s", ANSI_BOLD, ANSI_CYAN);
    printf("  ====================================\n");
    printf("  ||       PARTIE TERMINEE !         ||\n");
    printf("  ====================================\n");
    printf("%s\n", ANSI_RESET);

    if (state->winner == -1) {
        printf("  %s%sEGALITE !%s\n\n", ANSI_BOLD, ANSI_YELLOW, ANSI_RESET);
    } else if (state->winner > 0) {
        printf("  Le %sJoueur %d%s (", ANSI_BOLD, state->winner, ANSI_RESET);
        display_player_symbol(state->winner);
        printf(") a gagne !\n\n");
    }
}

void pause_display(const char *message) {
    char buffer[256];
    printf("\n  %s%s%s", ANSI_CYAN, message, ANSI_RESET);
    printf(" (Appuyez sur Entree pour continuer)\n");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    }
}

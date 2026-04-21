#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "display.h"
#include "save.h"

int read_int(int *value) {
    char buffer[256];
    char *endptr;
    long result;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }

    result = strtol(buffer, &endptr, 10);

    if (endptr == buffer || (*endptr != '\n' && *endptr != '\0')) {
        return 0;
    }

    *value = (int)result;
    return 1;
}

int generate_rotation_size(void) {
    return (rand() % 2 == 0) ? ROTATION_SIZE_SMALL : ROTATION_SIZE_BIG;
}

int ask_column(GameState *state) {
    int col;

    for (;;) {
        printf("  Choisissez une colonne (1-%d) : ", BOARD_WIDTH);

        if (!read_int(&col)) {
            printf("  %sEntree invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        col--;

        if (col < 0 || col >= BOARD_WIDTH) {
            printf("  %sColonne hors limites !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        if (!is_column_playable(state, col)) {
            printf("  %sCette colonne est pleine ou bloquee !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        return col;
    }
}

void ask_pivot(GameState *state, int *pivot_row, int *pivot_col,
               int piece_row, int piece_col) {
    int row, col;

    for (;;) {
        printf("  Choisissez la ligne du pivot (1-%d) : ", BOARD_HEIGHT);
        if (!read_int(&row)) {
            printf("  %sEntree invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }
        row--;

        printf("  Choisissez la colonne du pivot (1-%d) : ", BOARD_WIDTH);
        if (!read_int(&col)) {
            printf("  %sEntree invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }
        col--;

        if (!is_valid_pivot(state, row, col, state->rotation_size, piece_row, piece_col)) {
            printf("  %sPivot invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        *pivot_row = row;
        *pivot_col = col;
        return;
    }
}

int ask_rotation_direction(void) {
    int choice;

    for (;;) {
        printf("  Sens de rotation :\n");
        printf("    1. Sens horaire\n");
        printf("    2. Sens anti-horaire\n");
        printf("  Votre choix : ");

        if (!read_int(&choice)) {
            printf("  %sEntree invalide !%s\n", ANSI_RED, ANSI_RESET);
            continue;
        }

        if (choice == ROTATE_CLOCKWISE || choice == ROTATE_COUNTERCLOCKWISE) {
            return choice;
        }

        printf("  %sChoix invalide !%s\n", ANSI_RED, ANSI_RESET);
    }
}

void next_player(GameState *state) {
    state->current_player++;
    if (state->current_player > state->num_players) {
        state->current_player = 1;
    }
    state->turn_number++;
}

int play_turn(GameState *state) {
    int col, piece_row;
    int pivot_row, pivot_col;
    int direction;
    int save_choice;

    state->rotation_size = generate_rotation_size();

    clear_screen();
    display_board(state);
    display_turn_info(state);

    printf("  Voulez-vous sauvegarder ? (1 = Oui, 0 = Non) : ");
    if (read_int(&save_choice) && save_choice == 1) {
        if (save_game(state, SAVE_FILE)) {
            printf("  %sPartie sauvegardee avec succes !%s\n", ANSI_GREEN, ANSI_RESET);
        } else {
            printf("  %sErreur lors de la sauvegarde.%s\n", ANSI_RED, ANSI_RESET);
        }
        pause_display("Sauvegarde effectuee.");
        clear_screen();
        display_board(state);
        display_turn_info(state);
    }

    col = ask_column(state);

    piece_row = drop_piece(state, col, state->current_player);
    if (piece_row == -1) {
        printf("  %sErreur lors de l'insertion de la piece !%s\n", ANSI_RED, ANSI_RESET);
        return 0;
    }

    clear_screen();
    printf("  %s>>> Piece inseree dans la colonne %d%s\n", ANSI_CYAN, col + 1, ANSI_RESET);
    display_board(state);
    pause_display("Piece placee. Choisissez maintenant le pivot de rotation.");

    clear_screen();
    display_board(state);
    printf("\n  Taille de la zone de rotation : %s%d x %d%s\n",
           ANSI_BOLD, state->rotation_size, state->rotation_size, ANSI_RESET);
    printf("  La zone doit contenir votre piece en (%d, %d).\n\n",
           piece_row + 1, col + 1);

    ask_pivot(state, &pivot_row, &pivot_col, piece_row, col);

    clear_screen();
    printf("  %s>>> Zone de rotation selectionnee%s\n", ANSI_CYAN, ANSI_RESET);
    display_board_with_zone(state, pivot_row, pivot_col, state->rotation_size);

    direction = ask_rotation_direction();

    rotate_zone(state, pivot_row, pivot_col, state->rotation_size, direction);

    clear_screen();
    printf("  %s>>> Apres rotation%s\n", ANSI_CYAN, ANSI_RESET);
    display_board(state);
    pause_display("Rotation effectuee. Application de la gravite...");

    apply_gravity(state);

    clear_screen();
    printf("  %s>>> Apres application de la gravite%s\n", ANSI_CYAN, ANSI_RESET);
    display_board(state);

    state->winner = check_winner(state);
    if (state->winner != 0) {
        state->game_over = 1;
    }

    if (!state->game_over) {
        pause_display("Fin du tour.");
    }

    return 0;
}

void run_game(GameState *state) {
    int is_board_full;
    int row, col;

    while (!state->game_over) {
        play_turn(state);

        if (!state->game_over) {
            is_board_full = 1;
            for (row = 0; row < BOARD_HEIGHT && is_board_full; row++) {
                for (col = 0; col < BOARD_WIDTH && is_board_full; col++) {
                    if (state->grid[row][col] == CELL_EMPTY) {
                        is_board_full = 0;
                    }
                }
            }

            if (is_board_full) {
                state->game_over = 1;
                state->winner = -1;
            } else {
                next_player(state);
            }
        }
    }

    display_result(state);
    display_board(state);
}

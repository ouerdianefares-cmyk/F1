#include "board.h"

void init_board(GameState *state, int num_players) {
    int row, col;

    state->num_players = num_players;
    state->current_player = 1;
    state->turn_number = 1;
    state->rotation_size = 0;
    state->game_over = 0;
    state->winner = 0;

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (col = 0; col < BOARD_WIDTH; col++) {
            state->grid[row][col] = CELL_EMPTY;
        }
    }

    state->grid[0][0] = CELL_WALL;
    state->grid[0][BOARD_WIDTH - 1] = CELL_WALL;
    state->grid[BOARD_HEIGHT - 1][0] = CELL_WALL;
    state->grid[BOARD_HEIGHT - 1][BOARD_WIDTH - 1] = CELL_WALL;
}

void apply_gravity(GameState *state) {
    int col, row, dest_row;

    for (col = 0; col < BOARD_WIDTH; col++) {
        dest_row = BOARD_HEIGHT - 1;

        for (row = BOARD_HEIGHT - 1; row >= 0; row--) {
            if (state->grid[row][col] == CELL_WALL) {
                dest_row = row - 1;
            } else if (state->grid[row][col] != CELL_EMPTY) {
                if (row != dest_row) {
                    state->grid[dest_row][col] = state->grid[row][col];
                    state->grid[row][col] = CELL_EMPTY;
                }
                dest_row--;
            }
        }
    }
}

void apply_gravity_zone(GameState *state, int start_row, int start_col, int size) {
    (void)start_row;
    (void)start_col;
    (void)size;
    apply_gravity(state);
}

int is_column_playable(GameState *state, int col) {
    if (col < 0 || col >= BOARD_WIDTH) {
        return 0;
    }
    return state->grid[0][col] == CELL_EMPTY;
}

int drop_piece(GameState *state, int col, int player) {
    int row;

    if (!is_column_playable(state, col)) {
        return -1;
    }

    state->grid[0][col] = player;
    apply_gravity(state);

    for (row = BOARD_HEIGHT - 1; row >= 0; row--) {
        if (state->grid[row][col] == player) {
            return row;
        }
    }

    return -1;
}

void rotate_zone(GameState *state, int pivot_row, int pivot_col, int size, int direction) {
    int half_size = size / 2;
    int start_row = pivot_row - half_size;
    int start_col = pivot_col - half_size;
    int temp[ROTATION_SIZE_BIG][ROTATION_SIZE_BIG];
    int i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (direction == ROTATE_CLOCKWISE) {
                temp[j][size - 1 - i] = state->grid[start_row + i][start_col + j];
            } else {
                temp[size - 1 - j][i] = state->grid[start_row + i][start_col + j];
            }
        }
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            state->grid[start_row + i][start_col + j] = temp[i][j];
        }
    }
}

int is_valid_pivot(GameState *state, int pivot_row, int pivot_col, int size,
                   int piece_row, int piece_col) {
    int half_size = size / 2;
    int start_row = pivot_row - half_size;
    int start_col = pivot_col - half_size;
    int end_row = start_row + size - 1;
    int end_col = start_col + size - 1;

    (void)state;

    if (start_row < 0 || start_col < 0 ||
        end_row >= BOARD_HEIGHT || end_col >= BOARD_WIDTH) {
        return 0;
    }

    if (piece_row < start_row || piece_row > end_row ||
        piece_col < start_col || piece_col > end_col) {
        return 0;
    }

    return 1;
}

int check_winner(GameState *state) {
    int row, col, d, i;
    int winners[MAX_PLAYERS + 1] = {0};
    int num_winners = 0;

    int dir_row[] = {0, 1, 1, 1};
    int dir_col[] = {1, 0, 1, -1};

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (col = 0; col < BOARD_WIDTH; col++) {
            int cell = state->grid[row][col];
            if (cell <= 0) {
                continue;
            }

            for (d = 0; d < 4; d++) {
                int count = 1;
                int k;

                for (k = 1; k < WIN_COUNT; k++) {
                    int r = row + k * dir_row[d];
                    int c = col + k * dir_col[d];

                    if (r < 0 || r >= BOARD_HEIGHT || c < 0 || c >= BOARD_WIDTH) {
                        break;
                    }

                    if (state->grid[r][c] == cell) {
                        count++;
                    } else {
                        break;
                    }
                }

                if (count >= WIN_COUNT) {
                    winners[cell] = 1;
                }
            }
        }
    }

    for (i = 1; i <= state->num_players; i++) {
        if (winners[i]) {
            num_winners++;
        }
    }

    if (num_winners == 0) {
        return 0;
    }
    if (num_winners >= 2) {
        return -1;
    }

    for (i = 1; i <= state->num_players; i++) {
        if (winners[i]) {
            return i;
        }
    }

    return 0;
}

void copy_state(GameState *dest, const GameState *src) {
    int row, col;

    dest->num_players = src->num_players;
    dest->current_player = src->current_player;
    dest->turn_number = src->turn_number;
    dest->rotation_size = src->rotation_size;
    dest->game_over = src->game_over;
    dest->winner = src->winner;

    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (col = 0; col < BOARD_WIDTH; col++) {
            dest->grid[row][col] = src->grid[row][col];
        }
    }
}

#ifndef GAME_H
#define GAME_H

#include "board.h"

void run_game(GameState *state);
int play_turn(GameState *state);
int ask_column(GameState *state);
void ask_pivot(GameState *state, int *pivot_row, int *pivot_col, int piece_row, int piece_col);
int ask_rotation_direction(void);
int generate_rotation_size(void);
int read_int(int *value);
void next_player(GameState *state);

#endif

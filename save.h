#ifndef SAVE_H
#define SAVE_H

#include "board.h"

int save_game(const GameState *state, const char *filename);
int load_game(GameState *state, const char *filename);
int save_exists(const char *filename);

#endif

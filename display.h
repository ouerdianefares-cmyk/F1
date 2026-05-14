#ifndef DISPLAY_H
#define DISPLAY_H

#include "type.h"

void clear_screen(void);
void pause_screen(void);
char get_player_symbol(int player);

void display_game(const Game *game);

/* Affiche le plateau avec un pivot marqué par P */
void display_game_with_pivot(const Game *game, Position pivot);

/* Affiche le plateau avec les cases gagnantes surlignées */
void display_game_with_winning_cells(const Game *game,
                                     int winning_cells[BOARD_HEIGHT][BOARD_WIDTH]);

#endif /* DISPLAY_H */

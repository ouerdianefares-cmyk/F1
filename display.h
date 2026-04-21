#ifndef DISPLAY_H
#define DISPLAY_H

#include "board.h"

#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_BOLD    "\033[1m"

void clear_screen(void);
void display_board(const GameState *state);
void display_board_with_zone(const GameState *state, int pivot_row, int pivot_col, int size);
void display_column_numbers(void);
void display_welcome(void);
void display_menu(void);
void display_player_symbol(int player);
void display_turn_info(const GameState *state);
void display_result(const GameState *state);
void pause_display(const char *message);

#endif

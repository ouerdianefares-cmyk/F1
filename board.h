#ifndef BOARD_H
#define BOARD_H

#define BOARD_WIDTH  8
#define BOARD_HEIGHT 6
#define WIN_COUNT 5
#define MAX_PLAYERS 3

#define CELL_EMPTY       0
#define CELL_WALL       -1
#define CELL_PLAYER1     1
#define CELL_PLAYER2     2
#define CELL_PLAYER3     3

#define ROTATION_SIZE_SMALL 3
#define ROTATION_SIZE_BIG   5

#define ROTATE_CLOCKWISE        1
#define ROTATE_COUNTERCLOCKWISE 2

#define SAVE_FILE "cy_connect_save.dat"
#define MAX_SAVES 5

typedef struct {
    int grid[BOARD_HEIGHT][BOARD_WIDTH];
    int num_players;
    int current_player;
    int turn_number;
    int rotation_size;
    int game_over;
    int winner;
} GameState;

void init_board(GameState *state, int num_players);
void apply_gravity(GameState *state);
void apply_gravity_zone(GameState *state, int start_row, int start_col, int size);
int is_column_playable(GameState *state, int col);
int drop_piece(GameState *state, int col, int player);
void rotate_zone(GameState *state, int pivot_row, int pivot_col, int size, int direction);
int is_valid_pivot(GameState *state, int pivot_row, int pivot_col, int size, int piece_row, int piece_col);
int check_winner(GameState *state);
void copy_state(GameState *dest, const GameState *src);

#endif

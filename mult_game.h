#ifndef MULT_GAME_H
#define MULT_GAME_H

#include <stdio.h>

// Game constants - simulating hardware constraints
#define GRID1_ROWS 4
#define GRID1_COLS 9
#define GRID2_SIZE 9
#define MAX_MOVES 100

// Player types - using bitwise flags for efficiency
#define EMPTY 0
#define COMPUTER 1
#define PLAYER 2

// Game state structure - simulating CPU registers and memory
typedef struct {
    int computer_arrow;    // Position of computer arrow in Grid 2
    int player_arrow;      // Position of player arrow in Grid 2
    int computer_last_value;  // Track computer's last chosen value
    int player_last_value;    // Track player's last chosen value
    int current_player;    // Current player turn
    int game_over;         // Game over flag
    int winner;            // Winner of the game
    int computer_score;    // Computer's marked positions
    int player_score;      // Player's marked positions
    int move_count;        // Total moves made
} GameState;

// Board manager structure - simulating memory organization
typedef struct {
    int grid1[GRID1_ROWS][GRID1_COLS];  // Main game board values
    int marked[GRID1_ROWS][GRID1_COLS]; // Marked positions
    int grid2[GRID2_SIZE];              // Multiplier values (1-9)
} BoardManager;

// AI strategy structure - simulating decision logic
typedef struct {
    int blocking_priority;   // Priority for blocking player
    int winning_priority;    // Priority for winning moves
    int center_bias;         // Bias towards center positions
} AIStrategy;

// Function prototypes - simulating instruction set
void init_game(void);
void display_board(void);
int find_position_in_grid1(int value, int *row, int *col);
void mark_position(int value, int player);
int check_win_condition(int player);
int evaluate_move(int computer_pos, int player_pos);
void computer_move(void);
void handle_player_input(int ch);
void save_game_state(void);
void load_game_state(void);
void init_display(void);

// Inline functions for bitwise operations - simulating ALU operations
static inline int is_position_marked(int row, int col, int player) {
    extern BoardManager g_board;
    return (g_board.marked[row][col] & player) != 0;
}

static inline void set_position_flag(int row, int col, int flag) {
    extern BoardManager g_board;
    g_board.marked[row][col] |= flag;
}

static inline void clear_position_flag(int row, int col, int flag) {
    extern BoardManager g_board;
    g_board.marked[row][col] &= ~flag;
}

#endif // MULT_GAME_H
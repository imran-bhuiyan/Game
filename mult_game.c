#define _POSIX_C_SOURCE 199309L
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "mult_game.h"

// Global game state - simulating CPU registers
GameState g_state;
BoardManager g_board;
AIStrategy g_ai;

// Initialize the game state
void init_game() {
    // Initialize Grid 1 (6x9 multiplication results grid)
    int grid1_values[GRID1_ROWS][GRID1_COLS] = {
        {1, 2, 3, 4, 5, 6, 7, 8, 9},
        {10, 12, 14, 15, 16, 18, 20, 21, 24},
        {25, 27, 28, 30, 32, 35, 36, 40, 42},
        {45, 48, 49, 54, 56, 63, 64, 72, 81}
    };
    
    // Copy values to board
    for (int i = 0; i < GRID1_ROWS; i++) {
        for (int j = 0; j < GRID1_COLS; j++) {
            g_board.grid1[i][j] = grid1_values[i][j];
            g_board.marked[i][j] = EMPTY;
        }
    }
    
    // Initialize Grid 2 (1-9 multiplier grid)
    for (int i = 0; i < GRID2_SIZE; i++) {
        g_board.grid2[i] = i + 1;
    }
    
    // Initialize game state
    g_state.computer_arrow = -1;  // Outside grid
    g_state.player_arrow = -1;    // Outside grid
    g_state.current_player = COMPUTER;
    g_state.game_over = 0;
    g_state.winner = EMPTY;
    g_state.computer_score = 0;
    g_state.player_score = 0;
    g_state.move_count = 0;
    
    // Initialize AI strategy
    g_ai.blocking_priority = 3;
    g_ai.winning_priority = 5;
    g_ai.center_bias = 2;
}

// Display the game board using ncurses
void display_board() {
    clear();
    
    // Title
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 20, "MULTIPLICATION GAME");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // Grid 2 (Multiplier grid) - Top arrow (Computer)
    //mvprintw(2, 10, "Computer Arrow:");
    for (int i = 0; i < GRID2_SIZE; i++) {
        if (g_state.computer_arrow == i) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(3, 15 + i * 4, " v ");
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            mvprintw(3, 15 + i * 4, "   ");
        }
    }
    
    // Grid 2 numbers
    mvprintw(4, 10, "Grid 2: ");
    for (int i = 0; i < GRID2_SIZE; i++) {
        mvprintw(4, 15 + i * 4, " %d ", g_board.grid2[i]);
    }
    
    // Grid 2 - Bottom arrow (Player)
    //mvprintw(5, 10, "Player Arrow:");
    for (int i = 0; i < GRID2_SIZE; i++) {
        if (g_state.player_arrow == i) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(6, 15 + i * 4, " ^ ");
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            mvprintw(6, 15 + i * 4, "   ");
        }
    }
    
    // Grid 1 (Main game board)
    mvprintw(8, 10, "Main Board (Grid 1):");
    mvprintw(9, 10, "+");
    for (int j = 0; j < GRID1_COLS; j++) {
        printw("---+");
    }
    
    for (int i = 0; i < GRID1_ROWS; i++) {
        mvprintw(10 + i * 2, 10, "|");
        for (int j = 0; j < GRID1_COLS; j++) {
            char marker = ' ';
            int color_pair = 4;
            
            if (g_board.marked[i][j] == COMPUTER) {
                marker = 'C';
                color_pair = 2;
            } else if (g_board.marked[i][j] == PLAYER) {
                marker = 'P';
                color_pair = 3;
            }
            
            if (marker != ' ') {
                attron(COLOR_PAIR(color_pair) | A_BOLD);
            }
            
            printw("%2d%c|", g_board.grid1[i][j], marker);
            
            if (marker != ' ') {
                attroff(COLOR_PAIR(color_pair) | A_BOLD);
            }
        }
        
        mvprintw(11 + i * 2, 10, "+");
        for (int j = 0; j < GRID1_COLS; j++) {
            printw("---+");
        }
    }
    
    // Game status
    mvprintw(18, 10, "Scores - Computer: %d, Player: %d", 
             g_state.computer_score, g_state.player_score);
    mvprintw(19, 10, "Move Count: %d", g_state.move_count);
    
    // Previous choices
    mvprintw(20, 10, "Previous Choices - Computer: %d, Player: %d", 
             g_state.computer_arrow >= 0 ? g_board.grid2[g_state.computer_arrow] : 0,
             g_state.player_arrow >= 0 ? g_board.grid2[g_state.player_arrow] : 0);
    
    if (g_state.current_player == COMPUTER) {
        attron(COLOR_PAIR(2));
        mvprintw(22, 10, "Computer's turn...");
        attroff(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(3));
        mvprintw(22, 10, "Your turn!");
        mvprintw(23, 10, "First choose which arrow to move (c = red/computer, p = blue/player)");
        mvprintw(24, 10, "Then select a value for that arrow (1-9)");
        attroff(COLOR_PAIR(3));
    }
    
    // Always show controls at the bottom
    mvprintw(26, 10, "Press 'q' to quit, 'r' to restart, 's' to save");

    if (g_state.game_over) {
        attron(COLOR_PAIR(1) | A_BOLD);
        if (g_state.winner == COMPUTER) {
            mvprintw(23, 10, "GAME OVER - Computer Wins!");
        } else if (g_state.winner == PLAYER) {
            mvprintw(23, 10, "GAME OVER - You Win!");
        } else {
            mvprintw(23, 10, "GAME OVER - It's a Draw!");
        }
        attroff(COLOR_PAIR(1) | A_BOLD);
    }
    
    refresh();
}

// Find position of value in Grid 1
int find_position_in_grid1(int value, int *row, int *col) {
    for (int i = 0; i < GRID1_ROWS; i++) {
        for (int j = 0; j < GRID1_COLS; j++) {
            if (g_board.grid1[i][j] == value) {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    return 0;
}

// Mark a position in Grid 1
void mark_position(int value, int player) {
    int row, col;
    if (find_position_in_grid1(value, &row, &col)) {
        if (g_board.marked[row][col] == EMPTY) {
            g_board.marked[row][col] = player;
            if (player == COMPUTER) {
                g_state.computer_score++;
            } else {
                g_state.player_score++;
            }
        }
    }
}

// Check for four in a row
int check_win_condition(int player) {
    // Check horizontal only
    for (int i = 0; i < GRID1_ROWS; i++) {
        int count = 0;
        for (int j = 0; j < GRID1_COLS; j++) {
            if (g_board.marked[i][j] == player) {
                count++;
                if (count >= 4) return 1;
            } else {
                count = 0;
            }
        }
    }
    return 0;
}

// AI strategy evaluation
int evaluate_move(int computer_pos, int player_pos) {
    int product = g_board.grid2[computer_pos] * g_board.grid2[player_pos];
    int row, col;
    
    if (!find_position_in_grid1(product, &row, &col)) {
        return -1000;  // Invalid move
    }
    
    if (g_board.marked[row][col] != EMPTY) {
        return -500;   // Already taken
    }
    
    int score = 0;
    
    // Simulate marking this position for computer
    g_board.marked[row][col] = COMPUTER;
    
    // Check if this creates a win
    if (check_win_condition(COMPUTER)) {
        score += g_ai.winning_priority * 1000;
    }
    
    // Check if this blocks player win
    g_board.marked[row][col] = PLAYER;
    if (check_win_condition(PLAYER)) {
        score += g_ai.blocking_priority * 800;
    }
    
    // Reset position
    g_board.marked[row][col] = EMPTY;
    
    // Center bias
    int center_distance = abs(row - GRID1_ROWS/2) + abs(col - GRID1_COLS/2);
    score += g_ai.center_bias * (10 - center_distance);
    
    return score;
}

// Computer AI move
void computer_move() {
    int best_score = -10000;
    int best_comp_pos = g_state.computer_arrow;
    int best_player_pos = g_state.player_arrow;
    int best_move_type = 0; // 0 = move computer arrow, 1 = move player arrow
    
    // Try moving computer arrow
    for (int comp_pos = 0; comp_pos < GRID2_SIZE; comp_pos++) {
        if (g_state.player_arrow >= 0) {
            int score = evaluate_move(comp_pos, g_state.player_arrow);
            if (score > best_score) {
                best_score = score;
                best_comp_pos = comp_pos;
                best_player_pos = g_state.player_arrow;
                best_move_type = 0;
            }
        }
    }
    
    // Try moving player arrow
    for (int player_pos = 0; player_pos < GRID2_SIZE; player_pos++) {
        if (g_state.computer_arrow >= 0) {
            int score = evaluate_move(g_state.computer_arrow, player_pos);
            if (score > best_score) {
                best_score = score;
                best_comp_pos = g_state.computer_arrow;
                best_player_pos = player_pos;
                best_move_type = 1;
            }
        }
    }
    
    // If no arrows are set, set computer arrow
    if (g_state.computer_arrow < 0 && g_state.player_arrow < 0) {
        g_state.computer_arrow = rand() % GRID2_SIZE;
        g_state.current_player = PLAYER;
        return;
    }
    
    // Apply best move
    if (best_move_type == 0) {
        g_state.computer_arrow = best_comp_pos;
    } else {
        g_state.player_arrow = best_player_pos;
    }
    
    // Calculate and mark result
    if (g_state.computer_arrow >= 0 && g_state.player_arrow >= 0) {
        int product = g_board.grid2[g_state.computer_arrow] * 
                     g_board.grid2[g_state.player_arrow];
        mark_position(product, COMPUTER);
        g_state.move_count++;
        
        // Check win condition
        if (check_win_condition(COMPUTER)) {
            g_state.game_over = 1;
            g_state.winner = COMPUTER;
        }
    }
    
    g_state.current_player = PLAYER;
}

// Player move handling
void handle_player_input(int ch) {
    static int selected_pos = 0;
    static int choosing_arrow = 1; // 1: choose which arrow to change, 0: choose value for that arrow
    static int arrow_to_change = -1; // 0: computer, 1: player

    if (g_state.game_over) {
        switch (ch) {
            case 'q':
            case 'Q':
                exit(0);
                break;
            case 'r':
            case 'R':
                init_game();
                break;
            case 's':
            case 'S':
                save_game_state();
                mvprintw(24, 10, "Game saved!");
                break;
        }
        return;
    }

    if (choosing_arrow) {
        if (ch == 'c' || ch == 'C') {
            arrow_to_change = 0;
            choosing_arrow = 0;
            mvprintw(21, 10, "Select new value for computer's (red) arrow (1-9)");
        } else if (ch == 'p' || ch == 'P') {
            arrow_to_change = 1;
            choosing_arrow = 0;
            mvprintw(21, 10, "Select new value for your (blue) arrow (1-9)");
        } else {
            mvprintw(21, 10, "Which arrow do you want to change? (c = red/computer, p = blue/player)");
        }
    } else {
        switch (ch) {
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                selected_pos = ch - '1';
                if (arrow_to_change == 0) {
                    g_state.computer_arrow = selected_pos;
                } else if (arrow_to_change == 1) {
                    g_state.player_arrow = selected_pos;
                }
                // After changing one arrow, process the move
                if (g_state.computer_arrow >= 0 && g_state.player_arrow >= 0) {
                    int product = g_board.grid2[g_state.computer_arrow] * g_board.grid2[g_state.player_arrow];
                    mark_position(product, PLAYER);
                    g_state.move_count++;
                    // Check win condition
                    if (check_win_condition(PLAYER)) {
                        g_state.game_over = 1;
                        g_state.winner = PLAYER;
                    } else {
                        g_state.current_player = COMPUTER;
                    }
                }
                choosing_arrow = 1;
                arrow_to_change = -1;
                break;
            case 'q': case 'Q':
                exit(0);
                break;
            case 's': case 'S':
                save_game_state();
                mvprintw(24, 10, "Game saved!");
                break;
            case 'l': case 'L':
                load_game_state();
                mvprintw(24, 10, "Game loaded!");
                break;
            default:
                mvprintw(21, 10, "Select new value for %s arrow (1-9)",
                    arrow_to_change == 0 ? "computer's" : "your");
                break;
        }
    }

    // Show current selection or prompt
    if (choosing_arrow) {
        mvprintw(21, 10, "Which arrow do you want to change? (c = red/computer, p = blue/player)");
    } else {
        mvprintw(21, 10, "Select new value for %s arrow (1-9)",
            arrow_to_change == 0 ? "computer's" : "your");
        mvprintw(22, 10, "Selected position: %d (Value: %d)", selected_pos + 1, g_board.grid2[selected_pos]);
    }
}

// Save game state to file
void save_game_state() {
    FILE *file = fopen("game_save.dat", "wb");
    if (file) {
        fwrite(&g_state, sizeof(GameState), 1, file);
        fwrite(&g_board, sizeof(BoardManager), 1, file);
        fwrite(&g_ai, sizeof(AIStrategy), 1, file);
        fclose(file);
    }
}

// Load game state from file
void load_game_state() {
    FILE *file = fopen("game_save.dat", "rb");
    if (file) {
        fread(&g_state, sizeof(GameState), 1, file);
        fread(&g_board, sizeof(BoardManager), 1, file);
        fread(&g_ai, sizeof(AIStrategy), 1, file);
        fclose(file);
    }
}

// Initialize ncurses
void init_display() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Title
        init_pair(2, COLOR_RED, COLOR_BLACK);     // Computer
        init_pair(3, COLOR_BLUE, COLOR_BLACK);    // Player
        init_pair(4, COLOR_WHITE, COLOR_BLACK);   // Default
    }
}

// Main game loop
int main() {
    srand((unsigned int)time(NULL));
    
    init_display();
    init_game();
    
    int ch;
    while (1) {
        display_board();
        
        if (g_state.current_player == COMPUTER && !g_state.game_over) {
            struct timespec ts = {1, 0};  // 1 second, 0 nanoseconds
            nanosleep(&ts, NULL);  // 1 second delay for computer move
            computer_move();
        } else {
            ch = getch();
            handle_player_input(ch);
        }
        
        // Check for draw condition
        if (!g_state.game_over && g_state.computer_score + g_state.player_score >= 20) {
            g_state.game_over = 1;
            g_state.winner = EMPTY;  // Draw
        }
    }
    
    endwin();
    return 0;
}
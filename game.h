#ifndef CHECKERS_V2_GAME_H
#define CHECKERS_V2_GAME_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vector.h"

#define ROW_SIZE 8
#define COL_SIZE 8

typedef enum {white_king = 2, white = 1, no_piece = 0, black = -1, black_king = -2} Piece;
typedef enum {human, computer} Player;
typedef enum {up_left, up_right, jump_up_left, jump_up_right,
              down_left, down_right, jump_down_left, jump_down_right} Direction;
typedef enum {RUNNING, DRAW, COMPUTER_WON, HUMAN_WON, QUIT} Status;

typedef struct Position {
    int8_t row;
    int8_t col;
} Position;

typedef struct Move {
    Position from;
    Position dest;
    Direction direction;
} Move;

typedef struct Game {
    int8_t board[8][8];
    Player current_player;
    Status status;
} Game;

/*
 * Initializes game state, sets up board
 */
void init_game(Game* game);

/*
 * Returns vector that contains all available moves from current position
 * Gives ownership to caller
 */
vector* all_moves_for_piece(const Game* game, const Position* pos);

/*
 * fills vector with all possible moves for given player
 */
void all_moves(const Game* game, Player player, vector* moves);

/*
 * Returns true if move is valid
 */
bool move_is_valid(const Game* game, const Move* move);

/*
 * Returns true if vector contains jump move
 */
bool has_jump_move(const vector* moves);

/*
 * Moves piece on the board
 * if move is invalid function returns and nothing happens
 * generally we check if move is valid before calling this function
 */
void move_piece(Game* game, const Move* move);

/*
 * Checks if move is jump (capturing move)
 */
bool move_is_jump(const Move* move);

/*
 * Checks if player chooses piece of another player or cell that contains no piece at all
 */
bool player_chooses_wrong_piece(const Game* game, const Position* pos);

/*
 * Switches current player to another player
 */
void switch_player(Game* game);

uint8_t player_pieces_count(const Game* game, Player player);

/*
 * Sets move direction according to from and dest fields
 * Does not check for move validity. if move is invalid destination field is unchanged
 */
void set_move_direction(Move* move);

/*
 * Returns true if two positions are equal
 */
bool positions_equal(const Position* lhs, const Position* rhs);

// debugging
void print_board(const Game* game);
void print_move(const Move* move);
#endif //CHECKERS_V2_GAME_H

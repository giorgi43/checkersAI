#include "game.h"
#include "vector.h"
#include <stdlib.h>

bool positions_equal(const Position* lhs, const Position* rhs) {
    return lhs->row == rhs->row && lhs->col == rhs->col;
}

static bool in_bounds(const Position* pos) {
    return pos->row >= 0 && pos->row < ROW_SIZE && pos->col >= 0 && pos->col < ROW_SIZE;
}

// checks if the position is occupied by some piece or it's blank
static Piece piece_at(const Game* game, const Position* pos) {
    return game->board[pos->row][pos->col];
}

static void initialize_row(int8_t* row, Piece piece, bool even) {
    memset(row, no_piece, ROW_SIZE);
    if (piece == no_piece) return;
    int8_t i = (even) ? 0 : 1;
    for (; i < ROW_SIZE; i += 2) {
        row[i] = piece;
    }
}

void all_moves(const Game* game, Player player, vector* moves) {
    if (player != computer && player != human) return;
    Piece pr = (player == human) ? white : black;
    Piece pk = (player == human) ? white_king : black_king;

    for (int8_t row = 0; row < ROW_SIZE; row++) {
        for (int8_t col = 0; col < COL_SIZE; col++) {
            // if piece belongs to this player
            if (game->board[row][col] == pr || game->board[row][col] == pk) {
                Position pos = {row, col};
                vector* piece_moves = all_moves_for_piece(game, &pos);
                for (int i = 0; i < VectorLength(piece_moves); i++) {
                    Move* move = VectorNth(piece_moves, i);
                    VectorAppend(moves, move);
                }
                VectorDispose(piece_moves);
            }
        }
    }
}

bool has_jump_move(const vector* moves) {
    for (int i = 0; i < VectorLength(moves); i++) {
        if (move_is_jump(VectorNth(moves, i))) return true;
    }
    return false;
}

void init_game(struct Game* game) {
    initialize_row(game->board[0], black, false);
    initialize_row(game->board[1], black, true);
    initialize_row(game->board[2], black, false);
    initialize_row(game->board[3], no_piece, false);
    initialize_row(game->board[4], no_piece, false);
    initialize_row(game->board[5], white, true);
    initialize_row(game->board[6], white, false);
    initialize_row(game->board[7], white, true);

    game->current_player = computer;
    game->status = RUNNING;
}

Player get_current_player(const Game* game) { return game->current_player; }

vector* all_moves_for_piece(const Game* game, const Position* pos) {
    int8_t piece = game->board[pos->row][pos->col];
    if (piece == no_piece) return NULL;

    vector* moves = malloc(sizeof(vector));
    VectorNew(moves, sizeof(Move), NULL, 4);

    Position pos_ul = {pos->row-1, pos->col-1};
    Move ul = {*pos, pos_ul, up_left};
    Position pos_ur = {pos->row-1, pos->col+1};
    Move ur = {*pos, pos_ur, up_right};
    Position pos_jul = {pos->row-2, pos->col-2};
    Move jul = {*pos, pos_jul, jump_up_left};
    Position pos_jur = {pos->row-2, pos->col+2};
    Move jur = {*pos, pos_jur, jump_up_right};
    Position pos_dl = {pos->row+1, pos->col-1};
    Move dl = {*pos, pos_dl, down_left};
    Position pos_dr = {pos->row+1, pos->col+1};
    Move dr = {*pos, pos_dr, down_right};
    Position pos_jdl = {pos->row+2, pos->col-2};
    Move jdl = {*pos, pos_jdl, jump_down_left};
    Position pos_jdr = {pos->row+2, pos->col+2};
    Move jdr = {*pos, pos_jdr, jump_down_right};

    if (move_is_valid(game, &ul)) VectorAppend(moves, &ul);
    if (move_is_valid(game, &ur)) VectorAppend(moves, &ur);
    if (move_is_valid(game, &jul)) VectorAppend(moves, &jul);
    if (move_is_valid(game, &jur)) VectorAppend(moves, &jur);
    if (move_is_valid(game, &dl)) VectorAppend(moves, &dl);
    if (move_is_valid(game, &dr)) VectorAppend(moves, &dr);
    if (move_is_valid(game, &jdl)) VectorAppend(moves, &jdl);
    if (move_is_valid(game, &jdr)) VectorAppend(moves, &jdr);

    return moves;
}

void move_piece(Game* game, const Move* move) {
    if (!move_is_valid(game, move)) return;

    int8_t piece = game->board[move->from.row][move->from.col];
    game->board[move->from.row][move->from.col] = no_piece;
    game->board[move->dest.row][move->dest.col] = piece;

    if (move->direction == jump_up_right || move->direction == jump_up_left
        || move->direction == jump_down_left || move->direction == jump_down_right) {

        switch (move->direction) {
            case jump_up_left:
                game->board[move->from.row-1][move->from.col-1] = no_piece; break;
            case jump_up_right:
                game->board[move->from.row-1][move->from.col+1] = no_piece; break;
            case jump_down_left:
                game->board[move->from.row+1][move->from.col-1] = no_piece; break;
            case jump_down_right:
                game->board[move->from.row+1][move->from.col+1] = no_piece; break;
        }
    }

    // check if this move should make ordinary piece the king
    if (piece == white && move->dest.row == 0) {
        game->board[move->dest.row][move->dest.col] = white_king;
    } else if (piece == black && move->dest.row == ROW_SIZE-1) {
        game->board[move->dest.row][move->dest.col] = black_king;
    }
}

void switch_player(Game* game) {
    game->current_player = (game->current_player == human) ? computer : human;
}

bool player_chooses_wrong_piece(const Game* game, const Position* pos) {
    int8_t piece = game->board[pos->row][pos->col];
    return (piece == black && game->current_player == human) ||
           (piece == black_king && game->current_player == human) ||
           (piece == white && game->current_player == computer) ||
           (piece == white_king && game->current_player == computer) ||
           (piece == no_piece);
}

bool move_is_valid(const Game* game, const Move* move) {
    if (in_bounds(&move->from) == false) return false;
    int8_t piece = game->board[move->from.row][move->from.col];
    if (player_chooses_wrong_piece(game, &move->from)) return false;
    if (!in_bounds(&move->dest)) return false;
    if (piece_at(game, &move->dest) != no_piece) return false;

    Position pos = move->from;
    Position pos_ul = {pos.row-1, pos.col-1};
    Position pos_ur = {pos.row-1, pos.col+1};
    Position pos_jul = {pos.row-2, pos.col-2};
    Position pos_jur = {pos.row-2, pos.col+2};
    Position pos_dl = {pos.row+1, pos.col-1};
    Position pos_dr = {pos.row+1, pos.col+1};
    Position pos_jdl = {pos.row+2, pos.col-2};
    Position pos_jdr = {pos.row+2, pos.col+2};

    if (piece == white || piece == white_king) {
       if (positions_equal(&move->dest, &pos_ul) || positions_equal(&move->dest, &pos_ur)) {
           return true; // up left or up right case
       } else if (piece == white_king && (positions_equal(&move->dest, &pos_dl) || positions_equal(&move->dest, &pos_dr))) {
           return true; // down left and down right case if piece is white_king
       } else { // jumps
           if (positions_equal(&move->dest, &pos_jul) && (piece_at(game, &pos_ul) == black
                || piece_at(game, &pos_ul) == black_king)) {
               return true; // jump up left case
           } else if (positions_equal(&move->dest, &pos_jur) && (piece_at(game, &pos_ur) == black
                        || piece_at(game, &pos_ur) == black_king)) {
               return true; // jump up right case
           } else if (piece == white_king && positions_equal(&move->dest, &pos_jdl) &&
                        (piece_at(game, &pos_dl) == black || piece_at(game, &pos_dl) == black_king)) {
              return true; // jump down left case for white_king
           } else if (piece == white_king && positions_equal(&move->dest, &pos_jdr) &&
                        (piece_at(game, &pos_dr) == black || piece_at(game, &pos_dr) == black_king)) {
               return true; // jump down right case for white_king
           }
           return false; // none
       }
    }
    // else piece is either black or black_king
    if (positions_equal(&move->dest, &pos_dl) || positions_equal(&move->dest, &pos_dr)) {
        return true; // down left or down right case (from whites perspective)
    } else if (piece == black_king && (positions_equal(&move->dest, &pos_ul) || positions_equal(&move->dest, &pos_ur))) {
        return true; // up left or up right case if piece is black_king
    } else { // jumps
        if (positions_equal(&move->dest, &pos_jdl) && (piece_at(game, &pos_dl) == white
            || piece_at(game, &pos_dl) == white_king)) {
            return true; // jump down left case
        } else if (positions_equal(&move->dest, &pos_jdr) && (piece_at(game, &pos_dr) == white
                || piece_at(game, &pos_dr) == white_king)) {
            return true; // jump down right case
        } else if (piece == black_king && positions_equal(&move->dest, &pos_jul) &&
                (piece_at(game, &pos_ul) == white || piece_at(game, &pos_ul) == white_king)) {
            return true; // jump up left case for black_king
        } else if (piece == black_king && positions_equal(&move->dest, &pos_jur) &&
                    (piece_at(game, &pos_ur) == white || piece_at(game, &pos_ur) == white_king)) {
            return true; // jump up right case for black_king
        }
    }
    return false;
}

bool move_is_jump(const Move* move) {
   return move->direction == jump_up_left || move->direction == jump_up_right
            || move->direction == jump_down_left || move->direction == jump_down_right;
}

void set_move_direction(Move* move) {
    Position pos = move->from;
    Position dest = move->dest;
    Position pos_ul = {pos.row-1, pos.col-1};
    Position pos_ur = {pos.row-1, pos.col+1};
    Position pos_jul = {pos.row-2, pos.col-2};
    Position pos_jur = {pos.row-2, pos.col+2};
    Position pos_dl = {pos.row+1, pos.col-1};
    Position pos_dr = {pos.row+1, pos.col+1};
    Position pos_jdl = {pos.row+2, pos.col-2};
    Position pos_jdr = {pos.row+2, pos.col+2};
    if (positions_equal(&dest, &pos_ul)) move->direction = up_left;
    else if (positions_equal(&dest, &pos_ur)) move->direction = up_right;
    else if (positions_equal(&dest, &pos_jul)) move->direction = jump_up_left;
    else if (positions_equal(&dest, &pos_jur)) move->direction = jump_up_right;
    else if (positions_equal(&dest, &pos_dl)) move->direction = down_left;
    else if (positions_equal(&dest, &pos_dr)) move->direction = down_right;
    else if (positions_equal(&dest, &pos_jdl)) move->direction = jump_down_left;
    else if (positions_equal(&dest, &pos_jdr)) move->direction = jump_down_right;
}

uint8_t player_pieces_count(const Game* game, Player player) {
    uint8_t count = 0;
    for (int row = 0; row < ROW_SIZE; row++) {
        for (int col = 0; col < COL_SIZE; col++) {
            if (player == human) {
                if (game->board[row][col] == white || game->board[row][col] == white_king) {
                    count++;
                }
            } else {
                if (game->board[row][col] == black || game->board[row][col] == black_king) {
                    count++;
                }
            }
        }
    }
    return count;
}

void print_board(const Game* game) {
    printf("\t\t\t- - - - - - - -\n");
    for (int i = 0; i < 8; i++) {
        printf("\t\t\t");
        for (int j = 0; j < 8; j++) {
            char p;
            if (game->board[i][j] == white) p = 'w';
            else if (game->board[i][j] == white_king) p = 'W';
            else if (game->board[i][j] == black) p = 'b';
            else if (game->board[i][j] == black_king) p = 'B';
            else p = '.';
            printf("%c ", p);
        }
        printf("\n");
    }
    printf("\t\t\t- - - - - - - -\n");
}

void print_move(const Move* move) {
    Position from = move->from;
    Position dest = move->dest;
    printf("Move: %d,%d --> %d,%d\n", from.row, from.col, dest.row, dest.col);
}

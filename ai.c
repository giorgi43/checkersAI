#include "ai.h"
#include <stdbool.h>

static int8_t find_val(const Game* game) {
    int8_t result = 0;
    for (int row = 0; row < ROW_SIZE; row++) {
        for (int col = 0; col < COL_SIZE; col++) {
            result += game->board[row][col];
        }
    }
    return result;
}

// takes jump move as parameter. (assume move is valid) returns true if there is multi-jump
static bool multi_jump_required(const Game* game, const Move* move) {
    Game game_copy = *game;
    move_piece(&game_copy, move);
    vector* moves = all_moves_for_piece(&game_copy, &move->dest);
    if (has_jump_move(moves)) {
        VectorDispose(moves);
        return true;
    }
    VectorDispose(moves);
    return false;
}

/*
 * Fills states vector with all possible game states that can be formed from this move
 * ---------------------------------------------------------
 * if move is regular move, there will be only one game state
 * if move is jump move, maybe player has more moves (multi-jumps) to make
 * so there will be different game states for all paths player can take for multi-jump
 * if player does not have multi-jump after jump move, there will be only one game state
 * (assuming move is valid, so no check for validity)
 * @param game game state
 * @param move move to make
 * @param game_states vector of Game structs
 */
static void move_game_states(const Game* game, const Move* move, vector* game_states) {
    Game new_game_state = *game;
    move_piece(&new_game_state, move); // make move
    VectorAppend(game_states, &new_game_state); // add this new game state to vector
    // if move is regular move there is no more moves
    if (!move_is_jump(move)) return;
    // if move is jump but there is no more moves return
    if (!multi_jump_required(game, move)) return;
    vector* moves = all_moves_for_piece(&new_game_state, &move->dest);
    for (int i = 0; i < VectorLength(moves); i++) {
        const Move* mv = VectorNth(moves, i);
        move_game_states(&new_game_state, mv, game_states);
    }
    VectorDispose(moves);
}

// gives node ownership to caller
void create_tree(Node* root, int depth) {
    // copy initial game state to root
    root->value = find_val(&root->game);
    VectorNew(&root->children, sizeof(Node), NULL, 1); // initialize children vector;
    if (depth == 0) {
        return;
    }
    // find all possible moves from this game state
    vector moves;
    VectorNew(&moves, sizeof(Move), NULL, 6);
    all_moves(&root->game, root->game.current_player, &moves);
    bool jump = has_jump_move(&moves); // if ai has jump moves it should jump
    // make this moves and create corresponding nodes. append to children
    for (int i = 0; i < VectorLength(&moves); i++) {
        Move* move = VectorNth(&moves, i); // get move from possible moves
        if (jump && !move_is_jump(move)) continue; // if ai has to jump but this move is not jump, skip
        // create vector to store all game states that can be formed from this move
        vector states;
        VectorNew(&states, sizeof(Game), NULL, 4);
        move_game_states(&root->game, move, &states);
        // create child node(s) and add to children vector of root
        for (int j = 0; j < VectorLength(&states); j++) {
            Game* state = VectorNth(&states, j);
            switch_player(state); // don't forget to switch player manually after move is done
            Node child;
            child.game = *state;
            //child.value = find_val(&child.game);
            VectorAppend(&root->children, &child);
        }
        VectorDispose(&states);
    }
    VectorDispose(&moves); // don't need anymore
    // now we have root node with current game state, having its children game states
    // call recursively
    for (int i = 0; i < VectorLength(&root->children); i++) {
        Node* child = VectorNth(&root->children, i);
        create_tree(child, depth-1);
    }
}

static void free_tree(Node* root) {
    if (root == NULL) return;
    for (int i = 0; i < VectorLength(&root->children); i++) {
        Node* child = VectorNth(&root->children, i);
        free_tree(child);
    }
    VectorDispose(&root->children);
}

// dumb test
void ai_move_dumb(Game* game) {
    Node root;
    root.game = *game;
    create_tree(&root, 3);

    int8_t res = 100;
    Game min_state;
    for (int i = 0; i < VectorLength(&root.children); i++) {
        Node* node = VectorNth(&root.children, i);
        int8_t tmp = find_val(&node->game);
        if (tmp < res) {
            res = tmp;
            min_state = node->game;
        }
    }
    *game = min_state;
    free_tree(&root);
}
static void min(Node* root) {
    if (VectorLength(&root->children) == 0) return;
    int8_t min_val = 127;
    for (int i = 0; i < VectorLength(&root->children); i++) {
        Node* child = VectorNth(&root->children, i);
        if (child->value < min_val) {
            min_val = child->value;
        }
    }
    root->value = min_val;
}

static void max(Node* root) {
    if (VectorLength(&root->children) == 0) return;
    int8_t max_val = -128;
    for (int i = 0; i < VectorLength(&root->children); i++) {
        Node* child = VectorNth(&root->children, i);
        if (child->value > max_val) {
            max_val = child->value;
        }
    }
    root->value = max_val;
}

void ai_move(Game* game, int depth) {
    Node root;
    root.game = *game;
    create_tree(&root, depth);
    set_child_values(&root, depth, MIN);
    // travers root's children and pick minimum
    for (int i = 0; i < VectorLength(&root.children); i++) {
        Node* child = VectorNth(&root.children, i);
        if (child->value == root.value) {
            *game = child->game;
            break;
        }
    }
    free_tree(&root);
}

void set_child_values(Node* root, int depth, MinMax minmax) {
    if (VectorLength(&root->children) == 0 || depth == 0) return;
    for (int i = 0; i < VectorLength(&root->children); i++) {
        Node* child = VectorNth(&root->children, i);
        set_child_values(child, depth-1, (minmax == MAX) ? MIN : MAX);
        if (minmax == MAX) {
            max(root);
        } else {
            min(root);
        }
    }
}


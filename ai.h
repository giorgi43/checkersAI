#ifndef CHECKERS_V2_AI_H
#define CHECKERS_V2_AI_H
#include "game.h"
#include "vector.h"

typedef enum {MIN, MAX} MinMax;

typedef struct Node {
    Game game;
    int8_t value; // heuristic value
    vector children; // vector of Nodes
} Node;

void create_tree(Node* root, int depth);

void ai_move_dumb(Game* game);
void ai_move(Game* game, int depth);
void set_child_values(Node* root, int depth, MinMax minmax);

#endif //CHECKERS_V2_AI_H

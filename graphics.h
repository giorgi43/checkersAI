#ifndef CHECKERS_V2_GRAPHICS_H
#define CHECKERS_V2_GRAPHICS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "game.h"

#define BOARD_WIDTH  800
#define BOARD_HEIGHT 800
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 841
#define CELL_WIDTH (WINDOW_WIDTH/ROW_SIZE)
#define PIECE_RADIUS (((WINDOW_WIDTH/COL_SIZE) - 30)/2)
#define OFFSET 20

typedef struct Textures {
    SDL_Texture* texture_w;
    SDL_Texture* texture_wk;
    SDL_Texture* texture_b;
    SDL_Texture* texture_bk;
    SDL_Texture* texture_reds_turn;
    SDL_Texture* texture_blacks_turn;
    SDL_Texture* texture_red_won;
    SDL_Texture* texture_black_won;
    SDL_Texture* texture_draw;
} Textures;

/*
 * Initialize graphics window and renderer
 */
int init_graphics(SDL_Window** window, SDL_Renderer** renderer, Textures* textures);

/*
 * Free up memory and quit
 */
void free_graphics(SDL_Window** window, SDL_Renderer** renderer, Textures* textures);

/*
 * Renders whole board and players info
 */
void render_game(SDL_Renderer* renderer, const Game* game, const Textures* textures, const Position* highlight_pos);

/*
 * Renders board (with pieces)
 */
void render_game_board(SDL_Renderer* renderer, const Game* game,
                       const Textures* textures, const Position* highlight_pos);


#endif //CHECKERS_V2_GRAPHICS_H

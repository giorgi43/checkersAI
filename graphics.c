#include "graphics.h"

static const SDL_Color color_dark = {136,163,178,255};//{170,161,161,255};//{192,45,45,255};
static const SDL_Color color_light = {219,219,219,255};//{243,243,243,255};//{13,13,16,255};
static const SDL_Color color_highlight = {42,237,44,100};
static const SDL_Color color_contour = {0,0,0,255};
static const SDL_Color display_color = {0,0,0,255}; //{100, 104, 110, 255};

int init_graphics(SDL_Window** window, SDL_Renderer** renderer, Textures* textures) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    *window = SDL_CreateWindow("Checkers",
          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    *renderer = SDL_CreateRenderer(*window,
                      -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) {
        fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    // Load textures (White = Red & Black = Black)
    SDL_Surface* ws = IMG_Load("./texture/white.png");
    SDL_Surface* wks = IMG_Load("./texture/white_king.png");
    SDL_Surface* bs = IMG_Load("./texture/black.png");
    SDL_Surface* bks = IMG_Load("./texture/black_king.png");
    SDL_Surface* reds_turn_s = IMG_Load("./texture/RedsTurn.png");
    SDL_Surface* blacks_turn_s = IMG_Load("./texture/BlacksTurn.png");
    SDL_Surface* draw_s = IMG_Load("./texture/Draw.png");
    SDL_Surface* blacks_won_s = IMG_Load("./texture/BlackIsWinner.png");
    SDL_Surface* reds_won_s = IMG_Load("./texture/RedIsWinner.png");
    if (ws == NULL || bs == NULL || wks == NULL || bks == NULL ||
        reds_turn_s == NULL || blacks_turn_s == NULL || draw_s == NULL || blacks_won_s == NULL || reds_won_s == NULL) {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_Texture* w_texture = SDL_CreateTextureFromSurface(*renderer, ws);
    SDL_Texture* wk_texture = SDL_CreateTextureFromSurface(*renderer, wks);
    SDL_Texture* b_texture = SDL_CreateTextureFromSurface(*renderer, bs);
    SDL_Texture* bk_texture = SDL_CreateTextureFromSurface(*renderer, bks);
    SDL_Texture* reds_turn_texture = SDL_CreateTextureFromSurface(*renderer, reds_turn_s);
    SDL_Texture* blacks_turn_texture = SDL_CreateTextureFromSurface(*renderer, blacks_turn_s);
    SDL_Texture* draw_texture = SDL_CreateTextureFromSurface(*renderer, draw_s);
    SDL_Texture* black_won_texture = SDL_CreateTextureFromSurface(*renderer, blacks_won_s);
    SDL_Texture* red_won_texture = SDL_CreateTextureFromSurface(*renderer, reds_won_s);
    // free surfaces
    SDL_FreeSurface(ws); SDL_FreeSurface(bs); SDL_FreeSurface(wks);
    SDL_FreeSurface(bks); SDL_FreeSurface(reds_turn_s); SDL_FreeSurface(blacks_turn_s);
    SDL_FreeSurface(draw_s); SDL_FreeSurface(blacks_won_s); SDL_FreeSurface(reds_won_s);


    if (w_texture == NULL || wk_texture == NULL || b_texture == NULL
        || bk_texture == NULL || reds_turn_texture == NULL || blacks_turn_texture == NULL
        || draw_texture == NULL || black_won_texture == NULL || red_won_texture == NULL) {
       fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    textures->texture_w = w_texture;
    textures->texture_wk = wk_texture;
    textures->texture_b = b_texture;
    textures->texture_bk = bk_texture;
    textures->texture_reds_turn = reds_turn_texture;
    textures->texture_blacks_turn = blacks_turn_texture;
    textures->texture_black_won = black_won_texture;
    textures->texture_red_won = red_won_texture;
    textures->texture_draw = draw_texture;
    return EXIT_SUCCESS;
}

void free_graphics(SDL_Window** window, SDL_Renderer** renderer, Textures* textures) {
    SDL_DestroyTexture(textures->texture_w);
    SDL_DestroyTexture(textures->texture_wk);
    SDL_DestroyTexture(textures->texture_b);
    SDL_DestroyTexture(textures->texture_bk);
    SDL_DestroyTexture(textures->texture_reds_turn);
    SDL_DestroyTexture(textures->texture_blacks_turn);
    SDL_DestroyTexture(textures->texture_draw);
    SDL_DestroyTexture(textures->texture_black_won);
    SDL_DestroyTexture(textures->texture_red_won);
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    SDL_Quit();
}

void render_game(SDL_Renderer* renderer, const Game* game, const Textures* textures, const Position* highlight_pos) {
    render_game_board(renderer, game, textures, highlight_pos);
}

static void render_circle(SDL_Renderer* renderer, int32_t center_x, int32_t center_y, int32_t radius) {
    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);
    while (x >= y) {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, center_x + x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y + x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

static void render_filled_circle(SDL_Renderer* renderer, int32_t x, int32_t y, int32_t radius) {
    int32_t offsetx, offsety, d;
    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    while (offsety >= offsetx) {
        SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,x + offsety, y + offsetx);
        SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,x + offsetx, y + offsety);
        SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,x + offsetx, y - offsety);
        SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,x + offsety, y - offsetx);
        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

static void render_piece(SDL_Renderer* renderer, int32_t row, int32_t col, const SDL_Color* color, bool king) {
    int32_t cell_width = BOARD_WIDTH / ROW_SIZE;
    int32_t cell_height = BOARD_HEIGHT / COL_SIZE;
    int32_t piece_center_x = (col * cell_width) + cell_width/2;
    int32_t piece_center_y = (row * cell_height) + cell_height/2;

    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    render_filled_circle(renderer, piece_center_x, piece_center_y, PIECE_RADIUS);

    if (king) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        render_circle(renderer, piece_center_x, piece_center_y, PIECE_RADIUS);
        render_circle(renderer, piece_center_x, piece_center_y, PIECE_RADIUS - 5);
        render_circle(renderer, piece_center_x, piece_center_y, PIECE_RADIUS - 10);
        render_circle(renderer, piece_center_x, piece_center_y, PIECE_RADIUS - 15);
    }
}

void render_game_board(SDL_Renderer* renderer, const Game* game, const Textures* textures, const Position* highlight_pos) {
    SDL_Rect rect;
    rect.w = rect.h = 80;
    SDL_Rect cell;
    cell.w = cell.h = BOARD_WIDTH/COL_SIZE;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            cell.x = col * cell.w;
            cell.y = row * cell.h;
            if ((row + col)%2 == 0)
                SDL_SetRenderDrawColor(renderer,color_light.r,color_light.b,color_light.b,color_light.a);
            else
                SDL_SetRenderDrawColor(renderer, color_dark.r,color_dark.g,color_dark.b,color_dark.a);
            SDL_RenderFillRect(renderer, &cell);
            // draw contour
            //SDL_SetRenderDrawColor(renderer, color_contour.r, color_contour.g, color_contour.b, color_contour.a);
            //SDL_RenderDrawRect(renderer, &cell);
            // highlight
            if (highlight_pos != NULL && row == highlight_pos->row && col == highlight_pos->col) {
                SDL_SetRenderDrawColor(renderer, color_highlight.r, color_highlight.g, color_highlight.b, color_highlight.a);
                SDL_RenderFillRect(renderer, &cell);
            }

            // add pieces on the board
            rect.x = cell.x + OFFSET/2;
            rect.y = cell.y + OFFSET/2;
            if (game->board[row][col] == white) {
                SDL_RenderCopy(renderer, textures->texture_w, NULL, &rect);
            } else if (game->board[row][col] == white_king) {
                SDL_RenderCopy(renderer, textures->texture_wk, NULL, &rect);
            } else if (game->board[row][col] == black) {
                SDL_RenderCopy(renderer, textures->texture_b, NULL, &rect);
            } else if (game->board[row][col] == black_king) {
                SDL_RenderCopy(renderer, textures->texture_bk, NULL, &rect);
            }
        }
    }
    // status display
    SDL_SetRenderDrawColor(renderer, color_contour.r, color_contour.g, color_contour.b, color_contour.a);
    SDL_RenderDrawLine(renderer, 0, BOARD_HEIGHT, WINDOW_WIDTH, BOARD_HEIGHT);
    SDL_SetRenderDrawColor(renderer, display_color.r, display_color.g, display_color.b, display_color.a); // status display color
    switch (game->status) {
        case RUNNING: {
           if (game->current_player == human) {
               SDL_Rect text_rect = {10,BOARD_HEIGHT+5,188,31};
               SDL_RenderCopy(renderer, textures->texture_reds_turn, NULL, &text_rect); // render image
           } else {
               SDL_Rect text_rect = {10,BOARD_HEIGHT+5,222,31};
               SDL_RenderCopy(renderer, textures->texture_blacks_turn, NULL, &text_rect); // render image
           }
           break;
        }
        case HUMAN_WON: {
            SDL_Rect text_rect = {10,BOARD_HEIGHT+5,270,31};
            SDL_RenderCopy(renderer, textures->texture_red_won, NULL, &text_rect); // render image
            break;
        }
        case COMPUTER_WON: {
            SDL_Rect text_rect = {10,BOARD_HEIGHT+5,304,31};
            SDL_RenderCopy(renderer, textures->texture_black_won, NULL, &text_rect); // render image
            break;
        }
        case DRAW: {
            SDL_Rect text_rect = {10,BOARD_HEIGHT+5,115,31};
            SDL_RenderCopy(renderer, textures->texture_draw, NULL, &text_rect); // render image
            break;
        }
        default: {}
    }

}

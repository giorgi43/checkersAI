#include "game_loop.h"
void game_loop(Game* game, SDL_Renderer* renderer, Textures* textures,
               SDL_AudioDeviceID device, const Uint8* audio_buffer, Uint32 len) {
    print_board(game);
    bool first_press = true;
    bool move_formed = false;
    bool multi_jump_required = false;

    Move move, last_move;
    Position from, dest;

    vector possible_moves; // all possible moves for current player
    VectorNew(&possible_moves, sizeof(Move), NULL, 8);

    SDL_RenderClear(renderer);
    render_game(renderer, game, textures, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    SDL_Event e;
    while (game->status == RUNNING) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: game->status = QUIT; break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (first_press) {
                        from.row = e.button.y / CELL_WIDTH;
                        from.col = e.button.x / CELL_WIDTH;
                        first_press = false;
                        move_formed = false;
                    } else {
                        dest.row = e.button.y/CELL_WIDTH;
                        dest.col = e.button.x/CELL_WIDTH;
                        first_press = true;
                        move_formed = true;
                    }
                }
            }
        }
        // render board
        SDL_RenderClear(renderer);
        render_game(renderer, game, textures, (!first_press) ? &from : NULL); // render board
        SDL_RenderPresent(renderer);
        SDL_Delay(50);

        uint8_t pc = player_pieces_count(game, game->current_player);
        // if one of the player has no pieces left, opponent won the game
        if (game->current_player == human && pc == 0) {
            game->status = COMPUTER_WON;
            continue;
        } else if (game->current_player == computer && pc == 0) {
            game->status = HUMAN_WON;
            continue;
        }

        // if player has no legal moves, declare draw
        all_moves(game, game->current_player, &possible_moves);
        if (VectorLength(&possible_moves) == 0) {
            VectorClear(&possible_moves);
            game->status = DRAW;
            continue;
        }
        VectorClear(&possible_moves);

        // (computer vs player mode)
        // if computers turn, make move; continue
        if (game->current_player == computer) {
            SDL_Delay(500);
            ai_move(game, 7);
            play_audio(device, audio_buffer, len);
            continue;
        }

        if (move_formed == false) continue;
        move.from = from;
        move.dest = dest;
        print_move(&move);
        // at this point we have move. set direction and check for validity
        set_move_direction(&move);
        if (!move_is_valid(game, &move)) {
            printf("Wrong move.. choose again\n");
            first_press = true;
            move_formed = false;
            continue;
        }
        // if multi-jump required
        if (multi_jump_required) {
            // if selected move follows last jump but is not jump or selected move is not jump
            if ((positions_equal(&move.from, &last_move.dest) && !move_is_jump(&move))
                                    || !positions_equal(&move.from, &last_move.dest)) {
                first_press = true;
                move_formed = false;
                continue;
            }
            move_piece(game, &move);
        }
        // move is valid now
        // also check if move is not jump but player has jump move (not permitted according to rules)
        all_moves(game, game->current_player, &possible_moves); // add all possible moves for current player to vector
        if (has_jump_move(&possible_moves) && !move_is_jump(&move)) {
            printf("Player must jump\n");
            VectorClear(&possible_moves);
            first_press = true;
            move_formed = false;
            continue;
        }

        // if move is jump, check if one more jump is possible
        if (move_is_jump(&move)) {
            play_audio(device, audio_buffer, len);

            move_piece(game, &move);
            // if multi-jumps are possible, don't switch to other player. require player to multi-jump
            vector* piece_moves = all_moves_for_piece(game, &move.dest);
            if (has_jump_move(piece_moves)) {
                VectorDispose(piece_moves);
                printf("Player must multi-jump\n");
                last_move = move;
                multi_jump_required = true;
                VectorClear(&possible_moves);
                first_press = true;
                move_formed = false;
                continue;
            }
            VectorDispose(piece_moves);
        }

        if (!multi_jump_required && !move_is_jump(&move)) {
            move_piece(game, &move);
            play_audio(device, audio_buffer, len);
        }
        switch_player(game);
        multi_jump_required = false;

        VectorClear(&possible_moves); // clear possible moves for current player
        first_press = true;
        move_formed = false;
    }
    VectorDispose(&possible_moves);
    SDL_RenderClear(renderer);
    render_game(renderer, game, textures, NULL); // render board
    SDL_RenderPresent(renderer);
    if (game->status != QUIT) SDL_Delay(4000); // wait 4 sec before exit
}
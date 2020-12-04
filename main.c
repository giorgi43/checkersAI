#include "game.h"
#include "graphics.h"
#include "audio.h"
#include "game_loop.h"

int main(int argc, char* argv[]) {
    // initialize graphics
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    Textures textures;
    if (init_graphics(&window, &renderer, &textures) != 0) {
        return EXIT_FAILURE;
    }
    // initialize audio
    SDL_AudioSpec spec;
    Uint32 len;
    Uint8* audio_buffer;
    SDL_AudioDeviceID audio_device = init_audio(&spec, &len, &audio_buffer);

    // initialize game
    Game game;
    init_game(&game);

    // start game main loop
    game_loop(&game, renderer, &textures, audio_device, audio_buffer, len);

    // free up memory
    free_audio(audio_device, audio_buffer);
    free_graphics(&window, &renderer, &textures);
    return EXIT_SUCCESS;
}

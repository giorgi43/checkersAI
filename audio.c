#include "audio.h"

SDL_AudioDeviceID init_audio(SDL_AudioSpec* spec, Uint32* len, Uint8** buffer) {
    if (SDL_LoadWAV("./sfx/move.wav", spec, buffer, len) == NULL) {
        fprintf(stderr, "Could not open wav file: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_AudioDeviceID device;
    if ((device = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0)) == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return device;
}

int play_audio(SDL_AudioDeviceID device, const Uint8* buffer, Uint32 len) {
   if (SDL_QueueAudio(device, buffer, len) != 0) {
       fprintf(stderr, "%s\n", SDL_GetError());
       return EXIT_FAILURE;
   }
   SDL_PauseAudioDevice(device, 0);
}

void free_audio(SDL_AudioDeviceID device, Uint8* buffer) {
    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(buffer);
}
#ifndef CHECKERS_V2_AUDIO_H
#define CHECKERS_V2_AUDIO_H
#include <SDL2/SDL_audio.h>

SDL_AudioDeviceID init_audio(SDL_AudioSpec* spec, Uint32* len, Uint8** buffer);
int play_audio(SDL_AudioDeviceID device, const Uint8* buffer, Uint32 len);
void free_audio(SDL_AudioDeviceID device, Uint8* buffer);

#endif //CHECKERS_V2_AUDIO_H

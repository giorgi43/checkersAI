#ifndef CHECKERS_V2_GAME_LOOP_H
#define CHECKERS_V2_GAME_LOOP_H
#include "game.h"
#include "graphics.h"
#include "audio.h"
#include "ai.h"

void game_loop(Game* game, SDL_Renderer* renderer, Textures* textures,
               SDL_AudioDeviceID device, const Uint8* audio_buffer, Uint32 len);

#endif //CHECKERS_V2_GAME_LOOP_H

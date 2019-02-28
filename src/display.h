#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#include "cpu.h"

#define SDL_SCREEN_WIDTH 1280
#define SDL_SCREEN_HEIGHT 640
#define SDL_WINDOW_TITLE "Chip8 Emulator"

SDL_Window *initWindow();
SDL_Renderer *initRenderer(SDL_Window*);

void renderCPU(SDL_Renderer*, CPU);

void freeWindow(SDL_Window*);
void freeRenderer(SDL_Renderer*);

#endif

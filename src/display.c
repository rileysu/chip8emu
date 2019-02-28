#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "cpu.h"

SDL_Window *initWindow(){
	SDL_Window *window =  SDL_CreateWindow(
				SDL_WINDOW_TITLE,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_SCREEN_WIDTH,
				SDL_SCREEN_HEIGHT,
				SDL_WINDOW_OPENGL);
	if (window == NULL){
		fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
		exit(-1);
	}
}

SDL_Renderer *initRenderer(SDL_Window* window){
	SDL_Renderer *renderer = SDL_CreateRenderer(window,
							-1,
							SDL_RENDERER_ACCELERATED);
	if (renderer == NULL){	
		fprintf(stderr, "Could not create SDL renderer: %s\n", SDL_GetError());
		exit(-1);
	}
}

void renderCPU(SDL_Renderer *renderer, CPU cpu){
	if (cpu->uscreen != 1)
		return;
	cpu->uscreen = 0;

	int mulx = SDL_SCREEN_WIDTH / SCREEN_WIDTH;
	int muly = SDL_SCREEN_HEIGHT / SCREEN_HEIGHT;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect r;
	for (int y = 0; y < SCREEN_HEIGHT; y++){
		for (int x = 0; x < SCREEN_WIDTH; x++){
			if (cpu->screen[x][y]){
				r.x = x * mulx;
				r.y = y * muly;
				r.w = mulx;
				r.h = muly;
				SDL_RenderFillRect(renderer, &r);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void freeWindow(SDL_Window *window){
	SDL_DestroyWindow(window);
}

void freeRenderer(SDL_Renderer *renderer){
	SDL_DestroyRenderer(renderer);
}

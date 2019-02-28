#include <stdio.h>
#include <SDL2/SDL.h>

#include "reader.h"
#include "cpu.h"
#include "display.h"

int main(int argc, char *argv[]){
	if (argc != 2){
		return 1;
	}

	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	ArrList l = readFile(argv[1]);
	CPU cpu = initCPU();
	int quit = 0;
	SDL_Event e;
	

	loadCPU(l, cpu);
	freeArrList(l);
	SDL_Window *window = initWindow();
	SDL_Renderer *renderer = initRenderer(window);
	
	while (!quit){
		while(SDL_PollEvent(&e) != 0){
			switch(e.type){
				case SDL_QUIT:
					quit = 1;
					break;
			}
		}
		//printDiagsCPU(cpu);
		renderCPU(renderer, cpu);
		stepCPU(cpu);
	}
	freeCPU(cpu);
	freeRenderer(renderer);
	freeWindow(window);
	SDL_Quit();
}

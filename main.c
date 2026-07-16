#include <stdio.h>

#include <unistd.h>

#include <SDL2/SDL.h>

#include "chip8.h"

#define scale 8

int main(void)
{
	SDL_Window* window = (void*)0;
	SDL_Renderer* renderer = (void*)0;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(64 * scale, 32 * scale, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, scale, scale);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, "roms/animation.ch8");

	for (int i = 0; i < 25; i++)
	{
		chip8_cycle(&chip8);
		usleep(300000);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
		    if (event.type == SDL_QUIT) {
		        return -1;
		    }
		}
		
		for (int j = 0; j < 32; j++)
		{
			for (int k = 0; k < 64; k++)
			{
				if (chip8.display[j * 64 + k] == 1)
				{
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					SDL_RenderDrawPoint(renderer, k, j);
				}
				else
				{
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderDrawPoint(renderer, k, j);
				}
			}
		}
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	
	return 0;
}

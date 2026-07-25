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
	chip8_load_rom(&chip8, "roms/3-corax+.ch8");

	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type){
				case SDL_QUIT:
					return -1;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_1:
							chip8.keypad[0x1] = 1;
							break;
						case SDLK_2:
							chip8.keypad[0x2] = 1;
							break;
						case SDLK_3:
							chip8.keypad[0x3] = 1;
							break;
						case SDLK_4:
							chip8.keypad[0xC] = 1;
							break;
						case SDLK_q:
							chip8.keypad[0x4] = 1;
							break;
						case SDLK_w:
							chip8.keypad[0x5] = 1;
							break;
						case SDLK_e:
							chip8.keypad[0x6] = 1;
							break;
						case SDLK_r:
							chip8.keypad[0xD] = 1;
							break;
						case SDLK_a:
							chip8.keypad[0x7] = 1;
							break;
						case SDLK_s:
							chip8.keypad[0x8] = 1;
							break;
						case SDLK_d:
							chip8.keypad[0x9] = 1;
							break;
						case SDLK_f:
							chip8.keypad[0xE] = 1;
							break;
						case SDLK_z:
							chip8.keypad[0xA] = 1;
							break;
						case SDLK_x:
							chip8.keypad[0x0] = 1;
							break;
						case SDLK_c:
							chip8.keypad[0xB] = 1;
							break;
						case SDLK_v:
							chip8.keypad[0xF] = 1;
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_1:
							chip8.keypad[0x1] = 0;
							break;
						case SDLK_2:
							chip8.keypad[0x2] = 0;
							break;
						case SDLK_3:
							chip8.keypad[0x3] = 0;
							break;
						case SDLK_4:
							chip8.keypad[0xC] = 0;
							break;
						case SDLK_q:
							chip8.keypad[0x4] = 0;
							break;
						case SDLK_w:
							chip8.keypad[0x5] = 0;
							break;
						case SDLK_e:
							chip8.keypad[0x6] = 0;
							break;
						case SDLK_r:
							chip8.keypad[0xD] = 0;
							break;
						case SDLK_a:
							chip8.keypad[0x7] = 0;
							break;
						case SDLK_s:
							chip8.keypad[0x8] = 0;
							break;
						case SDLK_d:
							chip8.keypad[0x9] = 0;
							break;
						case SDLK_f:
							chip8.keypad[0xE] = 0;
							break;
						case SDLK_z:
							chip8.keypad[0xA] = 0;
							break;
						case SDLK_x:
							chip8.keypad[0x0] = 0;
							break;
						case SDLK_c:
							chip8.keypad[0xB] = 0;
							break;
						case SDLK_v:
							chip8.keypad[0xF] = 0;
							break;
						case SDLK_ESCAPE:
							SDL_DestroyRenderer(renderer);
							SDL_DestroyWindow(window);
							SDL_Quit();
							return -1;
							break;
					}
					break;
			}

		}

		for (int i = 0; i < 30; i++) 
		{
			chip8_cycle(&chip8);
		}

		if (chip8.delay_timer > 0)
		{
		    chip8.delay_timer--;
		}
		
		if (chip8.sound_timer > 0)
		{
		    // Trigger
		    chip8.sound_timer--;
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

		usleep(16000);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}

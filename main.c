#include <stdio.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "chip8.h"

#define SCALE 8
#define CYCLES_PER_FRAME 200

int main(int argc, char* argv[])
{
	if (argc < 2) return fprintf(stderr, "Usage: %s <path/to/rom>\n", argv[0]), -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) return fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError()), -1;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_CreateWindowAndRenderer(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, 0, &window, &renderer);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
	SDL_RenderSetScale(renderer, SCALE, SCALE);

	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, argv[1]);

	uint64_t last_time = SDL_GetPerformanceCounter();
	double target_frametime = 1.0 / 60.0;
	double accumulator = 0.0;

	while (1)
	{
		uint64_t current_time = SDL_GetPerformanceCounter();
		double delta_time = (double)(current_time - last_time) / SDL_GetPerformanceFrequency();
		last_time = current_time;
		if (delta_time > 0.25) delta_time = 0.25;
		accumulator += delta_time;
	
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				SDL_DestroyTexture(texture);
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
				return -1;
			}
			if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0)
			{
				uint8_t pressed = (event.type == SDL_KEYDOWN);
				switch(event.key.keysym.sym)
				{
					case SDLK_1: chip8.keypad[0x1] = pressed; break;
					case SDLK_2: chip8.keypad[0x2] = pressed; break;
					case SDLK_3: chip8.keypad[0x3] = pressed; break;
					case SDLK_4: chip8.keypad[0xC] = pressed; break;
					case SDLK_q: chip8.keypad[0x4] = pressed; break;
					case SDLK_w: chip8.keypad[0x5] = pressed; break;
					case SDLK_e: chip8.keypad[0x6] = pressed; break;
					case SDLK_r: chip8.keypad[0xD] = pressed; break;
					case SDLK_a: chip8.keypad[0x7] = pressed; break;
					case SDLK_s: chip8.keypad[0x8] = pressed; break;
					case SDLK_d: chip8.keypad[0x9] = pressed; break;
					case SDLK_f: chip8.keypad[0xE] = pressed; break;
					case SDLK_z: chip8.keypad[0xA] = pressed; break;
					case SDLK_x: chip8.keypad[0x0] = pressed; break;
					case SDLK_c: chip8.keypad[0xB] = pressed; break;
					case SDLK_v: chip8.keypad[0xF] = pressed; break;
				}
			}
		}
		
		while (accumulator >= target_frametime)
		{
			for (int i = 0; i < CYCLES_PER_FRAME; i++) chip8_cycle(&chip8);
		    if (chip8.delay_timer > 0) chip8.delay_timer--;
		    if (chip8.sound_timer > 0) chip8.sound_timer--;
		    accumulator -= target_frametime;
		}

		uint32_t pixels[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];
		for (int k = 0; k < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; k++) pixels[k] = chip8.display[k] ? 0xFFFFFFFF : 0xFF000000;
		SDL_UpdateTexture(texture, NULL, pixels, CHIP8_DISPLAY_WIDTH * sizeof(uint32_t));
        
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_Delay(1);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}

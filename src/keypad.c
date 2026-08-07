#include "keypad.h"
#include "chip8.h"
#include "video.h"

#include <SDL3/SDL.h>

int keypad_poll(void* userdata)
{
	Chip8* chip8 = (Chip8*)userdata;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) return -1;
		if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) memset(chip8->keypad, 0, sizeof(chip8->keypad));
		if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
		{
			uint8_t pressed = event.key.down;
			if (!event.key.repeat)
			{
				switch(event.key.key)
				{
					case SDLK_1: chip8->keypad[0x1] = pressed; break;
					case SDLK_2: chip8->keypad[0x2] = pressed; break;
					case SDLK_3: chip8->keypad[0x3] = pressed; break;
					case SDLK_4: chip8->keypad[0xC] = pressed; break;
					case SDLK_Q: chip8->keypad[0x4] = pressed; break;
					case SDLK_W: chip8->keypad[0x5] = pressed; break;
					case SDLK_E: chip8->keypad[0x6] = pressed; break;
					case SDLK_R: chip8->keypad[0xD] = pressed; break;
					case SDLK_A: chip8->keypad[0x7] = pressed; break;
					case SDLK_S: chip8->keypad[0x8] = pressed; break;
					case SDLK_D: chip8->keypad[0x9] = pressed; break;
					case SDLK_F: chip8->keypad[0xE] = pressed; break;
					case SDLK_Z: chip8->keypad[0xA] = pressed; break;
					case SDLK_X: chip8->keypad[0x0] = pressed; break;
					case SDLK_C: chip8->keypad[0xB] = pressed; break;
					case SDLK_V: chip8->keypad[0xF] = pressed; break;
				}	
			}
		}
	}
	return 0;
}

#include "keypad.h"
#include "chip8.h"
#include "video.h"

int keypad_poll(void* userdata)
{
	Chip8* chip8 = (Chip8*)userdata;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) return video_close(), -1;
		if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0)
		{
			uint8_t pressed = (event.type == SDL_KEYDOWN);
			switch(event.key.keysym.sym)
			{
				case SDLK_1: chip8->keypad[0x1] = pressed; break;
				case SDLK_2: chip8->keypad[0x2] = pressed; break;
				case SDLK_3: chip8->keypad[0x3] = pressed; break;
				case SDLK_4: chip8->keypad[0xC] = pressed; break;
				case SDLK_q: chip8->keypad[0x4] = pressed; break;
				case SDLK_w: chip8->keypad[0x5] = pressed; break;
				case SDLK_e: chip8->keypad[0x6] = pressed; break;
				case SDLK_r: chip8->keypad[0xD] = pressed; break;
				case SDLK_a: chip8->keypad[0x7] = pressed; break;
				case SDLK_s: chip8->keypad[0x8] = pressed; break;
				case SDLK_d: chip8->keypad[0x9] = pressed; break;
				case SDLK_f: chip8->keypad[0xE] = pressed; break;
				case SDLK_z: chip8->keypad[0xA] = pressed; break;
				case SDLK_x: chip8->keypad[0x0] = pressed; break;
				case SDLK_c: chip8->keypad[0xB] = pressed; break;
				case SDLK_v: chip8->keypad[0xF] = pressed; break;
			}
		}
	}
	return 0;
}

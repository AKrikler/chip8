#include "video.h"
#include "chip8.h"

#include <stdio.h>

#include <SDL3/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

static int closed = 0;

int video_init(const char* path)
{		
	char filename[64];
	const char* fwd = strrchr(path, '/');
	const char* back = strrchr(path, '\\');
	const char* slash = fwd > back ? fwd : back;
	strncpy(filename, slash ? slash + 1 : path, 64);
	*strrchr(filename, '.') = '\0';
	filename[63] = '\0';

	SDL_CreateWindowAndRenderer(filename, CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, 0, &window, &renderer);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderScale(renderer, SCALE, SCALE);
	
	return 0;
}

void video_render(void* userdata)
{
	uint32_t pixels[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];
	for (int k = 0; k < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; k++) pixels[k] = ((Chip8*)userdata)->display[k] ? 0xFFFFFFFF : 0xFF000000;
	SDL_UpdateTexture(texture, NULL, pixels, CHIP8_DISPLAY_WIDTH * sizeof(uint32_t));

	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void video_close(void)
{
	if (!closed)
	{
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		if (renderer) {
			SDL_SetRenderTarget(renderer, NULL);
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}
		if (window) {
			SDL_DestroyWindow(window);
			window = NULL;
		}

		SDL_Quit();
		closed = 1;
	}
}

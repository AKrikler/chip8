#include "video.h"
#include "chip8.h"

#include <stdio.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

static int closed = 0;

int video_init(const char* path)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError()), -1;

	SDL_CreateWindowAndRenderer(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, 0, &window, &renderer);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
	SDL_RenderSetScale(renderer, SCALE, SCALE);


	char filename[64];
	const char* fwd = strrchr(path, '/');
	const char* back = strrchr(path, '\\');
	const char* slash = fwd > back ? fwd : back;
	strncpy(filename, slash ? slash + 1 : path, 64);
	*strrchr(filename, '.') = '\0';
	filename[63] = '\0';
	SDL_SetWindowTitle(window, filename);
	
	return 0;
}

void video_render(void* userdata)
{
	uint32_t pixels[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];
	for (int k = 0; k < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; k++) pixels[k] = ((Chip8*)userdata)->display[k] ? 0xFFFFFFFF : 0xFF000000;
	SDL_UpdateTexture(texture, NULL, pixels, CHIP8_DISPLAY_WIDTH * sizeof(uint32_t));

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(1);
}

void video_close(void)
{
	if (!closed)
	{
		if (texture) SDL_DestroyTexture(texture);
		if (renderer) SDL_DestroyRenderer(renderer);
		if (window) SDL_DestroyWindow(window);
		SDL_Quit();
		closed = 1;
	}
}

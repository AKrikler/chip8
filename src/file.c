#include "file.h"

#include <SDL3/SDL.h>

typedef struct {
    char* path;
    int finished;
} FileDialogData;

static void SDLCALL file_callback(void* userdata, const char* const* filelist, int filter)
{
    (void)filter;
    FileDialogData* data = (FileDialogData*)userdata;
    
    if (!filelist) {
        SDL_Log("SDL Dialog Error: %s", SDL_GetError());
        data->path = NULL;
    } else if (!*filelist) {
        SDL_Log("Dialog cancelled by user.");
        data->path = NULL;
    } else {
        data->path = SDL_strdup(*filelist);
    }
    
    data->finished = 1;
}

const char* open_file(void)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return NULL;
	
	FileDialogData data;
	data.path = NULL;
	data.finished = 0;

	SDL_DialogFileFilter filters[] = { { "CHIP-8 ROMs", "ch8" }, { "All Files", "*" } };
	SDL_ShowOpenFileDialog(file_callback, &data, NULL, filters, 2, NULL, 0);

	while (!data.finished)
	{
		SDL_PumpEvents();
		SDL_Delay(10);
	}

	return data.path;
}

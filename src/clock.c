#include "clock.h"

#include "SDL2/SDL.h"

void clock_init(Clock* clock, double fps)
{
	clock->last_time = SDL_GetPerformanceCounter();
	clock->target_frametime = 1.0 / fps;
	clock->accumulator = 0;
}

int clock_process_frame(Clock* clock)
{
	uint64_t current_time = SDL_GetPerformanceCounter();
	double delta_time = (double)(current_time - clock->last_time) / SDL_GetPerformanceFrequency();
	clock->last_time = current_time;
	clock->accumulator += delta_time <= 0.25 ? delta_time : 0.25;

	if (clock->accumulator >= clock->target_frametime)
	{
		clock->accumulator -= clock->target_frametime;
		return 1;
	}

	return 0;
}

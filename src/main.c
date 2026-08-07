#include <stdio.h>

#include "chip8.h"
#include "audio.h"
#include "video.h"
#include "keypad.h"
#include "clock.h"
#include "file.h"

#define CYCLES_PER_FRAME 30


int main(void)
{
	const char* file = open_file();
	if (!file) return fprintf(stderr, "File not found\n"), -1;

	Chip8 chip8;
	Clock clock;
	chip8_init(&chip8);
	video_init(file);
	audio_init();
	clock_init(&clock, 60.0);
	chip8_load_rom(&chip8, file);

	while (1)
	{	
		if (keypad_poll(&chip8) == -1) break;

		if (clock_process_frame(&clock))
		{
			for (int i = 0; i < CYCLES_PER_FRAME; i++) chip8_cycle(&chip8);
		    if (chip8.delay_timer > 0) chip8.delay_timer--;
		    if (chip8.sound_timer > 0) chip8.sound_timer--;	
		}

		audio_update(&chip8);
		video_render(&chip8);
	}

	audio_close();
	video_close();
	return 0;
}

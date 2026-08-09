#include <stdio.h>

#include "chip8.h"
#include "audio.h"
#include "video.h"
#include "keypad.h"
#include "clock.h"
#include "file.h"

#define CYCLES_PER_FRAME 12


int main(void)
{
	const char* file = open_file();
	if (!file) return fprintf(stderr, "File not found\n"), -1;

	Chip8 chip8;
	Clock clock;
	video_init(file);
	audio_init();
	clock_init(&clock, 60.0);
	chip8_init(&chip8);
	Chip8Status rom_status = chip8_load_rom(&chip8, file);
	if (rom_status < 0) return fprintf(stderr, "Failed to load ROM: %s\n", chip8_status_str(rom_status)), -1;

	while (1)
	{	
		if (keypad_poll(&chip8) == -1) break;

		if (clock_process_frame(&clock))
		{
			for (int i = 0; i < CYCLES_PER_FRAME; i++)
			{
				Chip8Status cycle_status = chip8_cycle(&chip8);
				if (cycle_status < 0)
				{
				    fprintf(stderr, "Error: %s\n", chip8_status_str(cycle_status));
				    goto quit;
				}
			}
		    if (chip8.delay_timer > 0) chip8.delay_timer--;
		    if (chip8.sound_timer > 0) chip8.sound_timer--;
		}

		audio_update(&chip8);
		video_render(&chip8);
	}

quit:
	audio_close();
	video_close();
	return 0;
}

#include <stdio.h>
#include <unistd.h>

#include "chip8.h"
#include "audio.h"
#include "video.h"
#include "keypad.h"
#include "clock.h"

#define CYCLES_PER_FRAME 200

int main(int argc, char* argv[])
{
	if (argc < 2) return fprintf(stderr, "Usage: %s <path/to/rom>\n", argv[0]), -1;

	Chip8 chip8;
	Clock clock;
	chip8_init(&chip8);
	video_init();
	audio_init(&chip8);
	clock_init(&clock, 60.0);
	chip8_load_rom(&chip8, argv[1]);

	while (1)
	{	
		if (keypad_poll(&chip8) == -1) break;

		if (clock_process_frame(&clock))
		{
			for (int i = 0; i < CYCLES_PER_FRAME; i++) chip8_cycle(&chip8);
		    if (chip8.delay_timer > 0) chip8.delay_timer--;
		    if (chip8.sound_timer > 0) chip8.sound_timer--;	
		}

		video_render(&chip8);
	}

	video_close();
	audio_close();
	return 0;
}

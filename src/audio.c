#include "audio.h"

#include "chip8.h"

void audio_callback(void* userdata, Uint8* stream, int len)
{
	Chip8* chip8 = (Chip8*)userdata;
	Sint16* buffer = (Sint16*)stream;
	int num_samples = len/sizeof(Sint16);

	static double phase = 0.0;
	double step = (2 * M_PI * FREQUENCY) / SAMPLE_RATE;

	for (int i = 0; i < num_samples; i++)
	{
		if (chip8->sound_timer > 0)
		{
			buffer[i] = sin(phase) > 0 ? VOLUME : -VOLUME;
			phase += step;
			if (phase > 2 * M_PI) phase -= 2 * M_PI;
		}
		else
		{
			buffer[i] = 0;
		}
	}
}

int audio_init(void* userdata)
{

	if (SDL_Init(SDL_INIT_AUDIO) < 0) return fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError()), -1;

	SDL_AudioSpec desired, obtained;
	memset(&desired, 0, sizeof(desired));
	desired.freq = SAMPLE_RATE;
	desired.format = AUDIO_S16SYS;
	desired.channels = 1;
	desired.samples = 512;
	desired.callback = audio_callback;
	desired.userdata = userdata;

	if (SDL_OpenAudio(&desired, &obtained) < 0) return fprintf(stderr, "SDL_OpenAudio Error: %s\n", SDL_GetError()), -1;

	SDL_PauseAudio(0);
	return 0;
}


void audio_close(void)
{
	SDL_CloseAudio();
}

#include "audio.h"
#include "chip8.h"

#include <stdio.h>
#include <math.h>

#include <SDL3/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static SDL_AudioStream* audio_stream = NULL;

int audio_init(void)
{
	SDL_AudioSpec spec;
	spec.freq = SAMPLE_RATE;
	spec.format = SDL_AUDIO_S16;
	spec.channels = 1;

	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);

	SDL_ResumeAudioStreamDevice(audio_stream);
	return 0;
}

void audio_update(void* userdata)
{
	if (!audio_stream) return;
	
	Chip8* chip8 = (Chip8*)userdata;

	int queued_bytes = SDL_GetAudioStreamQueued(audio_stream);
	int target_bytes = (SAMPLE_RATE * sizeof(Sint16)) / 10;

	if (queued_bytes < target_bytes)
	{
		int num_samples = 512;
		Sint16 buffer[512];

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
				phase = 0.0;
			}
		}
		SDL_PutAudioStreamData(audio_stream, buffer, num_samples * sizeof(Sint16));
	}
}

void audio_close(void)
{
	if (audio_stream)
	{
		SDL_DestroyAudioStream(audio_stream);
		audio_stream = NULL;
	}
}

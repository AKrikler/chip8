#include "audio.h"
#include "chip8.h"

#include <stdio.h>
#include <math.h>

#include <SDL3/SDL.h>

#define MAX_BUFFER_SAMPLES 8192
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static SDL_AudioStream* audio_stream = NULL;
static uint64_t last_time = 0;
static double phase = 0.0;
static double sample_remainder = 0.0;

int audio_init(void)
{
    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_ResumeAudioStreamDevice(audio_stream);
    last_time = SDL_GetPerformanceCounter();
    return 0;
}

void audio_update(void* userdata)
{
    if (!audio_stream) return;
    Chip8* chip8 = (Chip8*)userdata;

    uint64_t current_time = SDL_GetPerformanceCounter();
    double elapsed = (double)(current_time - last_time) / SDL_GetPerformanceFrequency();
    last_time = current_time;

    double exact_samples = elapsed * SAMPLE_RATE + sample_remainder;
    int num_samples = (int)exact_samples;
    sample_remainder = exact_samples - num_samples;
    if (num_samples <= 0) return;
    if (num_samples > MAX_BUFFER_SAMPLES) num_samples = MAX_BUFFER_SAMPLES;

    Sint16 buffer[MAX_BUFFER_SAMPLES];
    for (int i = 0; i < num_samples; i++)
    {
        if (chip8->sound_timer > 0)
        {
            buffer[i] = sin(phase) > 0 ? VOLUME : -VOLUME;
            phase += (2 * M_PI * FREQUENCY) / SAMPLE_RATE;
            if (phase >= 2 * M_PI) phase -= 2 * M_PI;
        }
        else
        {
            buffer[i] = 0;
            phase = 0.0;
        }
    }
    SDL_PutAudioStreamData(audio_stream, buffer, num_samples * sizeof(Sint16));
}

void audio_close(void)
{
    if (audio_stream)
    {
        SDL_DestroyAudioStream(audio_stream);
        audio_stream = NULL;
    }
}


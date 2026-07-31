#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FREQUENCY 440.0
#define SAMPLE_RATE 44100
#define VOLUME 12000

void audio_callback(void* userdata, Uint8* stream, int len);
int audio_init(void* usderdata);
void audio_close(void);

#endif

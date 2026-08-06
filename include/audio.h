#ifndef AUDIO_H
#define AUDIO_H

#define FREQUENCY 440.0
#define SAMPLE_RATE 44100
#define VOLUME 12000

int audio_init(void);
void audio_update(void* userdata);
void audio_close(void);

#endif

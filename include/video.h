#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>

#define SCALE 8

int video_init(void);
void video_render(void* userdata);
void video_close(void);

#endif

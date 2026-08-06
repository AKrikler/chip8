#ifndef VIDEO_H
#define VIDEO_H

#define SCALE 8

int video_init(const char* path);
void video_render(void* userdata);
void video_close(void);

#endif

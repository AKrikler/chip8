#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct
{
	uint64_t last_time;
	double accumulator;
	double target_frametime;
} Clock;

void clock_init(Clock* clock, double fps);
int clock_process_frame(Clock* clock);

#endif

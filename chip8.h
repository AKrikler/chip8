#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_ROM_START 0x200

typedef struct
{
	uint8_t memory[4096];
	uint8_t regs[16];
	uint16_t ir;
	uint16_t pc;
	uint8_t display[64*32];
	uint16_t stack[16];
	uint8_t sp;
	uint8_t delay_timer;
	uint8_t sound_timer;
} Chip8;

void chip8_init(Chip8* chip8);
void chip8_load_rom(Chip8* chip8, const char* filename);
void chip8_cycle(Chip8* chip8);

#endif

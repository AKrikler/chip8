#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_ROM_START 512
#define CHIP8_ROM_END 4095

#define CHIP8_COORD(x, y) ((x) + (y) * CHIP8_DISPLAY_WIDTH)

typedef enum {
	CHIP8_ERR_OOB_MEMORY	  = -7,
	CHIP8_ERR_UNKNOWN_OPCODE  = -6,
	CHIP8_ERR_INVALID_ROM	  = -5,
	CHIP8_ERR_ROM_TOO_LARGE   = -4,
	CHIP8_ERR_FILE_NOT_FOUND  = -3,
    CHIP8_ERR_STACK_OVERFLOW  = -2,
    CHIP8_ERR_STACK_UNDERFLOW = -1,
    CHIP8_OK                  =  0,
    CHIP8_DREW_FRAME          =  1,
} Chip8Status;

typedef struct
{
	uint8_t memory[4096];
	uint8_t regs[16];
	uint16_t ir;
	uint16_t pc;
	uint8_t sp;
	uint16_t stack[16];
	uint8_t display[CHIP8_DISPLAY_WIDTH*CHIP8_DISPLAY_HEIGHT];
	uint8_t keypad[16];
	uint8_t delay_timer;
	uint8_t sound_timer;
} Chip8;

void chip8_init(Chip8* chip8);
Chip8Status chip8_load_rom(Chip8* chip8, const char* filename);
Chip8Status chip8_cycle(Chip8* chip8);
const char* chip8_status_str(Chip8Status status);

#endif

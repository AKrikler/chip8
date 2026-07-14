#include <stdio.h>

#include "chip8.h"

#define TEST_I 15
#define TEST_V 135

int main(void)
{
	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, "roms/test.ch8");

	for (int i = 0; i < 16; i++)
	{
	    chip8_cycle(&chip8);
	    printf("Cycle %d: V0=%d VA=%d VF=%d PC=%d\n", i, chip8.regs[0], chip8.regs[0xA], chip8.regs[0xF], chip8.pc);
	}
	
	return 0;
}

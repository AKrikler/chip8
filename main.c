#include <stdio.h>

#include "chip8.h"

#define TEST_I 15
#define TEST_V 135

int main(void)
{
	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, "roms/test.ch8");

	chip8.display[TEST_I] = TEST_V; 
	printf("Before Cycle: display[%d] = %d\n", TEST_I, chip8.display[TEST_I]);
	printf("PC: %d\n", chip8.pc);

	chip8_cycle(&chip8);
	printf("After Cycle: display[%d] = %d\n", TEST_I, chip8.display[TEST_I]);
	printf("PC: %d\n", chip8.pc);
	
	return 0;
}

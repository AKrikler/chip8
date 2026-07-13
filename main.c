#include <stdio.h>

#include "chip8.h"

int main(void)
{
	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, "roms/test.ch8");

	for (int i = 0; i < 8; i++) {
	    printf("%02X ", chip8.memory[CHIP8_ROM_START + i]);
	}
	printf("\n");
	
	return 0;
}

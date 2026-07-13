
#include "chip8.h"

#include <stdio.h>
#include <string.h>

void chip8_init(Chip8* chip8)
{
	memset(chip8->memory, 0, sizeof(chip8->memory));
	memset(chip8->regs, 0, sizeof(chip8->regs));
	chip8->ir = 0;
	chip8->pc = CHIP8_ROM_START;
	memset(chip8->display, 0, sizeof(chip8->display));
	memset(chip8->stack, 0, sizeof(chip8->stack));
	chip8->sp = 0;
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;
}

void chip8_load_rom(Chip8* chip8, const char* filename)
{
	FILE* rom_file = fopen(filename, "rb");
	if (rom_file == NULL)
	{
		perror("Failed to open ROM");
		return;
	}
	
	size_t max_rom_size = sizeof(chip8->memory) - CHIP8_ROM_START;
	fseek(rom_file, 0, SEEK_END);
	size_t file_size = ftell(rom_file);
	fseek(rom_file, 0, SEEK_SET);
	if (file_size > max_rom_size) {
	    fprintf(stderr, "ROM too large\nROM Size: %zu\nMax Size: %zu", file_size, max_rom_size);
	    return;
	}
	
	size_t bytes = fread(chip8->memory + CHIP8_ROM_START, sizeof(uint8_t), file_size, rom_file);
	if (bytes != file_size) {
	    fprintf(stderr, "Read error: expected %zu bytes, got %zu\n", file_size, bytes);
	    fclose(rom_file);
	    return;
	}
	
	fclose(rom_file);
}

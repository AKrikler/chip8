
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
	if (file_size > max_rom_size)
	{
		fprintf(stderr, "ROM too large\nROM Size: %zu\nMax Size: %zu\n", file_size, max_rom_size);
	    return;
	}
	
	size_t bytes = fread(chip8->memory + CHIP8_ROM_START, sizeof(uint8_t), file_size, rom_file);
	if (bytes != file_size)
	{
	    fprintf(stderr, "Read error: expected %zu bytes, got %zu\n", file_size, bytes);
	    fclose(rom_file);
	    return;
	}
	
	fclose(rom_file);
}

void chip8_cycle(Chip8* chip8)
{
	uint8_t high = chip8->memory[chip8->pc];
	uint8_t low = chip8->memory[chip8->pc+1];
	uint16_t opcode = high << 8 | low;
	chip8->pc += 2;

	switch (opcode >> 12)
	{
		case 0x0:
			if (opcode == 0x00E0)
			{
				memset(chip8->display, 0, sizeof(chip8->display));
			}
			else if (opcode == 0x00EE)
			{
				// 00EE RET
			}
			else
			{
				// 0nnn
			}
			break;
		case 0x1:
			// 1nnn JP addr
			break;
		case 0x2:
			// 2nnn CALL addr
			break;
		case 0x3:
			// 3xkk SE Vx, byte
			break;
		case 0x4:
			// 4xkk SNE Vx, byte
			break;
		case 0x5:
			// 5xy0 SE Vx, Vy
			break;
		case 0x6:
			// 6xkk LD Vx, byte
			break;
		case 0x7:
			// 7xkk ADD Vx, byte
			break;
		case 0x8:
			switch (opcode & 0xF)
			{
				case 0x0:
					break;
				case 0x1:
					break;
				case 0x2:
					break;
				case 0x3:
					break;
				case 0x4:
					break;
				case 0x5:
					break;
				case 0x6:
					break;
				case 0x7:
					break;
				case 0xE:
					break;					
			}
			break;
		case 0x9:
			// 9xy0 SNE Vx, Vy
			break;
		case 0xA:
			// Annn LD IR, addr
			break;
		case 0xB:
			// Bnnn JP V0, addr
			break;
		case 0xC:
			// Cxkk RND Vx, byte
			break;
		case 0xD:
			// Dxyn DRW Vx, Vy, nibble
			break;
		case 0xE:
			switch (opcode & 0xFF)
			{
				case 0x9E:
					break;
				case 0xA1:
					break;
			}
			break;
		case 0xF:
			switch (opcode & 0xFF)
			{
				case 0x07:
					break;
				case 0x0A:
					break;
				case 0x15:
					break;
				case 0x18:
					break;
				case 0x1E:
					break;
				case 0x29:
					break;
				case 0x33:
					break;
				case 0x55:
					break;
				case 0x65:
					break;
			}
			break;
	}
}

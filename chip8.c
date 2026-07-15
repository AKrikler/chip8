
#include "chip8.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
				chip8->pc = chip8->stack[--chip8->sp];
			}
			else
			{
				// 0nnn
			}
			break;
		case 0x1:
			chip8->pc = opcode & 0x0FFF;
			break;
		case 0x2:
			chip8->stack[chip8->sp++] = chip8->pc;
			chip8->pc = opcode & 0x0FFF;
			break;
		case 0x3:
			if (chip8->regs[opcode >> 8 & 0x0F] == (opcode & 0xFF))
			{
				chip8->pc += 2;
			}
			break;
		case 0x4:
			if (chip8->regs[opcode >> 8 & 0x0F] != (opcode & 0xFF))
			{
				chip8->pc += 2;
			}
			break;
		case 0x5:
			if (chip8->regs[opcode >> 8 & 0x0F] == chip8->regs[opcode >> 4 & 0x0F])
			{
				chip8->pc += 2;
			}
			break;
		case 0x6:
			chip8->regs[opcode >> 8 & 0x0F] = opcode & 0xFF;
			break;
		case 0x7:
			chip8->regs[opcode >> 8 & 0x0F] += opcode & 0xFF;
			break;
		case 0x8:
			switch (opcode & 0xF)
			{
				case 0x0:
					chip8->regs[opcode >> 8 & 0x0F] = chip8->regs[opcode >> 4 & 0x0F];
					break;
				case 0x1:
					chip8->regs[opcode >> 8 & 0x0F] |= chip8->regs[opcode >> 4 & 0x0F];
					break;
				case 0x2:
					chip8->regs[opcode >> 8 & 0x0F] &= chip8->regs[opcode >> 4 & 0x0F];
					break;
				case 0x3:
					chip8->regs[opcode >> 8 & 0x0F] ^= chip8->regs[opcode >> 4 & 0x0F];
					break;
				case 0x4:
					chip8->regs[0xF] = chip8->regs[opcode >> 8 & 0x0F] + chip8->regs[opcode >> 4 & 0xF] > 0xFF ? 1 : 0;
					chip8->regs[opcode >> 8 & 0x0F] += chip8->regs[opcode >> 4 & 0x0F];
					break;
				case 0x5:
					chip8->regs[0xF] = chip8->regs[opcode >> 8 & 0x0F] >= chip8->regs[opcode >> 4 & 0xF] ? 1 : 0;
					chip8->regs[opcode >> 8 & 0x0F] -= chip8->regs[opcode >> 4 & 0xF];
					break;
				case 0x6:
					chip8->regs[0xF] = chip8->regs[opcode >> 8 & 0x0F] & 0x1;
					chip8->regs[opcode >> 8 & 0x0F] >>= 1;
					break;
				case 0x7:
					chip8->regs[0xF] = chip8->regs[opcode >> 4 & 0xF] >= chip8->regs[opcode >> 8 & 0x0F] ? 1 : 0;
					chip8->regs[opcode >> 4 & 0xF] -= chip8->regs[opcode >> 8 & 0x0F];
					break;
				case 0xE:
					chip8->regs[0xF] = (chip8->regs[opcode >> 8 & 0x0F] & 0x80) >> 7;
					chip8->regs[opcode >> 8 & 0x0F] <<= 1;
					break;					
			}
			break;
		case 0x9:
			if (chip8->regs[opcode >> 8 & 0x0F] != chip8->regs[opcode >> 4 & 0x0F])
			{
				chip8->pc += 2;
			}
			break;
		case 0xA:
			chip8->ir = opcode & 0x0FFF;
			break;
		case 0xB:
			chip8->pc = chip8->regs[0x0] + (opcode & 0x0FFF);
			break;
		case 0xC:
			chip8->regs[opcode >> 8 & 0x0F] = (uint8_t)rand() & (opcode & 0xFF);
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

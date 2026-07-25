
#include "chip8.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COORD(x, y) ((x) + (y) * 64)

uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8* chip8)
{
	memset(chip8->memory, 0, sizeof(chip8->memory));
	memset(chip8->regs, 0, sizeof(chip8->regs));
	chip8->ir = 0;
	chip8->pc = CHIP8_ROM_START;
	chip8->sp = 0;
	memset(chip8->stack, 0, sizeof(chip8->stack));
	memset(chip8->display, 0, sizeof(chip8->display));
	memset(chip8->keypad, 0, sizeof(chip8->keypad));
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	memcpy(chip8->memory, fontset, sizeof(fontset));
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
	uint16_t opcode = (chip8->memory[chip8->pc] << 8) | (chip8->memory[chip8->pc+1]);
	chip8->pc += 2;

	printf("PC=%d opcode=0x%04X\n", chip8->pc, opcode);

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
					chip8->regs[opcode >> 8 & 0xF] = chip8->regs[opcode >> 4 & 0x0F] - chip8->regs[opcode >> 8 & 0x0F];
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
			{
				uint8_t vx = chip8->regs[opcode >> 8 & 0x0F];
				uint8_t vy = chip8->regs[opcode >> 4 & 0x0F];
				int collision = 0;

				for (int i = 0; i < (opcode & 0x0F); i++)
				{
					for (int j = 0; j < 8; j++)
					{
						int x = (vx + j) % 64;
						int y = (vy + i) % 32;
						uint8_t prev = chip8->display[COORD(x, y)];
						chip8->display[COORD(x, y)] ^= (chip8->memory[chip8->ir + i] >> (7 - j)) & 1;
						collision += prev == 1 && chip8->display[COORD(vx + j, vy + i)] != prev ? 1 : 0;
					}
				}
				chip8->regs[0xF] = collision ? 1 : 0;
				break;				
			}

		case 0xE:
			switch (opcode & 0xFF)
			{
				case 0x9E:
					if (chip8->keypad[opcode >> 8 & 0x0F])
					{
						chip8->pc += 2;
					}
					break;
				case 0xA1:
					if (!chip8->keypad[opcode >> 8 & 0x0F])
					{
						chip8->pc += 2;
					}
					break;
			}
			break;
		case 0xF:
			switch (opcode & 0xFF)
			{
				case 0x07:
					chip8->regs[opcode >> 8 & 0x0F] = chip8->delay_timer;
					break;
				case 0x0A:
				{
				    int pressed = -1;
				    for (int i = 0; i < 16; i++)
				    {
				        if (chip8->keypad[i])
				        {
				            pressed = i;
				            break;
				        }
				    }
				    if (pressed != -1) chip8->regs[(opcode & 0x0F00) >> 8] = pressed;
				    else chip8->pc -= 2; 
				    break;
				}
				case 0x15:
					chip8->delay_timer = chip8->regs[opcode >> 8 & 0x0F];
					break;
				case 0x18:
					chip8->sound_timer = chip8->regs[opcode >> 8 & 0x0F];
					break;
				case 0x1E:
					chip8->ir += chip8->regs[opcode >> 8 & 0x0F];
					break;
				case 0x29:
					chip8->ir = chip8->regs[opcode >> 8 & 0x0F] * 5;
					break;
				case 0x33:
				{
					uint8_t vx = chip8->regs[opcode >> 8 & 0x0F];
					uint8_t hundreds = vx / 100;
					uint8_t tens = (vx - hundreds * 100) / 10;
					uint8_t ones = (vx - hundreds * 100 - tens * 10);

					chip8->memory[chip8->ir + 0] = hundreds;
					chip8->memory[chip8->ir + 1] = tens;
					chip8->memory[chip8->ir + 2] = ones;
					break;
				}
				case 0x55:
					for (int i = 0; i <= (opcode >> 8 & 0x0F); i++)
					{
						chip8->memory[chip8->ir + i] = chip8->regs[i];
					}
					break;
				case 0x65:
					for (int i = 0; i <= (opcode >> 8 & 0x0F); i++)
					{
						chip8->regs[i] = chip8->memory[chip8->ir + i];
					}
					break;
			}
			break;
	}
}

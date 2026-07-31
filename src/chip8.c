#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t fontset[80] =
{
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
	memset(chip8, 0, sizeof(*chip8));
	chip8->pc = CHIP8_ROM_START;
	memcpy(chip8->memory, fontset, sizeof(fontset));
}

void chip8_load_rom(Chip8* chip8, const char* path)
{
	FILE* f = fopen(path, "rb");
	if (!f) return (void)(perror("Failed to open ROM"));

	long size = (fseek(f, 0, SEEK_END), ftell(f));
	rewind(f);
	
	if (size > 0 && (size_t)size <= sizeof(chip8->memory) - CHIP8_ROM_START) fread(chip8->memory + CHIP8_ROM_START, 1, size, f);
	else fprintf(stderr, "ROM (%zu bytes) too large for Chip8 (%zu bytes)\n", (size_t)size, sizeof(chip8->memory) - CHIP8_ROM_START);
	
	fclose(f);
}

void chip8_cycle(Chip8* chip8)
{
	uint16_t opcode = (chip8->memory[chip8->pc] << 8) | (chip8->memory[chip8->pc+1]);
	chip8->pc += 2;

	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t y = (opcode >> 4) & 0x0F;
	uint8_t n = opcode & 0x0F;
	uint8_t kk = opcode & 0xFF;
	uint16_t nnn = opcode & 0x0FFF;
	
	switch (opcode >> 12)
	{
		case 0x0:
			if (opcode == 0x00E0) memset(chip8->display, 0, sizeof(chip8->display));
			else if (opcode == 0x00EE) chip8->pc = chip8->stack[--chip8->sp];
			break;
		case 0x1: chip8->pc = nnn; break;
		case 0x2: chip8->stack[chip8->sp++] = chip8->pc, chip8->pc = nnn; break;
		case 0x3: if (chip8->regs[x] == kk) chip8->pc += 2; break;
		case 0x4: if (chip8->regs[x] != kk) chip8->pc += 2; break;
		case 0x5: if (chip8->regs[x] == chip8->regs[y]) chip8->pc += 2; break;
		case 0x6: chip8->regs[x] = kk; break;
		case 0x7: chip8->regs[x] += kk; break;
		case 0x8:
			switch (n)
			{
				case 0x0: chip8->regs[x] = chip8->regs[y]; break;
				case 0x1: chip8->regs[x] |= chip8->regs[y]; chip8->regs[0xF] = 0; break;
				case 0x2: chip8->regs[x] &= chip8->regs[y]; chip8->regs[0xF] = 0; break;
				case 0x3: chip8->regs[x] ^= chip8->regs[y]; chip8->regs[0xF] = 0; break;
				case 0x4:
				{
					uint16_t sum = chip8->regs[x] + chip8->regs[y];
					chip8->regs[x] = sum;
					chip8->regs[0xF] = sum > 0xFF;
					break;
				}
				case 0x5:
				{
					uint8_t borrow = chip8->regs[x] >= chip8->regs[y];
					chip8->regs[x] -= chip8->regs[y];
					chip8->regs[0xF] = borrow;
					break;
				}
				case 0x6:
				{
					uint8_t lsb = chip8->regs[x] & 0x1;
					chip8->regs[x] >>= 1;
					chip8->regs[0xF] = lsb;
					break;
				}
				case 0x7:
				{
					uint8_t borrow = chip8->regs[y] >= chip8->regs[x];
					chip8->regs[x] = chip8->regs[y] - chip8->regs[x];
					chip8->regs[0xF] = borrow;
					break;
				}
				case 0xE:
				{
					uint8_t msb = chip8->regs[x] >> 7;
					chip8->regs[x] <<= 1;
					chip8->regs[0xF] = msb;
					break;
				}
			}
			break;
		case 0x9: if (chip8->regs[x] != chip8->regs[y]) chip8->pc += 2; break;
		case 0xA: chip8->ir = nnn; break;
		case 0xB: chip8->pc = chip8->regs[0x0] + nnn; break;
		case 0xC: chip8->regs[x] = rand() & kk; break;
		case 0xD:
		{
			uint8_t vx = chip8->regs[x], vy = chip8->regs[y];
			int collision = 0;

			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					int x = (vx + j) % 64;
					int y = (vy + i) % 32;
					uint8_t prev = chip8->display[CHIP8_COORD(x, y)];
					chip8->display[CHIP8_COORD(x, y)] ^= (chip8->memory[chip8->ir + i] >> (7 - j)) & 1;
					collision += prev == 1 && chip8->display[CHIP8_COORD(x, y)] != prev ? 1 : 0;
				}
			}
			chip8->regs[0xF] = collision ? 1 : 0;
			break;				
		}
		case 0xE:
			if (kk == 0x9E && chip8->keypad[chip8->regs[x]]) chip8->pc += 2;
            if (kk == 0xA1 && !chip8->keypad[chip8->regs[x]]) chip8->pc += 2;
            break;
		case 0xF:
			switch (kk)
			{
				case 0x07: chip8->regs[x] = chip8->delay_timer; break;
				case 0x0A:
				{
					static int wait_for_release = -1;

					if (wait_for_release != -1)
					{
						if (chip8->keypad[wait_for_release] == 0)
				        {
				            chip8->regs[x] = (uint8_t)wait_for_release;
				            wait_for_release = -1;
				        } 
				        else 
				        {
				            chip8->pc -= 2;
				        }						
					}
					else
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
				
				        if (pressed != -1) 
				        {
				            wait_for_release = pressed;
				            chip8->pc -= 2;
				        } 
				        else 
				        {
				            chip8->pc -= 2;
				        }
					}

				    break;
				}
				case 0x15: chip8->delay_timer = chip8->regs[x]; break;
				case 0x18: chip8->sound_timer = chip8->regs[x]; break;
				case 0x1E: chip8->ir += chip8->regs[x]; break;
				case 0x29: chip8->ir = chip8->regs[x] * 5; break;
				case 0x33:
				{
					uint8_t val = chip8->regs[x];
					chip8->memory[chip8->ir + 0] = val / 100;
					chip8->memory[chip8->ir + 1] = (val / 10) % 10;
					chip8->memory[chip8->ir + 2] = val % 10;
					break;
				}
				case 0x55: 
				    memcpy(chip8->memory + chip8->ir, chip8->regs, x + 1); 
				    chip8->ir += (x + 1);
				    break;
				case 0x65: 
				    memcpy(chip8->regs, chip8->memory + chip8->ir, x + 1); 
				    chip8->ir += (x + 1); 
				    break;
			}
			break;
	}
}

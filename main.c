#include <stdio.h>

#include <unistd.h>

#include "chip8.h"

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

int main(void)
{
	Chip8 chip8;
	chip8_init(&chip8);
	chip8_load_rom(&chip8, "roms/animation.ch8");

	for (int i = 0; i < 20; i++)
	{
		chip8_cycle(&chip8);
		usleep(300000);

		clear();
		gotoxy(0,0);
		for (int j = 0; j < 32; j++)
		{
			for (int k = 0; k < 64; k++)
			{
				if (chip8.display[j * 64 + k] == 1)
				{
					printf("#");
				}
				else
				{
					printf(".");
				}
			}
			printf("\n");
		}
	}
	
	return 0;
}

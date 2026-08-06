#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct
{
	char name[32];
	uint16_t address;
} Label;

int is_reg(const char* str)
{
	return strlen(str) == 2 && str[0] == 'V' && isxdigit((unsigned char)str[1]);
}

int is_number(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!('0' <= str[i] && str[i] <= '9'))
		{
			return 0;
		}
	}
	return 1;
}

uint16_t find_label(Label* labels, int label_count, const char* name)
{
	for (int i = 0; i < label_count; i++)
	{
		if (strcmp(labels[i].name, name) == 0)
		{
			return labels[i].address;
		}
	}
	fprintf(stderr, "Unknown label: %s\n", name);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
	    fprintf(stderr, "Usage: %s <input.chip8> <output.ch8>\n", argv[0]);
	    return 1;
	}
	
	FILE* out = fopen(argv[2], "wb");

	int address = 512;

	FILE* fptr = fopen(argv[1], "r");
	Label labels[64];
	int label_count = 0;

	if (fptr != NULL)
	{
		char line[256];
		
		while (fgets(line, sizeof(line), fptr))
		{
			char* p = line;
			while (*p == ' ' || *p == '\t') p++;

			char* newline = strchr(p, '\n');
			if (newline != NULL) *newline = '\0';
			char* comment = strchr(p, ';');
			if (comment != NULL) *comment = '\0';

			if (strlen(p) == 0)
			{
				continue;
			}
			else if (p[strlen(p) - 1] == ':')
			{
				p[strlen(p)-1] = '\0';
				strncpy(labels[label_count].name, p, sizeof(labels[label_count].name));
				labels[label_count++].address = address;
			}
			else if (strncmp(p, "DB", 2) == 0)
			{
				int commas = 0;
				while (*p != '\0') commas += *p++ == ',' ? 1 : 0;
				address += commas + 1;
			}
			else
			{
				address += 2;
			}
		}

		rewind(fptr);
		
		while (fgets(line, sizeof(line), fptr))
		{
			char* p = line;
			while (*p == ' ' || *p == '\t') p++;
			
			char* newline = strchr(p, '\n');
			if (newline != NULL) *newline = '\0';
			char* comment = strchr(p, ';');
			if (comment != NULL) *comment = '\0';
		
			if (strlen(p) == 0)
			{
				continue;
			}
			else if (p[strlen(p) - 1] == ':')
			{
				continue;
			}
			else if (strncmp(p, "DB", 2) == 0)
			{
				char* token = strtok(p + 2, " ,");
				while (token != NULL)
				{
					uint8_t byte = (uint8_t)strtol(token, NULL, 0);
					fwrite(&byte, 1, 1, out);
					token = strtok(NULL, " ,");
				}
			}
			else
			{
				uint16_t opcode = 0;
				char* mnemonic = strtok(p, " ");

				if (strcmp(mnemonic, "CLS") == 0)
				{
					opcode = 0x00E0;
				}
				else if (strcmp(mnemonic, "RET") == 0)
				{
					opcode = 0x00EE;
				}
				else if (strcmp(mnemonic, "JP") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					
					if (strcmp(op1, "V0") == 0)
					{
						if (is_number(op2))
						{
							opcode = strtol(op2, NULL, 0);
						}
						else
						{
							opcode = find_label(labels, label_count, op2);
						}
						opcode = 0xB000 | (opcode & 0x0FFF);
					}
					else
					{
						if (is_number(op1))
						{
							opcode = strtol(op1, NULL, 0);
						}
						else
						{
							opcode = find_label(labels, label_count, op1);
						}
						opcode = 0x1000 | (opcode & 0x0FFF);
					}
				}
				else if (strcmp(mnemonic, "CALL") == 0)
				{
					char* op = strtok(NULL, " ,");
					opcode = is_number(op) ? strtol(op, NULL, 0) : find_label(labels, label_count, op);
					opcode = 0x2000 | (opcode & 0x0FFF);
				}
				else if (strcmp(mnemonic, "SE") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					if (is_reg(op1) && is_number(op2))
					{
						opcode = 0x3000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2, NULL, 0) & 0x00FF);
					}
					else if (is_reg(op1) && is_reg(op2))
					{
						opcode = 0x5000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
					}
				}
				else if (strcmp(mnemonic, "SNE") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					
					if (is_reg(op1) && is_number(op2))
					{
						opcode = 0x4000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2, NULL, 0) & 0x00FF);
					}
					else if (is_reg(op1) && is_reg(op2))
					{
						opcode = 0x9000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
					}
				}
				else if (strcmp(mnemonic, "LD") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					if (is_reg(op1) && is_number(op2))
					{
						opcode = 0x6000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2, NULL, 0) & 0x00FF);
					}
					else if (is_reg(op1) && is_reg(op2))
					{
						opcode = 0x8000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
					}
					else if (strcmp(op1, "I") == 0)
					{
						opcode = 0xA000 | (is_number(op2) ? strtol(op2, NULL, 0) : find_label(labels, label_count, op2) & 0x0FFF);
					}
					else if (is_reg(op1) && strcmp(op2, "DT") == 0)
					{
						opcode = 0xF007 | (strtol(op1 + 1, NULL, 16) << 8);
					}
					else if (is_reg(op1) && strcmp(op2, "K") == 0)
					{
						opcode = 0xF00A | (strtol(op1 + 1, NULL, 16) << 8);
					}
					else if (strcmp(op1, "DT") == 0 && is_reg(op2))
					{
						opcode = 0xF015 | (strtol(op2 + 1, NULL, 16) << 8);
					}
					else if (strcmp(op1, "ST") == 0 && is_reg(op2))
					{
						opcode = 0xF018 | (strtol(op2 + 1, NULL, 16) << 8);
					}
					else if (strcmp(op1, "F") == 0 && is_reg(op2))
					{
						opcode = 0xF029 | (strtol(op2 + 1, NULL, 16) << 8);
					}
					else if (strcmp(op1, "B") == 0 && is_reg(op2))
					{
						opcode = 0xF033 | (strtol(op2 + 1, NULL, 16) << 8);
					}
					else if (strcmp(op1, "[I]") == 0 && is_reg(op2))
					{
						opcode = 0xF055 | (strtol(op2 + 1, NULL, 16) << 8);
					}
					else if (is_reg(op1) && strcmp(op2, "[I]") == 0)
					{
						opcode = 0xF065 | (strtol(op1 + 1, NULL, 16) << 8);
					}
				}
				else if (strcmp(mnemonic, "ADD") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					if (is_reg(op1) && is_number(op2))
					{
						opcode = 0x7000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2, NULL, 0) & 0x00FF);
					}
					else if (is_reg(op1) && is_reg(op2))
					{
						opcode = 0x8004 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
					}
					else if (strcmp(op1, "I") == 0 && is_reg(op2))
					{
						opcode = 0xF01E | (strtol(op2 + 1, NULL, 16) << 8);
					}
				}
				else if (strcmp(mnemonic, "OR") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					
					opcode = 0x8001 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
				}
				else if (strcmp(mnemonic, "AND") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					
					opcode = 0x8002 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
				}
				else if (strcmp(mnemonic, "XOR") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					
					opcode = 0x8003 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
				}
				else if (strcmp(mnemonic, "SUB") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					opcode = 0x8005 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
				}
				else if (strcmp(mnemonic, "SHR") == 0)
				{
					opcode = 0x8006 | (strtol(strtok(NULL, " ,") + 1, NULL, 16) << 8);
				}
				else if (strcmp(mnemonic, "SUBN") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					opcode = 0x8007 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4);
				}
				else if (strcmp(mnemonic, "SHL") == 0)
				{
					opcode = 0x800E | (strtol(strtok(NULL, " ,") + 1, NULL, 16) << 8);
				}
				else if (strcmp(mnemonic, "RND") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");

					opcode = 0xC000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2, NULL, 0) & 0x00FF);
				}
				else if (strcmp(mnemonic, "DRW") == 0)
				{
					char* op1 = strtok(NULL, " ,");
					char* op2 = strtok(NULL, " ,");
					char* op3 = strtok(NULL, " ,");

					opcode = 0xD000 | (strtol(op1 + 1, NULL, 16) << 8) | (strtol(op2 + 1, NULL, 16) << 4) | (strtol(op3, NULL, 0) & 0x000F);
				}
				else if (strcmp(mnemonic, "SKP") == 0)
				{
					opcode = 0xE09E | (strtol(strtok(NULL, " ,") + 1, NULL, 16) << 8);
				}
				else if (strcmp(mnemonic, "SKPN") == 0)
				{
					opcode = 0xE0A1 | (strtol(strtok(NULL, " ,") + 1, NULL, 16) << 8);
				}
				
				uint8_t high = opcode >> 8;
				uint8_t low = opcode & 0xFF;
				fwrite(&high, 1, 1, out);
				fwrite(&low, 1, 1, out);
				address += 2;
			}
		}
		
	}

	fclose(fptr);
	fclose(out);
	return 0;
}

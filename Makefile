CC = @gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LIBS = -lSDL2 -lm

EMU_TARGET = chip8_emulator
ASM_TARGET = assembler

EMU_SRCS = $(wildcard src/*.c)
EMU_OBJS = $(patsubst src/%.c, obj/%.o, $(EMU_SRCS))

ASM_SRC = tools/assembler.c
ASM_OBJ = obj/assembler.o

all: $(EMU_TARGET) $(ASM_TARGET)

$(EMU_TARGET): $(EMU_OBJS)
	$(CC) $(EMU_OBJS) -o $@ $(LIBS)

$(ASM_TARGET): $(ASM_OBJ)
	$(CC) $(ASM_OBJ) -o $@

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: tools/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	@mkdir -p obj

clean:
	@rm -rf obj $(EMU_TARGET) $(ASM_TARGET)

.PHONY: all clean

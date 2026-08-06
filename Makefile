CC = @gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LIBS = -lSDL3 -lm

EMU_TARGET = chip8_emulator
ASM_TARGET = assembler

EMU_SRCS = $(wildcard src/*.c)
EMU_OBJS = $(patsubst src/%.c, obj/%.o, $(EMU_SRCS))

ASM_SRC = tools/assembler.c
ASM_OBJ = obj/assembler.o

WIN_CC = x86_64-w64-mingw32-gcc
WIN_DEST = /mnt/c/Users/$(USER)/Downloads
WIN_EMU_TARGET = $(WIN_DEST)/chip8_emulator.exe 
WIN_ASM_TARGET = $(WIN_DEST)/assembler.exe
WIN_LIBS = -lmingw32 -lSDL3 -lm -mwindows

SDL_DEPS_DIR = deps/sdl-mingw
SDL_VERSION = 3.4.14
SDL_WIN_INC = $(SDL_DEPS_DIR)/SDL3-$(SDL_VERSION)/x86_64-w64-mingw32/include
SDL_WIN_LIB = $(SDL_DEPS_DIR)/SDL3-$(SDL_VERSION)/x86_64-w64-mingw32/lib

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

$(SDL_DEPS_DIR):
	@echo "Downloading Windows SDL3 development libraries..."
	@mkdir -p deps
	@curl -L https://github.com/libsdl-org/SDL/releases/download/release-$(SDL_VERSION)/SDL3-devel-$(SDL_VERSION)-mingw.tar.gz -o deps/sdl3.tar.gz
	@mkdir -p $(SDL_DEPS_DIR)
	@tar -xzf deps/sdl3.tar.gz -C $(SDL_DEPS_DIR)
	@rm deps/sdl3.tar.gz

windows: $(SDL_DEPS_DIR) $(WIN_DEST)
	$(WIN_CC) src/*.c -Wall -Wextra -std=c99 -Iinclude -I$(SDL_WIN_INC) -L$(SDL_WIN_LIB) -o $(WIN_EMU_TARGET) $(WIN_LIBS)

$(WIN_DEST):
	mkdir -p $(WIN_DEST)

clean:
	@rm -rf obj $(EMU_TARGET) $(ASM_TARGET)

.PHONY: all clean windows

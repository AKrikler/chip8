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

SDL_DEPS_DIR = deps/sdl-source
SDL_VERSION = 3.4.14
SDL_WIN_INC = $(SDL_DEPS_DIR)/include
WIN_STATIC_LIB = $(SDL_DEPS_DIR)/build-static/libSDL3.a
WIN_LIBS = $(WIN_STATIC_LIB) -lmingw32 -lm -mwindows -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -lsetupapi -lhid -ldxguid -DSDL_STATIC_LIB

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
	@echo "Downloading full SDL3 source code for static compilation..."
	@mkdir -p deps
	@curl -L https://github.com/libsdl-org/SDL/releases/download/release-$(SDL_VERSION)/SDL3-$(SDL_VERSION).tar.gz -o deps/sdl3_src.tar.gz
	@mkdir -p $(SDL_DEPS_DIR)
	@tar -xzf deps/sdl3_src.tar.gz -C $(SDL_DEPS_DIR) --strip-components=1
	@rm deps/sdl3_src.tar.gz

$(WIN_STATIC_LIB): $(SDL_DEPS_DIR)
	@echo "Cross-compiling SDL3 static library..."
	@mkdir -p $(SDL_DEPS_DIR)/build-static
	@cd $(SDL_DEPS_DIR)/build-static && \
	cmake .. \
		-DCMAKE_SYSTEM_NAME=Windows \
		-DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
		-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_SHARED_LIBS=OFF \
		-DSDL_STATIC=ON \
		-DSDL_SHARED=OFF \
		-DSDL_TEST_ENABLED=OFF
	@cmake --build $(SDL_DEPS_DIR)/build-static -j4

windows: $(SDL_DEPS_DIR) $(WIN_STATIC_LIB) $(WIN_DEST)
	$(WIN_CC) src/*.c -Wall -Wextra -std=c99 -Iinclude -I$(SDL_WIN_INC) -o $(WIN_EMU_TARGET) $(WIN_LIBS)

$(WIN_DEST):
	mkdir -p $(WIN_DEST)

clean:
	@rm -rf obj deps $(EMU_TARGET) $(ASM_TARGET)

.PHONY: all clean windows

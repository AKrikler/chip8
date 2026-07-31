# CHIP-8 Emulator & Assembler

A cross-platform CHIP-8 emulator and assembler written in C (`-std=c99`), with hardware rendering via SDL2, audio, and input handling.

## Features
* **CHIP-8 Interpreter:** Full implementation of standard instructions, ROM loading, and memory management.
* **Custom Assembler:** Assembler to compile custom assembly scripts into executable CHIP-8 ROMs.
* **Cross-Platform:** Built to run natively on Linux and target Windows via cross-compilation with MinGW.
* **Audio & Video:** Real-time sound generation and display rendering driven by SDL2.

---

## Project Structure
```text
.
├── include/         # Header files (audio, keypad, video, chip8)
├── src/             # Emulator implementation files
├── tools/           # Assembler and auxiliary tools
└── Makefile         # Build automation script

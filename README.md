# CHIP-8 Emulator

This project is an implementation of a **CHIP-8 emulator**, created as an exploration of emulator design, low-level systems programming, and CPU instruction execution.

## Overview
CHIP-8 is a simple, interpreted programming language used on early microcomputers. This emulator recreates the CHIP-8 virtual machine, including its instruction set, memory model, registers, timers, and display logic.

The project focuses on accurately emulating the CHIP-8 execution cycle while maintaining clear and modular code.

## Features
- Implementation of the CHIP-8 instruction set
- 4KB memory model
- 16 general-purpose registers (V0–VF)
- Stack and program counter management
- Timers (delay and sound)
- Keyboard input handling
- 64×32 monochrome display rendering

## Tech Stack
- **C** (or update to C++ if applicable)
- **SDL / GLFW / terminal rendering** (update based on implementation)
- **Linux / macOS**

## Emulation Cycle
1. Fetch opcode from memory at the program counter
2. Decode instruction
3. Execute instruction
4. Update timers

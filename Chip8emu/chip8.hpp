//
//  chip8.hpp
//  Chip8emu
//
//  Created by Ruijing Li on 9/4/18.
//  Copyright © 2018 Ruijing. All rights reserved.
//

#ifndef chip8_hpp
#define chip8_hpp

#include <stdio.h>
#include <iostream>

class Chip8
{
    void clearScreen();
public:
    bool drawFlag;
    void initialize();
    void loadGame(const char *);
    void emulateCycle();
    void setKeys();
};

// The Chip 8 has 35 opcodes which are all two bytes long.
unsigned short opcode;
// The Chip 8 has 4K memory
unsigned char memory[4096];
// No interrupt or hardware registers
// 2 timer registers count at 60 Hz. count down to zero
unsigned char delay_timer;
//system buzzer sounds whenever sound timer reaches zero
unsigned char sound_timer;
unsigned char chip8_fontset[80] =
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

/* CPU registers: The Chip 8 has 15 8-bit general purpose registers named V0,V1 up to VE. The 16th register is used  for the ‘carry flag’. Eight bits is one byte
 */
unsigned char V[16];
// Index register I with value from 0x000 to 0xFFF
unsigned short I;
// program counter (pc) with value from 0x000 to 0xFFF
unsigned short pc;

/*
 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 0x200-0xFFF - Program ROM and work RAM
 */

/* instruction draws sprite to screen, done in XOR mode and if pixel turned off,
 VF register set. Collision detection
 */

// graphics are b&w and screen has total of 2048 pixels with state (0, 1)
unsigned char gfx[64*32];


// stack used to remember current location before jump is performed
// perform jump or call subroutine, store pc in stack
unsigned short stack[16];
// stack pointer remembers which of 16 levels of stack is used
unsigned short sp;

// Chip 8 HEX based keypad (0x0 - 0xF)
unsigned char key[16];
#endif /* chip8_hpp */



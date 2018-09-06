//
//  chip8.cpp
//  Chip8emu
//
//  Created by Ruijing Li on 9/4/18.
//  Copyright © 2018 Ruijing. All rights reserved.
//

#include "chip8.hpp"

void Chip8::initialize()
{
    // Initialize registers and memory once
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    
}

void Chip8::emulateCycle()
{
    // Fetch Opcode
    /* system will fetch 1 opcode from memory at loc specified by pc
     * data is stored in array in which each address contains 1 byte
     * fetch 2 sucessive bytes and merge
     */
    opcode = memory[pc] << 8 | memory[pc+1];
    
    // Decode Opcode
    // check the opcode table to see what it means.
    
    // Execute Opcode
    /*
     * Because every instruction is 2 bytes long, we need to increment the program counter by two after
     * every executed opcode. This is true unless you jump to a certain address in the memory or if you
     * call a subroutine (in which case you need to store the program counter in the stack). If the next
     * opcode should be skipped, increase the program counter by four.
     */
    // Update timers
    /* both timers count down to zero if they have been set to a value larger than zero. Since these timers count down at 60 Hz, you might want to implement something that slows down your emulation cycle (Execute 60 opcodes in one second).
     */
}

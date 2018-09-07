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
    
    pc = 0x200; // program counter starts at 0x200
    opcode = 0; // reset curr opcode
    I = 0; // reset index register
    sp = 0; // reset stack pointer
    
    // Clear display
    for(int i = 0; i < 64*32; ++i)
        gfx[i] = 0;
    
    for(int i = 0; i < 16; ++i)
    {
        stack[i] = 0;    // Clear stack
        V[i] = 0; // Clear registers V0-VF
    }
    
    // Clear memory (start at 80, since later will load fontset)
    for(int i =  80; i < 4096; ++i)
        memory[i] = 0;
    
    // Load fontset
    // This fontset should be loaded in memory location 0x50 == 80
    for(int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];
    
    // Reset Timers
    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::loadGame(const char * filename)
{
    /*
     load the program into the memory (use fopen in binary mode) and start filling the memory at
     location: 0x200 == 512.
     */
    unsigned int bufferSize = 4096 - 512; // because chip8 progs use 0x200 to 0xFFF
    unsigned char buffer[bufferSize];
    FILE *pfile = fopen(filename, "rb");
    if(!pfile)
    {
        std::cout << "Could not open file " << filename;
        return;
    }
    size_t bytesRead = fread(buffer, sizeof(char), bufferSize, pfile);
    if(bytesRead == 0)
    {
        std::cout << "Problem reading file";
        return;
    }
    fclose(pfile);
    
    for(int i = 0; i < bufferSize; ++i)
        memory[i+512] = buffer[i];
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
    switch(opcode & 0xF000)
    {
        case 0xA000: // ANNN: Sets I to address NNN
            // Execute Opcode
            I = opcode & 0x0FFF;
            /*
             * Because every instruction is 2 bytes long, we need to increment the program counter by
             * two after
             * every executed opcode. This is true unless you jump to a certain address in the memory or
             * if you
             * call a subroutine (in which case you need to store the program counter in the stack). If
             * the next
             * opcode should be skipped, increase the program counter by four.
             */
            pc += 2;
            break;
            
        case 0x0000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears screen
                    break;
                    
                case 0x000E: //0x00EE: Returns from subroutine
                    break;
                    
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
            
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
    }


    // Update timers
    /* both timers count down to zero if they have been set to a value larger than zero. Since these timers count down at 60 Hz, you might want to implement something that slows down your emulation cycle (Execute 60 opcodes in one second).
     */
    if(delay_timer > 0)
        --delay_timer;
    
    if(sound_timer > 0)
    {
        if(sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void Chip8::setKeys()
{
}

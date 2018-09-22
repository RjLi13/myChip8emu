//
//  chip8.cpp
//  Chip8emu
//
//  Created by Ruijing Li on 9/4/18.
//  Copyright © 2018 Ruijing. All rights reserved.
//

#include "chip8.hpp"

Chip8::Chip8()
{
    initialize();
}

Chip8::~Chip8()
{
    // Don't see an use case for this, so blank for now
}

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
    // This fontset should be loaded in memory location 0x50 == 80
    // there is a reason this starts at mem address 0 (for opcode fx29)
    // cpu code probably does not have to be in memory either
    for(int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];
    
    // Reset Timers
    delay_timer = 0;
    sound_timer = 0;
}

bool Chip8::loadGame(const char * filename)
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
       std::cout << "Could not open file " << filename << std::endl;
        return 1;
    }
    size_t bytesRead = fread(buffer, sizeof(char), bufferSize, pfile);
    if(bytesRead == 0)
    {
       std::cout << "Problem reading file" << std::endl;
        return 1;
    }
    fclose(pfile);
    
    for(int i = 0; i < bufferSize; ++i)
        memory[i+512] = buffer[i];
    
    return 0;
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
        
        case 0x1000: // 1NNN: jumps to address NNN
            pc = opcode & 0x0FFF;
            break;
            
        case 0x2000: // 2NNN: Calls subroutine at NNN
            stack[sp] = pc; // store current address
            ++sp; // increase sp to avoid overwriting stack
            pc = opcode & 0x0FFF; // set pc to NNN (jump)
            break;
        
        case 0x3000: // 3XNN: Skips the next instruction if VX equals NN.
            // shift by 8 to get X (shift is bits, hex is nibble)
            if ( V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF) )
                pc += 2; // skips
            
            pc += 2;
            break;
            
        case 0x4000: // Skips next instruction if VX not equals NN
            if ( V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 2; // skips
            
            pc += 2;
            break;
            
        case 0x5000: // Skips the next instruction if VX equals VY.
            if ( (opcode & 0x000F) != 0)
                goto UNKNOWNOP;
            
            if ( V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 2;
            
            pc += 2;
            break;
        
        case 0x6000: // Sets VX to NN.
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
            
        case 0x7000: //Adds NN to VX
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;
            
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 8XY0
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                
                case 0x0001: // 8XY1
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0002: // 8XY2
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0003: // 8XY3
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                
                case 0x0004: // 8XY4
                    // solve case of carry (if sum is greater than FF)
                    if ( V[(opcode & 0x0F00) >> 8] > (0xFF - V[opcode & 0x00F0]) )
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0005: // 8XY5
                    if ( V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0006: // 8XY6
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x0F);
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                    
                case 0x0007: // 8XY7
                    if ( V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x000E: // 8XYE
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0xF0) >> 4;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                    
                default:
                    printf ("Unknown opcode for 8 [0x0000]: 0x%X\n", opcode);
            }
            break;
            
        case 0x9000:
            if ( (opcode & 0x000F) != 0)
                goto UNKNOWNOP;
            
            if ( V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 2;
            
            pc += 2;
            break;

        case 0xB000:
            pc = V[0] + (opcode & 0x0FFF);
            break;
            
        case 0xC000:
        {
            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(0,255); // [0,255]
            V[(opcode & 0x0F00) >> 8] = distribution(generator) & (opcode & 0x00FF);
            pc += 2;
            break;
        }
            
        case 0xD000:
        {
            /*
             Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
             */
            // 1 pixel = 1 bit
            // The state of each pixel is set by using a bitwise XOR operation
            // This means that it will compare the current pixel state with the current value in the memory. If the current value is different from the value in the memory, the bit value will be 1. If both values match, the bit value will be 0.
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;
            V[0xF] = 0;
            
            // loop over each row
            for (int yline = 0; yline < height; ++yline)
            {
                // fetch pixel value from memory starting at I
                pixel = memory[I + yline];
                // loop over 8 pixels
                for (int xline = 0; xline < 8; ++xline)
                {
                    // check if current evaluated pixel is set to 1 (0x80 >> xline scans through byte 1 bit at a time)
                    if ( (pixel & (0x80 >> xline)) != 0)
                    {
                        // check if pixel on display is set to 1
                        if (gfx[x + xline + ( (y + yline) * 64)] == 1)
                            V[0xF] = 1; // collision
                        
                        //set pixel value
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
            
        case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x009E: // EX9E
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 2;
                        
                    pc += 2;
                    break;
                
                case 0x00A1:
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 2;
                    
                    pc += 2;
                    break;
                    
                default:
                    printf ("Unknown opcode for E [0x0000]: 0x%X\n", opcode);
            }
          break;
            
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                    
                case 0x000A:
                    // blocking
                    while(1)
                    {
                        for (int i = 0; i < 0xF; ++i)
                        {
                            if (key[i] == 1)
                            {
                                V[(opcode & 0x0F00) >> 8] = i;
                                goto OUT;
                            }
                        }
                    }
                OUT:
                    pc += 2;
                    break;
                    
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x001E:
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    I = V[(opcode & 0x0F00) >> 8] * 0x5; // Multiply by 5 because in memory array
                                                        // each digit font spans 5 and has location 5*digit
                                                        // see load fontset in initialize
                    pc += 2;
                    break;
                    
                case 0x0033:
                    // take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                    
                case 0x0055:
                {
                    int j = I;
                    for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i)
                        memory[j++] = V[i];
                    
                    pc += 2;
                    break;
                }
                    
                case 0x0065:
                {
                    int j = I;
                    for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i)
                        V[i] = memory[j++];
                    
                    pc += 2;
                    break;
                }
                default:
                    printf ("Unknown opcode for F [0x0000]: 0x%X\n", opcode);
            }
          break;
            
        case 0x0000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears screen
                    if ( (opcode & 0x00F0) != 0x00E0)
                        goto UNKNOWNZERO;
                        
                    // Clear display
                    for(int i = 0; i < 64*32; ++i)
                        gfx[i] = 0;
                    
                    pc += 2;
                    break;
                    
                case 0x000E: //0x00EE: Returns from subroutine
                    --sp; // go to past stack level
                    pc = stack[sp];
                    pc += 2;
                    break;
                
                UNKNOWNZERO:
                default:
                    printf ("Unknown opcode for 0 [0x0000]: 0x%X\n", opcode);
            }
            break;
            
        UNKNOWNOP:
        default:
            printf ("Unknown opcode for all: 0x%X\n", opcode);
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


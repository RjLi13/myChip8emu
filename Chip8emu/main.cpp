//
//  main.cpp
//  Chip8emu
//
//  Created by Ruijing Li on 9/4/18.
//  Copyright © 2018 Ruijing. All rights reserved.
//

#include <iostream>
#include <GLUT/GLUT.h> // OpenGL graphics and input
#include "chip8.hpp" // Your cpu core implementation

// class to handle opcodes
Chip8 myChip8;
void setupGraphics();
void setupInput();
void drawGraphics();

int main(int argc, const char * argv[])
{
    // Set up render system (window size, display mode, etc) -- GLUT
    setupGraphics();
    // register input callbacks (bind callbacks)
    setupInput();
    
    // Intialize Chip8 system (clear the memory, registers, and screen)
    myChip8.initialize();
    // Load game into memory (through copy)
    myChip8.loadGame("Pong");
    
    //Emulation Loop
    for(;;)
    {
        // Emulate one cycle
        myChip8.emulateCycle();
        
        /* If drawFlag is set, update screen
         * because does not draw every cycle
         * only 2 opcodes set flag
         * 0x00E0 - clears screen
         * 0xDXYN - draws sprite on screen
         */
        if(myChip8.drawFlag)
            drawGraphics();
            
        // Store key press state (press and release) in part that emulates keypad
        myChip8.setKeys();
    }
    return 0;
}

void setupGraphics()
{
}

void setupInput()
{
}

void drawGraphics()
{
}

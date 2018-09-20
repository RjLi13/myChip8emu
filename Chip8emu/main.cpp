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

// Display size
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// class to handle opcodes
Chip8 myChip8;
// modifier is likely to make the resolution actually seeable
int modifier = 10;


// window size
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

void display();
void reshape_window(GLsizei, GLsizei); // GLsizei is OPENGL int (to maintain 32 bits)
void keyboardUp(unsigned char, int, int);
void keyboardDown(unsigned char, int, int);

// Use new drawing method
#define DRAWWITHTEXTURE
typedef unsigned char u8; // define u8 as unsigned char
u8 screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3];
void setupTexture();

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("Usage: myChip8 chip8application\n\n");
        return 1;
    }
    
    // Load game
    if(!myChip8.loadGame(argv[1]))
        return 1;
    
    // Setup OPENGL
    glutInit(&argc, argv); // sets up program for glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // GLUT_DOUBLE - creates double buffered window
                                                // GLUT_RGBA - RGBA mode window
    glutInitWindowSize(display_width, display_height); // sets size
    glutInitWindowPosition(320, 320); //sets x, y location in pixels
    glutCreateWindow("Ruijing Li's Chip8");
    glutDisplayFunc(display); // display callback.
                            // called when window is to be redisplayed, normal plane is use
                            // must displayFunc before any window is shown
    glutIdleFunc(display); // callback for background processing tasks or continous animation
    glutReshapeFunc(reshape_window); // callback when window reshaped or when first shown
    glutKeyboardFunc(keyboardDown); // callback for when user presses button on keyboard
    glutKeyboardUpFunc(keyboardUp); // callback for when user releases key press
    
#ifdef DRAWWITHTEXTURE
    setupTexture(); // setup the new graphics method if can handle
#endif
    
    glutMainLoop(); // starts event processing loop, emulation loop
    return 0;
}

// Setup Texture
void setupTexture()
{
    // Clear screen
    for(int y = 0; y < SCREEN_HEIGHT; ++y)
        for(int x = 0; x < SCREEN_WIDTH; ++x)
            screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;
    
    // Create a texture
    // specifices target texture
    // specifies level of detail, 0 is base image
    // specifies color components
    // specifies dimensions
    // magic number
    // specifies format of pixel data
    // specifies data type of pixel
    // specifies pointer to image data in memory. GLvoid is void.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);
    
    // Set up the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // sets GL_TEXTURE_MAG_FILTER = GL_NEAREST for
                                                                    // texture mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    // Enable textures
    glEnable(GL_TEXTURE_2D); // enable texture mode if computer supports
}

/* new drawing method */
void updateTexture(const Chip8& c8)
{
    // Update pixels
    for(int y = 0; y < 32; ++y)
        for(int x = 0; x < 64; ++x)
            if(c8.gfx[(y * 64) + x] == 0)
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;    // Disabled
            else
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled
    
    // Update Texture
    // specifies texture subimage
    // same args as create texture except for x and y offset but no magic number
    glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);
    
    // draw the window or actual pixels for sprite
    glBegin( GL_QUADS ); // specifies primitives created from vertices
        // gltextcoord sets texture coordinates, glvertex specifies coordinates of vertex
        glTexCoord2d(0.0, 0.0);     glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0);     glVertex2d(display_width, 0.0);
        glTexCoord2d(1.0, 1.0);     glVertex2d(display_width, display_height);
        glTexCoord2d(0.0, 1.0);     glVertex2d(0.0, display_height);
    glEnd();
}

// Old gfx code
void drawPixel(int x, int y)
{
    glBegin(GL_QUADS);
        glVertex3f((x * modifier) + 0.0f,     (y * modifier) + 0.0f,     0.0f);
        glVertex3f((x * modifier) + 0.0f,     (y * modifier) + modifier, 0.0f);
        glVertex3f((x * modifier) + modifier, (y * modifier) + modifier, 0.0f);
        glVertex3f((x * modifier) + modifier, (y * modifier) + 0.0f,     0.0f);
    glEnd();
}

/* Old drawing method */
void updateQuads(const Chip8& c8)
{
    // Loop through graphics array
    for(int y = 0; y < 32; ++y)
        for(int x = 0; x < 64; ++x)
        {
            if(c8.gfx[(y*64) + x] == 0)
                glColor3f(0.0f,0.0f,0.0f); // set color of pixel (RGB)
            else
                glColor3f(1.0f,1.0f,1.0f);
            
            drawPixel(x, y);
        }
}

/* This seems to be the emulation loop body */
void display()
{
    myChip8.emulateCycle(); // emulate one cycle
    /* If drawFlag is set, update screen
      * because does not draw every cycle
      * only 2 opcodes set flag
      * 0x00E0 - clears screen
      * 0xDXYN - draws sprite on screen
      */
    if (myChip8.drawFlag)
    {
        // draw graphics
        
        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT); // sets buffer to glClearColor values.
        
#ifdef DRAWWITHTEXTURE
        updateTexture(myChip8); // draw with textures
#else
        updateQuads(myChip8); // draw with old api
#endif
        
        // Swap buffers!
        glutSwapBuffers(); // swap back layer with front layer if double buffered
                        // contents of back buffer is undefined
                        // in other words, buffer is frame so current frame is done with, work on next frame
        
        // Processed frame (reset drawFlag until switched on)
        myChip8.drawFlag = false;
    }
}

void reshape_window(GLsizei w, GLsizei h)
{
    /* My guess is it resets colors
     * then does some matrix operations needed
     * for graphics to resize
     * Note it targets projection first resizing possibly the window
     * Then targets the actual graphics display and resizes
     */
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f); // specify red, green, blue, alpha values when color buffers cleared
    glMatrixMode(GL_PROJECTION); // specify matrix mode for matrix ops
                                // Applies subsequent matrix operations to the projection matrix stack.
    glLoadIdentity(); // replaces current matrix with identity matrix
    gluOrtho2D(0, w, h, 0); // define 2d orthographic matrix for 2d ortho viewing region
                            // left, right, bottom, top coordinates for clipping planes
    glMatrixMode(GL_MODELVIEW); // Applies subsequent matrix operations to the modelview matrix stack.
    glViewport(0, 0, w, h); // x, y specify lower left corner of viewport, w,h for dimensions
                            // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glViewport.xml
                            // url for some math equations used to resize viewport
                            // viewport is users visible area
    // Resize quad
    display_width = w;
    display_height = h;
}

/* Store key press (setKeys part 1) */
void keyboardDown(unsigned char key, int x, int y)
{
    if(key == 27)    // esc
        exit(0);
    
    // Key mapping:
    /*
     Keypad                   Keyboard
     +-+-+-+-+                +-+-+-+-+
     |1|2|3|C|                |1|2|3|4|
     +-+-+-+-+                +-+-+-+-+
     |4|5|6|D|                |Q|W|E|R|
     +-+-+-+-+       =>       +-+-+-+-+
     |7|8|9|E|                |A|S|D|F|
     +-+-+-+-+                +-+-+-+-+
     |A|0|B|F|                |Z|X|C|V|
     +-+-+-+-+                +-+-+-+-+
     */
    
    if(key == '1')         myChip8.key[0x1] = 1;
    else if(key == '2')    myChip8.key[0x2] = 1;
    else if(key == '3')    myChip8.key[0x3] = 1;
    else if(key == '4')    myChip8.key[0xC] = 1;
    
    else if(key == 'q')    myChip8.key[0x4] = 1;
    else if(key == 'w')    myChip8.key[0x5] = 1;
    else if(key == 'e')    myChip8.key[0x6] = 1;
    else if(key == 'r')    myChip8.key[0xD] = 1;
    
    else if(key == 'a')    myChip8.key[0x7] = 1;
    else if(key == 's')    myChip8.key[0x8] = 1;
    else if(key == 'd')    myChip8.key[0x9] = 1;
    else if(key == 'f')    myChip8.key[0xE] = 1;
    
    else if(key == 'z')    myChip8.key[0xA] = 1;
    else if(key == 'x')    myChip8.key[0x0] = 1;
    else if(key == 'c')    myChip8.key[0xB] = 1;
    else if(key == 'v')    myChip8.key[0xF] = 1;
    
    //printf("Press key %c\n", key);
}

/* Release key press (setKeys part 2) */
void keyboardUp(unsigned char key, int x, int y)
{
    if(key == '1')         myChip8.key[0x1] = 0;
    else if(key == '2')    myChip8.key[0x2] = 0;
    else if(key == '3')    myChip8.key[0x3] = 0;
    else if(key == '4')    myChip8.key[0xC] = 0;
    
    else if(key == 'q')    myChip8.key[0x4] = 0;
    else if(key == 'w')    myChip8.key[0x5] = 0;
    else if(key == 'e')    myChip8.key[0x6] = 0;
    else if(key == 'r')    myChip8.key[0xD] = 0;
    
    else if(key == 'a')    myChip8.key[0x7] = 0;
    else if(key == 's')    myChip8.key[0x8] = 0;
    else if(key == 'd')    myChip8.key[0x9] = 0;
    else if(key == 'f')    myChip8.key[0xE] = 0;
    
    else if(key == 'z')    myChip8.key[0xA] = 0;
    else if(key == 'x')    myChip8.key[0x0] = 0;
    else if(key == 'c')    myChip8.key[0xB] = 0;
    else if(key == 'v')    myChip8.key[0xF] = 0;
}



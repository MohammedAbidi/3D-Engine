#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "pnm_display.h"
#include "rd_display.h"
#include "rd_error.h"


static const bool DEBUG_MODE = false;

static const float BYTE_CONVERSION = 255; // colors are from 0.0 to 1.0, bytes are from 0 to 255, so just scale by 255

std::vector<std::vector<char>> screen; // 2D vector representation of the ppm file
char backR = 0, backG = 0, backB = 0; // background color saved as seperate chars for easy setting
int frameNumberPPM; // Saved value for the frame number
int triple_xSize; // Only exists so the computer doesnt have to keep multiplying size by 3 every time

// Where everything for initially setting up a device is done.
// The global values display_xSize and display_ySize in rd_display.h are guaranteed to have valid values at this point.
// This is where the 2D vector is dynamically allocated to temporarily hold the image.
int pnm_init_display() {
    if (DEBUG_MODE) std::cout << "P1. DISPLAY:\n"
        << display_xSize << std::endl
        << display_ySize << std::endl;

    // Initialize 2D vector
    screen.resize(display_ySize);
    screen.shrink_to_fit();
    triple_xSize = display_xSize * 3;
    for (std::vector<char>& screenLine : screen) {
        screenLine.resize(triple_xSize);
        screenLine.shrink_to_fit();
    }

    return RD_OK;
}

// Called when the input file is finished.
int pnm_end_display() {
    if (DEBUG_MODE) std::cout << "P2. DISPLAY END\n";
    return RD_OK;
}

// Prepare for a new image by clearing out the 2D array. (also saves frame_number)
int pnm_init_frame(int fn) {
    if (DEBUG_MODE) std::cout << "P3. FRAME: " << fn << std::endl;
    frameNumberPPM = fn;
    pnm_clear();
    return RD_OK;
}

// Finishes and outputs the result to a ppm file
int pnm_end_frame() {
    if (DEBUG_MODE) std::cout << "P4. FRAME END\n";
    std::ofstream ppm;
    std::string uniqueExtension = "_" + std::to_string(frameNumberPPM) + ".ppm";
    ppm.open(display_name + uniqueExtension, std::ios::binary);

    // magic (ppm is "P6"), \n, Width and Height ASCII integers, \n, Max intensity value (0 to 65535), \n
    ppm << "P6\n" << display_xSize << ' ' << display_ySize << "\n255\n";

    // Copy vector into ppm file
    for (std::vector<char>& screenLine : screen)
        for (int x : screenLine)
            ppm.put(x);

    ppm.close();
    return RD_OK;
}

// Sets the image value at location (x, y) to the value of the current color.
int pnm_write_pixel(int x, int y, const float rgb[]) {
    if (DEBUG_MODE) std::cout << "P5. WRITE: ";
    int t = x*3;
    std::vector<char>& screenLine = screen[y];

    // +.5 for rounding when flooring with int cast
    screenLine[t++] = (int)(rgb[0] * BYTE_CONVERSION + .5); // Red
    screenLine[t++] = (int)(rgb[1] * BYTE_CONVERSION + .5); // Green
    screenLine[t]   = (int)(rgb[2] * BYTE_CONVERSION + .5); // Blue

    if (DEBUG_MODE) std::cout << x << ' ' << y 
        << " = " << screenLine[t-2] << ' ' << screenLine[t-1] << ' ' << screenLine[t] << std::endl;
    return RD_OK;
}

// Reads the value from location(x, y) in the array to the red, green, and blue values passed in.
int pnm_read_pixel(int x, int y, float rgb[]) {
    if (DEBUG_MODE) std::cout << "P6. READ: ";
    int t = x*3;
    std::vector<char>& screenLine = screen[y];

    rgb[0] = (float)screenLine[t++] / BYTE_CONVERSION; // Red
    rgb[1] = (float)screenLine[t++] / BYTE_CONVERSION; // Green
    rgb[2] = (float)screenLine[t]   / BYTE_CONVERSION; // Blue

    if (DEBUG_MODE) std::cout << x << ' ' << y 
        << " = " << rgb[0] << ' ' << rgb[1] << ' ' << rgb[2] << std::endl;
    return RD_OK;
}

// Sets and saves the backround color
int pnm_set_background(const float rgb[]) {
    if (DEBUG_MODE) std::cout << "P7. BACKGROUND\n"
        << rgb[0] << ' ' << rgb[1] << ' ' << rgb[2] << std::endl;

    // +.5 for rounding when flooring with int cast
    const int newBackR = (int)(rgb[0] * BYTE_CONVERSION + .5);
    const int newBackG = (int)(rgb[1] * BYTE_CONVERSION + .5);
    const int newBackB = (int)(rgb[2] * BYTE_CONVERSION + .5);

    if (DEBUG_MODE) std::cout << newBackR << ' ' << newBackG << ' ' << newBackB << std::endl;
    
    // Replace background
    for (std::vector<char>& screenLine : screen) {
        for (int i = 0; i < triple_xSize;) {
            int ri = i++;
            int gi = i++;
            int bi = i++;
            if (screenLine[ri] == backR
             && screenLine[gi] == backG
             && screenLine[bi] == backB) {
                screenLine[ri] = newBackR;
                screenLine[gi] = newBackG;
                screenLine[bi] = newBackB;
            }
        }
    }
    
    backR = newBackR;
    backG = newBackG;
    backB = newBackB;

    return RD_OK;
}

// (Re)initializes the image array to the background color.
int pnm_clear() {
    if (DEBUG_MODE) std::cout << "P8. CLEAR\n";

    for (std::vector<char>& screenLine : screen) {
        for (int i = 0; i < triple_xSize;) {
            screenLine[i++] = backR; // Red
            screenLine[i++] = backG; // Green
            screenLine[i++] = backB; // Blue
        }
    }

    return RD_OK;
}
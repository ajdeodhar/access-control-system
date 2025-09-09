#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

// Define the font structure
typedef struct {
    const uint8_t* data;
    uint8_t width;
    uint8_t height;
} FontDef;

// Declare two fonts (7x10 and 11x18)
extern FontDef Font_7x10;
extern FontDef Font_11x18;

// Define color constants
#define White 1
#define Black 0

#endif

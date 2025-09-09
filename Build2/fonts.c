#include "fonts.h"

// Dummy font data for example (actual font data would be huge arrays)
static const uint8_t Font7x10Data[] = {
    0x00, 0x00, 0x00, 0x00 // add more if needed
};

static const uint8_t Font11x18Data[] = {
    0x00, 0x00, 0x00, 0x00 // add more if needed
};

// Define Font_7x10 with dummy values
FontDef Font_7x10 = {
    .data = Font7x10Data,
    .width = 7,
    .height = 10
};

// Define Font_11x18 with dummy values
FontDef Font_11x18 = {
    .data = Font11x18Data,
    .width = 11,
    .height = 18
};

//FontDef Font_11x18 = { Font11x18Data, 11, 18 };

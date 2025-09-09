// Dummy ssd1306.c content
#include "ssd1306.h"

static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static uint8_t CurrentX = 0;
static uint8_t CurrentY = 0;

void ssd1306_command(uint8_t cmd) {
    i2c_start(SSD1306_ADDR << 1);
    i2c_write(SSD1306_COMMAND);
    i2c_write(cmd);
    i2c_stop();
}

void ssd1306_data(uint8_t data) {
    i2c_start(SSD1306_ADDR << 1);
    i2c_write(SSD1306_DATA);
    i2c_write(data);
    i2c_stop();
}

void ssd1306_init(void) {
    _delay_ms(100);

    ssd1306_command(0xAE); // Display OFF
    ssd1306_command(0x20); // Set Memory Addressing Mode
    ssd1306_command(0x10); // Page addressing mode
    ssd1306_command(0xB0); // Page Start Address
    ssd1306_command(0xC8); // COM Output Scan Direction
    ssd1306_command(0x00); // Low column address
    ssd1306_command(0x10); // High column address
    ssd1306_command(0x40); // Start line address
    ssd1306_command(0x81); // Set contrast control
    ssd1306_command(0xFF);
    ssd1306_command(0xA1); // Set segment re-map
    ssd1306_command(0xA6); // Normal display
    ssd1306_command(0xA8); // Set multiplex ratio
    ssd1306_command(0x3F);
    ssd1306_command(0xA4); // Display RAM content
    ssd1306_command(0xD3); // Set display offset
    ssd1306_command(0x00);
    ssd1306_command(0xD5); // Set display clock
    ssd1306_command(0xF0);
    ssd1306_command(0xD9); // Set pre-charge period
    ssd1306_command(0x22);
    ssd1306_command(0xDA); // Set COM pins
    ssd1306_command(0x12);
    ssd1306_command(0xDB); // Set Vcomh
    ssd1306_command(0x20);
    ssd1306_command(0x8D); // Enable charge pump
    ssd1306_command(0x14);
    ssd1306_command(0xAF); // Display ON

    ssd1306_clear();
    ssd1306_update_screen();
}

void ssd1306_clear(void) {
    for (uint16_t i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = 0x00;
    }
}

void ssd1306_update_screen(void) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);

        i2c_start(SSD1306_ADDR << 1);
        i2c_write(SSD1306_DATA);
        for (uint8_t col = 0; col < SSD1306_WIDTH; col++) {
            i2c_write(SSD1306_Buffer[SSD1306_WIDTH * page + col]);
        }
        i2c_stop();
    }
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    if (color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void ssd1306_set_cursor(uint8_t x, uint8_t y) {
    CurrentX = x;
    CurrentY = y;
}

void ssd1306_write_char(char ch, FontDef Font, uint8_t color) {
    uint16_t i, j;
    for (i = 0; i < Font.height; i++) {
        uint16_t line = Font.data[(ch - 32) * Font.height + i];
        for (j = 0; j < Font.width; j++) {
            if (line & (1 << j)) {
                ssd1306_draw_pixel(CurrentX + j, CurrentY + i, color);
            } else {
                ssd1306_draw_pixel(CurrentX + j, CurrentY + i, !color);
            }
        }
    }
    CurrentX += Font.width;
}

void ssd1306_write_string(char* str, FontDef Font, uint8_t color) {
    while (*str) {
        ssd1306_write_char(*str, Font, color);
        str++;
    }
}

void ssd1306_display(void) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_write_command(0xB0 + page);       // Set page address
        ssd1306_write_command(0x00);              // Set lower column address
        ssd1306_write_command(0x10);              // Set higher column address

        for (uint8_t col = 0; col < 128; col++) {
            ssd1306_write_data(SSD1306_Buffer[page * 128 + col]);
        }
    }
}

// Add these at the bottom of ssd1306.c or just before ssd1306_display

void ssd1306_write_command(uint8_t cmd) {
    ssd1306_command(cmd);
}

void ssd1306_write_data(uint8_t data) {
    ssd1306_data(data);
}

//avrdude -c arduino -p m328p -P COM7 -b 19200 -U flash:w:main.hex

// Dummy ssd1306.h content
#ifndef SSD1306_H_
#define SSD1306_H_

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "i2c_master.h"
#include "fonts.h"
void ssd1306_write_command(uint8_t command);
void ssd1306_write_data(uint8_t data);

// OLED width and height
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Commands
#define SSD1306_ADDR           0x3C

#define SSD1306_COMMAND        0x00
#define SSD1306_DATA           0x40

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_update_screen(void);
void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void ssd1306_write_char(char ch, FontDef Font, uint8_t color);
void ssd1306_write_string(char* str, FontDef Font, uint8_t color);
void ssd1306_set_cursor(uint8_t x, uint8_t y);
void ssd1306_display(void);

#endif

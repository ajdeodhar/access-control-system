// Dummy i2c_master.h content
#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <avr/io.h>
//#define __AVR_ATmega328P__  // Required for correct register definitions

#include <util/twi.h>

void i2c_init(void);
void i2c_start(uint8_t address);
void i2c_write(uint8_t data);
void i2c_stop(void);

#endif

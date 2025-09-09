#ifndef F_CPU
#define F_CPU 16000000UL // CPU frequency (16 MHz for ATmega328P)
#endif

#define F_SCL 100000UL // I2C clock speed 100kHz

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

// I2C initialization
void i2c_init(void) {
    TWSR = 0x00; // Prescaler = 1
    TWBR = ((F_CPU / F_SCL) - 16) / 2; // Bit rate register
    TWCR = (1 << TWEN); // Enable TWI
}

// Send START condition + address
void i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // START condition
    while (!(TWCR & (1 << TWINT))); // Wait for TWINT to set

    TWDR = address; // Load slave address
    TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
    while (!(TWCR & (1 << TWINT))); // Wait for TWINT to set
}

// Write data byte
void i2c_write(uint8_t data) {
    TWDR = data; // Load data
    TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
    while (!(TWCR & (1 << TWINT))); // Wait for TWINT to set
}

// Send STOP condition
void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // STOP condition
    _delay_us(10); // Short delay
}

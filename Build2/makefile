MCU=atmega328p
F_CPU=16000000UL
BAUD=9600
PROGRAMMER=arduino
PORT=COM8  # <- Replace with your actual COM port (e.g., COM3 on Windows)

CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude

CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Wall -Os

TARGET=main

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b 115200 -U flash:w:$(TARGET).hex

clean:
	rm -f *.o *.elf *.hex

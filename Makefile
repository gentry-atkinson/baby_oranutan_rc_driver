MCU=atmega328p
PORT=$(shell pavr2cmd --prog-port)
CFLAGS=-g -Wall -mcall-prologues -mmcu=$(MCU) $(DEVICE_SPECIFIC_CFLAGS) -Os
LDFLAGS=-Wl,-gc-sections -Wl,-relax
CC=avr-gcc
TARGET=rc

AVRDUDE_DEVICE = m328p
AVRDUDE=avrdude
DEVICE ?= atmega168
OBJECT_FILES=rc.o
OBJ2HEX=avr-objcopy
LDFLAGS=-Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

all: $(TARGET).hex

clean:
	rm -f *.o *.elf *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

rc.o: rc.c

%.obj: $(OBJECT_FILES)
		$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
		$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

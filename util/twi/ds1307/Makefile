CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

CCFLAGS = -Wall -Os -mmcu=atmega328p -DF_CPU=16000000UL -DBAUD=57600 -std=c99
OBJCOPYFLAGS = -O ihex
DUDEFLAGS = -p atmega328p -b 57600 -c arduino -D -P /./COM3 -q

SOURCES = main.c uart.c twi_module.c ds1307.c
#uart.c pc_interrupts.c rot_driver.c lcd_44780.c
#HEADERS = pin.h
COMPILED = firmware.hex
LINKED = firmware.o
OBJECTS = $(SOURCES:.c=.o)

all: $(COMPILED)

flash: $(COMPILED)
	$(DUDE) $(DUDEFLAGS) -Uflash:w:$<:i

disassemble: $(LINKED)
	$(OBJDUMP) $(OBJDUMPFLAGS) $(LINKED)

clean:
	$(RM) $(COMPILED) $(LINKED) $(OBJECTS)

$(COMPILED): $(LINKED)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(LINKED): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) $(CCFLAGS) -c $(SOURCES)

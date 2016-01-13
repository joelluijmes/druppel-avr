CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

CCFLAGS = -Wall -O2 -mmcu=attiny85 -DF_CPU=800000UL -DBAUD=19200 -std=c99
OBJCOPYFLAGS = -O ihex
DUDEFLAGS = -pattiny85 -b 9600 -B12 -c usbasp -PCOM3 -v

SOURCES = main.c ../../util/twi/twi.c ../../util/twi/usi/twi_usi.c ../../util/eeprom_usi/eeprom.c ../../util/ds1307_usi/ds1307.c communication.c
COMPILED = firmware.hex
LINKED = firmware.o
OBJECTS = $(SOURCES:.c=.o)
LIBRARIES = ../twi

all: $(COMPILED)
	avr-size firmware.o

flash: $(COMPILED)
	$(DUDE) $(DUDEFLAGS) -Uflash:w:$<:i

disassemble: $(LINKED)
	$(OBJDUMP) $(OBJDUMPFLAGS) $(LINKED)

clean:
	$(RM) $(COMPILED) $(LINKED) $(OBJECTS)

$(COMPILED): $(LINKED)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(LINKED): $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(LINKED)
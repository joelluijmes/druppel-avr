CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

CCFLAGS = -Wall -Os -mmcu=attiny85 -DWDT_ACTIVE -DSENSOR_TIMOUT=4 -DF_CPU=8000000UL -DBAUD=57600 -std=c99
OBJCOPYFLAGS = -O ihex
DUDEFLAGS = -pattiny85 -b 57600 -B12 -c usbasp  -v

SOURCES = main.c ../../util/twi/twi.c ../../util/twi/usi/twi_usi.c ../../util/ds1307_usi/ds1307.c ../../util/eeprom_usi/eeprom.c sensors.c communication.c

COMPILED = firmware.hex
LINKED = firmware.o
OBJECTS = $(SOURCES:.c=.o)

all: $(COMPILED)
	avr-size firmware.o

flash: $(COMPILED)
	$(DUDE) $(DUDEFLAGS) -Uflash:w:$<:i

disassemble: $(LINKED)
	$(OBJDUMP) $(OBJDUMPFLAGS) $(LINKED)

preprocessor: $(SOURCES)
	$(CC) $(CCFLAGS) -E -nostdinc $(SOURCES) 

clean:
	$(RM) $(COMPILED) $(LINKED) $(OBJECTS)

$(COMPILED): $(LINKED)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(LINKED): $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(LINKED)

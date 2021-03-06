PROJECT=gps_uart



SOURCES=./src/GPS_ROBOT_REV1.c


CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega164p
PROG_MMCU=atmega164p
PROGRAMMER=usbtiny
PORT=/dev/cuaU0

#defaults for atmega164p
LOW_FUSE=0x7f    #use external #0x42 default
HIGH_FUSE=0x99
EXT_FUSE=0xff

 

###############################


CFLAGS=-mmcu=$(MMCU) -Wall -Os -std=gnu99 -funsigned-char \
	-funsigned-bitfields -fpack-struct -fshort-enums -MD -MP -MT -I./

$(PROJECT).hex: $(PROJECT).out
	@echo "Creating hex file..."
	$(OBJCOPY) -j .text -O ihex $(PROJECT).out $(PROJECT).hex
	rm -f ./*.d
	rm -f ./*.out
	@echo

$(PROJECT).out: $(SOURCES)
	@echo
	@echo "Compiling..."
	$(CC) $(CFLAGS) -o $(PROJECT).out $(SOURCES)
	avr-size $(PROJECT).out
	@echo

program: $(PROJECT).hex
	@echo
	@echo "Downloading..."
	avrdude -P $(PORT) -v -p $(PROG_MMCU) -c $(PROGRAMMER) -e \
	-U flash:w:$(PROJECT).hex
	@echo

program_fuses:
	@echo
	@echo "Writing fuses..."
	avrdude -P $(PORT) -v -p $(PROG_MMCU) -c $(PROGRAMMER) -e \
	-U lfuse:w:$(LOW_FUSE):m -U hfuse:w:$(HIGH_FUSE):m -U efuse:w:$(EXT_FUSE):m
	@echo

clean:
	@echo
	@echo "Cleaning..."
	rm -f ./*.out
	rm -f ./*.hex
	rm -f ./*.d
	rm -f ./*.map
	rm -f ./*.o
	@echo


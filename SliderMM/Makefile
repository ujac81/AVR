

MCU       = atmega32
PART      = m32
F_CPU     = 1600000
ISP       = avrispmkII
BITCLOCK  = 10

CC=/usr/bin/avr-gcc
AVROBJCOPY=/usr/bin/avr-objcopy
AVRDUDE=/usr/bin/avrdude


INCLUDES=-I/usr/lib/avr/include
CFLAGS=-W -Wall -Os -mmcu=${MCU} -std=gnu99 -DF_CPU=$(F_CPU)UL -pipe

slidermm.o: slidermm.c
	${CC} ${CFLAGS} ${INCLUDES} -c slidermm.c -o slidermm.o

slidermm.elf: slidermm.o
	${CC} ${CFLAGS} ${INCLUDES} slidermm.o -o slidermm.elf

slidermm.hex: slidermm.elf
	${AVROBJCOPY} -j .text -j .data -O ihex slidermm.elf slidermm.hex

slidermm_eeprom.hex: slidermm.elf
	${AVROBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex slidermm.elf slidermm_eeprom.hex

all: slidermm.hex slidermm_eeprom.hex

clean:
	rm -f *.o *.hex *.elf

flash: all
	sudo ${AVRDUDE} -c ${ISP}  -p ${PART} -B${BITCLOCK} -U flash:w:slidermm.hex


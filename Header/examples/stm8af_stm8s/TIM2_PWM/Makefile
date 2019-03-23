## A directory for common include files
INCLUDEDIR = ../../../stm8/stm8af_stm8s

## Compiler settings
CC = sdcc
DEFINES=
CFLAGS = --std-sdcc99 -mstm8 $(DEFINES) -I$(INCLUDEDIR)
LDFLAGS = -lstm8 -mstm8 --out-fmt-ihx

## Settings for stm8flash SWIM upload tool
STM8FLASH            = stm8flash
STM8FLASH_DEVICE     = stm8s105c6
STM8FLASH_PROGRAMMER = stlink

## Settings for stm8gal UART upload tool
STM8GAL      = stm8gal
STM8GAL_PORT = /dev/ttyUSB0
#STM8GAL_PORT = COM17

## Get program name from enclosing directory name
PROGRAM = $(lastword $(subst /, ,$(CURDIR)))
SOURCES=$(wildcard *.c $(COMMONDIR)/*.c)
OBJECTS=$(SOURCES:.c=.rel)
HEADERS=$(wildcard *.h $(COMMONDIR)/*.h)
CCOMPILEDFILES=$(SOURCES:.c=.asm) $(SOURCES:.c=.lst) $(SOURCES:.c=.rel) \
               $(SOURCES:.c=.rst) $(SOURCES:.c=.sym)

.PHONY: all clean flash

$(PROGRAM).ihx: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.rel : %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f $(PROGRAM).ihx $(PROGRAM).cdb $(PROGRAM).lk $(PROGRAM).map $(CCOMPILEDFILES)

swim: $(PROGRAM).ihx
	$(STM8FLASH) -c $(STM8FLASH_PROGRAMMER) -p $(STM8FLASH_DEVICE) -w $(PROGRAM).ihx

serial: $(PROGRAM).ihx
	$(STM8GAL) -p $(STM8GAL_PORT) -w $(PROGRAM).ihx -R 2

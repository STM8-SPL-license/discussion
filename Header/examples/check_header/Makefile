## A directory for common include files
COMMONDIR = ../../stm8

## Compiler settings
CC = sdcc
DEFINES=
CPPFLAGS = -I$(COMMONDIR)
CFLAGS = --std-sdcc99 -mstm8 $(DEFINES)
LDFLAGS = -lstm8 -mstm8 --out-fmt-ihx

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

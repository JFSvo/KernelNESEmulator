C_SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJ = ${C_SOURCES:.c=.o}

CC = i686-elf-gcc
CFLAGS = -g -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I./include -I../../src

nes_emulator.bin: $(OBJ)
	$(CC) -T ../linker.ld -o $@ $(CFLAGS) $(OBJ) -lgcc

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o
	rm -rf nes_emulator.bin

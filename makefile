FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/string/string.o ./build/fs/pparser.o ./build/disk/streamer.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/emulator/CPU/emulator.o ./build/emulator/CPU/tracelogger.o ./build/emulator/CPU/opcode_table.o ./build/emulator/CPU/emulator_debug.o ./build/emulator/PPU/ppu.o ./build/drivers/timer/pit.o ./build/drivers/vga/vga.o ./build/drivers/keyboard/keyboard.o ./build/tictactoe.o
# ./build/task/tss.asm.o ./build/task/task.o ./build/task/task.asm.o ./build/task/process.o
INCLUDES = -I ./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc 
NASM_BOOT_FLAGS = -f bin

DIRS = \
    build \
    build/idt \
    build/memory \
    build/memory/heap \
    build/memory/paging \
    build/io \
    build/disk \
    build/string \
    build/fs \
    build/fs/fat \
    build/gdt \
    build/task \
    build/emulator \
	build/emulator/CPU \
	build/emulator/PPU \
    build/drivers \
    build/drivers/timer \
    build/drivers/vga \
	build/drivers/keyboard \
    bin


all: dirs ./bin/boot_$(MODE).bin ./bin/kernel.bin
	rm -rf ./bin/os.bin 
	dd if=./bin/boot_$(MODE).bin >> ./bin/os.bin 
	dd if=./bin/kernel.bin >> ./bin/os.bin 
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mkdir -p /mnt/d
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# Copy a file over
	sudo cp ./hello.txt /mnt/d
	sudo cp ./roms/testing/* /mnt/d
	sudo cp ./roms/tictactoe/* /mnt/d
	sudo umount /mnt/d

dirs:
	mkdir -p $(DIRS)

./bin/kernel.bin: $(FILES) 
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

ifeq ($(MODE), vga)
    NASM_BOOT_FLAGS += -DVGA_MODE
endif

./bin/boot_$(MODE).bin: ./src/boot/boot.asm
	nasm $(NASM_BOOT_FLAGS) ./src/boot/boot.asm -o ./bin/boot_$(MODE).bin 
	
./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o 

./build/kernel.o: ./src/kernel.c 
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/tictactoe.o: ./src/tictactoe.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/tictactoe.c -o ./build/tictactoe.o

./build/idt/idt.asm.o: ./src/idt/idt.asm  
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o  
	
./build/idt/idt.o: ./src/idt/idt.c  
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99  -c ./src/idt/idt.c -o ./build/idt/idt.o  
	
./build/memory/memory.o: ./src/memory/memory.c  
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c  ./src/memory/memory.c -o ./build/memory/memory.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o
	
./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I ./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o
	
./build/string/string.o: ./src/string/string.c 
	i686-elf-gcc $(INCLUDES) -I ./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/fs/pparser.o: ./src/fs/pparser.c 
	i686-elf-gcc $(INCLUDES) -I ./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/pparser.c -o ./build/fs/pparser.o

./build/disk/streamer.o: ./src/disk/streamer.c 
	i686-elf-gcc $(INCLUDES) -I ./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/file.o: ./src/fs/file.c 
	i686-elf-gcc $(INCLUDES) -I ./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c 
	i686-elf-gcc $(INCLUDES) -I ./src/fs -I ./src/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/gdt/gdt.o: ./src/gdt/gdt.c 
	i686-elf-gcc $(INCLUDES) -I ./src/gdt $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o 
	
./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm 
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

# ./build/task/tss.asm.o: ./src/task/tss.asm 
# 	nasm -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

# ./build/task/task.o: ./src/task/task.c 
# 	i686-elf-gcc $(INCLUDES) -I ./src/task $(FLAGS) -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o
	
# ./build/task/process.o: ./src/task/process.c 
# 	i686-elf-gcc $(INCLUDES) -I ./src/task $(FLAGS) -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

# ./build/task/task.asm.o: ./src/task/task.asm 
# 	nasm -f elf -g ./src/task/task.asm -o ./build/task/task.asm.o

./build/emulator/CPU/emulator.o: ./src/emulator/CPU/emulator.c 
	i686-elf-gcc $(INCLUDES) -I ./src/emulator/CPU $(FLAGS) -std=gnu99 -c ./src/emulator/CPU/emulator.c -o ./build/emulator/CPU/emulator.o

./build/emulator/CPU/tracelogger.o: ./src/emulator/CPU/tracelogger.c 
	i686-elf-gcc $(INCLUDES) -I ./src/emulator/CPU $(FLAGS) -std=gnu99 -c ./src/emulator/CPU/tracelogger.c -o ./build/emulator/CPU/tracelogger.o

./build/emulator/CPU/opcode_table.o: ./src/emulator/CPU/opcode_table.c 
	i686-elf-gcc $(INCLUDES) -I ./src/emulator/CPU $(FLAGS) -std=gnu99 -c ./src/emulator/CPU/opcode_table.c -o ./build/emulator/CPU/opcode_table.o 
	
./build/emulator/CPU/emulator_debug.o: ./src/emulator/CPU/emulator_debug.c 
	i686-elf-gcc $(INCLUDES) -I ./src/emulator/CPU $(FLAGS) -std=gnu99 -c ./src/emulator/CPU/emulator_debug.c -o ./build/emulator/CPU/emulator_debug.o

# ./build/emulator/PPU/ppu.o: ./src/emulator/PPU/ppu.c 
# 	i686-elf-gcc $(INCLUDES) -I ./src/emulator/PPU $(FLAGS) -std=gnu99 -c ./src/emulator/PPU/ppu.c -o ./build/emulator/PPU/ppu.o

./build/drivers/timer/pit.o: ./src/drivers/timer/pit.c
	i686-elf-gcc $(INCLUDES) -I./src/drivers/timer $(FLAGS) -std=gnu99 -c ./src/drivers/timer/pit.c -o ./build/drivers/timer/pit.o

./build/drivers/vga/vga.o: ./src/drivers/vga/vga.c
	i686-elf-gcc $(INCLUDES) -I./src/drivers/vga $(FLAGS) -std=gnu99 -c ./src/drivers/vga/vga.c -o ./build/drivers/vga/vga.o 
	
./build/drivers/keyboard/keyboard.o: ./src/drivers/keyboard/keyboard.c
	i686-elf-gcc $(INCLUDES) -I./src/drivers/keyboard $(FLAGS) -std=gnu99 -c ./src/drivers/keyboard/keyboard.c -o ./build/drivers/keyboard/keyboard.o 
	
clean:
	rm -rf ./bin/boot_text.bin
	rm -rf ./bin/boot_vga.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o

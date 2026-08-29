#honestly idk how I did this but at least it works- FOR NOW. :sob:
TARGET      := i686-pc-none-elf

CC          := clang
LD          := ld.lld
OBJCOPY     := llvm-objcopy

CFLAGS      := --target=$(TARGET) \
               -ffreestanding \
               -fno-stack-protector \
               -fno-pie \
               -m32 \
               -march=i686 \
               -Wall \
               -Wextra \
               -O2

LDFLAGS     := -m elf_i386 \
               -T linker.ld \
               --oformat=elf32-i386

KERNEL      := kernel.elf

C_SOURCES   := kernel.c fs.c gdt.c print.c device.c keyboard.c serial.c
ASM_SOURCES := boot.s gdt_flush.s

C_OBJECTS   := $(C_SOURCES:.c=.o)
ASM_OBJECTS := $(ASM_SOURCES:.s=.o)

OBJECTS     := $(ASM_OBJECTS) $(C_OBJECTS)


.PHONY: all clean run

all: $(KERNEL)

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) --target=$(TARGET) -m32 -c $< -o $@

clean:
	rm -f $(OBJECTS) $(KERNEL)

run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)
#yes it's made for QEMU shush it won't run on ANYTHING yet

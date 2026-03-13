# Define directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
BOOT_DIR = $(SRC_DIR)/boot

# Find all source files (*.c) recursively
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Create object files from source files (*.o)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Kernel directories
KERNEL_DIR = $(SRC_DIR)/kernel

# Compiler and flags
CC = x86_64-elf-gcc
CFLAGS = -ffreestanding -m32
LD = x86_64-elf-ld
LDFLAGS = -m elf_i386
ASM = nasm
AFLAGS = -f bin

# Boot sector
BOOT_BIN = $(BIN_DIR)/boot_sect.bin

# Kernel
KERNEL_OBJ = $(OBJ_DIR)/kernel/kernel.o
KERNEL_BIN = $(BIN_DIR)/kernel.bin

# Combined OS image
OS_IMAGE = $(BIN_DIR)/os-image.bin

# Default target
all: $(OS_IMAGE)

# Assemble the boot sector
$(BOOT_BIN): $(BOOT_DIR)/boot.asm $(BOOT_DIR)/print_string.asm $(BOOT_DIR)/print_string_pm.asm $(BOOT_DIR)/switch_to_pm.asm $(BOOT_DIR)/gdt.asm | $(BIN_DIR)
	$(ASM) $(AFLAGS) -i $(BOOT_DIR)/ $(BOOT_DIR)/boot.asm -o $(BOOT_BIN)

# Compile kernel C source to object file
$(KERNEL_OBJ): $(KERNEL_DIR)/kernel.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel object into flat binary
$(KERNEL_BIN): $(KERNEL_OBJ) | $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ -Ttext 0x1000 $< --oformat binary

# Concatenate boot sector and kernel into a single OS image, padded to at least 16 sectors (8KB)
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@
	truncate -s 8192 $@

# Rule to create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Run in QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# Clean up compiled files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: clean all run

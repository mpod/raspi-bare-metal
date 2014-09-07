ARMGNU ?= arm-none-eabi

BUILD = build/
SOURCE = src/
TARGET = kernel.img
QEMUTARGET = kernel.elf
LIST = kernel.list
MAP = kernel.map
LINKER = link.ld
START = $(SOURCE)start.s
LIB = lib/
OBJECTS := $(addprefix $(BUILD),bcm2835.o ili9340.o pitft.o fb.o qemu.o raycasting.o main.o)

all: $(LIST) $(TARGET)

qemu: OBJECTS = $(addprefix $(BUILD),qemu.o raycasting.o main.o)
qemu: clean $(BUILD) qemu-main kernel.elf

qemu-main: 
	$(ARMGNU)-gcc -Wall -nostdlib -nostartfiles -ffreestanding -std=gnu99 -DQEMU=1 -c $(SOURCE)main.c -o $(BUILD)main.o

$(QEMUTARGET): $(OBJECTS) $(BUILD)start.o $(LINKER)
	$(ARMGNU)-ld --no-undefined -Map $(MAP) -T $(LINKER) -o kernel.elf $(BUILD)start.o $(OBJECTS) -L $(LIB) -lm -lc -lgcc

runqemu: 
	qemu-system-arm -m 128 -kernel kernel.elf -serial stdio -machine integratorcp

pitft: OBJECTS = $(addprefix $(BUILD),bcm2835.o ili9340.o pitft.o raycasting.o main.o)
pitft: clean $(BUILD) pitft-main all

pitft-main:
	$(ARMGNU)-gcc -Wall -nostdlib -nostartfiles -ffreestanding -std=gnu99 -DPITFT=1 -c $(SOURCE)main.c -o $(BUILD)main.o

fb: OBJECTS = $(addprefix $(BUILD),bcm2835.o fb.o raycasting.o main.o)
fb: clean $(BUILD) fb-main all

fb-main:
	$(ARMGNU)-gcc -Wall -nostdlib -nostartfiles -ffreestanding -std=gnu99 -DFB=1 -c $(SOURCE)main.c -o $(BUILD)main.o

$(LIST) : $(BUILD)output.elf
	$(ARMGNU)-objdump -d $(BUILD)output.elf > $(LIST)

$(TARGET) : $(BUILD)output.elf
	$(ARMGNU)-objcopy $(BUILD)output.elf -O binary $(TARGET) 

$(BUILD)output.elf : $(OBJECTS) $(BUILD)start.o $(LINKER)
	$(ARMGNU)-ld --no-undefined -Map $(MAP) -T $(LINKER) -o $(BUILD)output.elf $(BUILD)start.o $(OBJECTS) -L $(LIB) -lm -lc -lgcc

$(BUILD)%.o: $(SOURCE)%.c $(BUILD)
	$(ARMGNU)-gcc -Wall -nostdlib -nostartfiles -ffreestanding -std=gnu99 -c $< -o $@

$(BUILD)start.o: $(SOURCE)start.s $(BUILD)
	$(ARMGNU)-as -I $(START) $< -o $@

$(BUILD):
	mkdir $@

clean : 
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
	-rm -f $(QEMUTARGET)


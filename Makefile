#
# Makefile to compile the self-healing TrustZone based FreeRTOS 
#
# @author:       Michael Denzel
# @contact:      https://github.com/mdenzel
# @license:      GNU General Public License 2.0 or later
# @date:         2017-03-17
# @version:      0.1
#

# --- INIT ---
CROSS_PREFIX= arm-none-eabi-
AS = $(CROSS_PREFIX)as
CC = $(CROSS_PREFIX)gcc
LD = $(CROSS_PREFIX)ld
OBJCOPY = $(CROSS_PREFIX)objcopy
OBJDUMP = $(CROSS_PREFIX)objdump

# --- OBJECTS ---
BUILD_DIR = build
OBJS += $(BUILD_DIR)/entry.o #has to be the first entry!
OBJS += $(BUILD_DIR)/main.o
#Drivers
OBJS += $(BUILD_DIR)/mxc_serial.o
OBJS += $(BUILD_DIR)/IO.o
#Analysis
OBJS += $(BUILD_DIR)/timing.o
#Interrupts/TrustZone
OBJS += $(BUILD_DIR)/interrupts.o
OBJS += $(BUILD_DIR)/trustzone.o
#Self-healing
OBJS += $(BUILD_DIR)/selfhealing.o
#General
OBJS += $(BUILD_DIR)/kernelpanic.o
OBJS += $(BUILD_DIR)/exceptions.o
#Tasks
OBJS += $(BUILD_DIR)/controltasks.o
#	FreeRTOS Core
OBJS += $(BUILD_DIR)/FreeRTOS/croutine.o
OBJS += $(BUILD_DIR)/FreeRTOS/list.o
OBJS += $(BUILD_DIR)/FreeRTOS/queue.o
OBJS += $(BUILD_DIR)/FreeRTOS/tasks.o
#	Selected HEAP implementation for FreeRTOS
OBJS += $(BUILD_DIR)/FreeRTOS/MemMang/heap_4.o
#	FreeRTOS portable layer
OBJS += $(BUILD_DIR)/FreeRTOS/GCC/port.o
OBJS += $(BUILD_DIR)/FreeRTOS/GCC/portisr.o



# --- FLAGS ---
override CFLAGS += -Wall -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-a8 -DCORTEX_A8 -I Demo -I Demo/Drivers -I Demo/TrustZone -I Demo/Tasks -I Demo/Analysis -I FreeRTOS/Source/include -I FreeRTOS/Source/portable/GCC -I FreeRTOS/Source/portable/MemMang -std=c99 -DTRUSTZONE
ASFLAGS = -Wall -mcpu=cortex-a8
CPPFLAGS = -DTRUSTZONE
LDFLAGS = -L "/usr/lib/gcc/arm-none-eabi/5.2.0" -lgcc -L "/usr/arm-none-eabi/lib" -lc

# --- Compilation ---
.phony: all noTZ dir clean debug smc info timingTz timingNoTz

all: clean dir kernel.img

noTz: CFLAGS:=$(filter-out -DTRUSTZONE,$(CFLAGS))
noTz: CPPFLAGS:=$(filter-out -DTRUSTZONE,$(CPPFLAGS))
noTz: all

debug: CFLAGS += -DDEBUG
debug: CPPFLAGS += -DDEBUG
debug: info

smc: CFLAGS += -DPRINTSMC
smc: CPPFLAGS += -DPRINTSMC
smc: info

info: CFLAGS += -DINFO
info: CPPFLAGS += -DINFO
info: all

timingTz: CFLAGS += -DTIMING
timingTz: CPPFLAGS += -DTIMING
timingTz: all

timingNoTz: CFLAGS += -DTIMING
timingNoTz: CFLAGS:=$(filter-out -DTRUSTZONE,$(CFLAGS))
timingNoTz: CPPFLAGS += -DTIMING
timingNoTz: CPPFLAGS:=$(filter-out -DTRUSTZONE,$(CPPFLAGS))
timingNoTz: all

dir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/FreeRTOS
	@mkdir -p $(BUILD_DIR)/FreeRTOS/GCC
	@mkdir -p $(BUILD_DIR)/FreeRTOS/MemMang

clean:
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.asm *.img
	@rm -fr $(BUILD_DIR)

# --- RULES ---

#application
$(BUILD_DIR)/%.o: Demo/%.s
#	call the c preprocessor also on asm files
	@cpp $(CPPFLAGS) $< > $(BUILD_DIR)/$*.s
	@$(AS) $(ASFLAGS) $(BUILD_DIR)/$*.s -o $@ >/dev/null

$(BUILD_DIR)/%.o: Demo/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: Demo/Analysis/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: Demo/Drivers/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: Demo/TrustZone/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: Demo/Tasks/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

#FreeRTOS
$(BUILD_DIR)/FreeRTOS/%.o: FreeRTOS/Source/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/FreeRTOS/GCC/%.o: FreeRTOS/Source/portable/GCC/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/FreeRTOS/MemMang/%.o: FreeRTOS/Source/portable/MemMang/%.c
	@$(CC) $(CFLAGS) -c $< -o $@


# --- TARGET ---
kernel.img: kernel.ld $(OBJS)
	@$(CC) $(OBJS) -Wl,-T kernel.ld -Wl,$(LDFLAGS) -nostartfiles -o $(BUILD_DIR)/kernel.elf
	@$(OBJDUMP) -D $(BUILD_DIR)/kernel.elf > $(BUILD_DIR)/kernel.asm #for debugging purposes
	@$(OBJCOPY) $(BUILD_DIR)/kernel.elf -O binary kernel.img

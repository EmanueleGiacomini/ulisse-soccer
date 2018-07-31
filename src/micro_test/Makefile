# @author: Emanuele Giacomini

# .o files

PROJ_NAME=digio_test

ARDUINO_DIR=/home/emanuele/.local/share/umake/ide/arduino
ARM_TOOLS_DIR=$(ARDUINO_DIR)/hardware/tools/arm/bin
TEENSY_LIB_DIR=$(ARDUINO_DIR)/hardware/teensy/avr/cores/teensy3

CC_NAME =   arm-none-eabi-gcc
LD_NAME =   $(CC_NAME)
AR_NAME =   arm-none-eabi-ar
AS_NAME =   arm-none-eabi-as
OBJCOPY_NAME    =   arm-none-eabi-objcopy

CC=$(ARM_TOOLS_DIR)/$(CC_NAME)
LD=$(ARM_TOOLS_DIR)/$(LD_NAME)
AR=$(ARM_TOOLS_DIR)/$(AR_NAME)
AS=$(ARM_TOOLS_DIR)/$(AS_NAME)
OBJCOPY=$(ARM_TOOLS_DIR)/$(OBJCOPY_NAME)

PROJ_DIR=/home/emanuele/percorso-eccellenza/ulisse-soccer/src
PROJ_LIB_DIR=$(PROJ_DIR)/arch/include
PROJ_SRC_DIR=$(PROJ_DIR)/arch/teensy

_OBJS = encoder.o	\
				pins.o		\
				timer.o

OBJS = $(addprefix $(PROJ_SRC_DIR)/, $(_OBJS))


#Compile flags
ASFLAGS=-g
LDFLAGS=
CFLAGS= -c -O2 -g -Wall -ffunction-sections -nostdlib -MMD \
        -fno-exceptions -felide-constructors -std=gnu++14   \
        -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant   \
        -D__MK20DX256__ -DF_CPU=72000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH	\
				-I$(TEENSY_LIB_DIR) -I$(TEENSY_LIB_DIR)/avr/

CFLAGS += -I$(PROJ_LIB_DIR)


###############################################

all: $(PROJ_NAME).elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(CFLAGS) $< -o $@

%.elf: %.o
	$(CC) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) $< -O binary $@


#all:	$(PROJ_NAME).elf

#$(PROJ_NAME).elf: $(OBJS) $(PROJ_NAME).o
#	$(CC)  -o $@ $(OBJS) $(CFLAGS)
#	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
#	$(OBJCOPY) -O binary $(PROJ_NAME).hex $(PROJ_NAME).bin

#%.o: $(PROJ_SRC_DIR)/%.c $(PROJ_LIB_DIR)/%.h
#	$(CC) -o $@ $(CFLAGS)

.PHONY:	clean

clean:
	rm -f *.o
	rm -f $(PROJ_LIB_DIR)/*.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
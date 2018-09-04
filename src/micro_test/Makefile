#	Built by Emanuele Giacomini
# Based on https://github.com/74hc595/teensy3-without-arduino

# Need to install newest version of arm-none-eabi
# To do so, follow these instructions

# Remove any previous installations of gcc-arm
# $> sudo apt remove gcc-arm-none-eabi
# Install the one from PPA:
#	$> sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
#	$> sudo apt-get update
#	$> sudo apt-get install gcc-arm-embedded

# More instructions at https://stackoverflow.com/questions/50151897/arm-gcc-conflicting-cpu-architectures

TARGET = teensy-mk-test
$(info [Target name]: $(TARGET))

# Only supported version is 31 =)
TEENSY = 31

# CPU core speed, available frequencies:
# 24000000
# 48000000
# 96000000
TEENSY_CORE_SPEED = 96000000

# Some libraries will require this to be defined
# If you define this, you will break the default main.cpp
#ARDUINO = 10600

# configurable options
OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH

# Directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

#************************************#
# Toolchain and teensy resources
#************************************#
ARDUINODIR = /home/emanuele/.local/share/umake/ide/arduino/hardware
TOOLSPATH = $(ARDUINODIR)/tools

LIBRARYPATH = /home/emanuele/percorso-eccellenza/ulisse-soccer/src/arch/teensy
COREPATH = $(LIBRARYPATH)/core



#************************************#
# Compiler Flags
#************************************#

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -mthumb -ffunction-sections -fdata-sections -nostdlib -MMD $(OPTIONS) -DTEENSYDUINO=124 -DF_CPU=$(TEENSY_CORE_SPEED) -DTEENSY_BAREBONES -I$(COREPATH)
# Explanation for each flag:
# -Wall : enables all compiler's warning messages.
# -g 		: generates debug information to be used by GDB debugger.
# -Os 	: Optimize for size.
# -mthumb : 
# -ffunction-sections	: (look at next flag)	
# -fdata-sections	:	Both needed for dead code elimination
# -nostdlib	: Do not use the standard system startup files or libraries when linking.
# -MMD : (TODO)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# CFLAGS = compiler options for C files
CFLAGS = 

# Linker options
LDFLAGS = -Os -Wl,--gc-sections -mthumb
# -Wl, --gc-sections	:	pass --gc-sections as option for the linker

# Additional libraries to link
LIBS = 

ifeq ($(TEENSY), 31)
	CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4
  LDSCRIPT = $(COREPATH)/mk20dx256.ld
  LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else
	$(error Invalid setting for TEENSY)
endif 

ifdef ARDUINO
	CPPFLAGS += -DARDUINO=$(ARDUINO)
else
	CPPFLAGS += -DUSING_MAKEFILE
endif

# Toolchain 
TOOLNAME=arm-none-eabi-
CC = $(TOOLNAME)gcc
CXX = $(TOOLNAME)g++
OBJCOPY = $(TOOLNAME)objcopy
SIZE = $(TOOLNAME)size

# automatically create lists of the sources and objects
LC_FILES := $(wildcard $(LIBRARYPATH)/*.c)
TC_FILES := $(wildcard $(COREPATH)/*.c)
C_FILES := $(wildcard *.c)

# Include paths for libraries
#L_INC := $(foreach lib, $(filter %/, $(wildcard $(LIBRARYPATH)/*)), -I$(lib))
L_INC := -I$(LIBRARYPATH)/../include -I$(LIBRARYPATH)/core -I$(LIBRARYPATH)

SOURCES := $(C_FILES:.c=.o) $(TC_FILES:.c=.o) $(LC_FILES:.c=.o)
OBJS := $(foreach src, $(SOURCES), $(BUILDDIR)/$(src))

#RULES
all:	hex

build:	$(TARGET).elf

hex:	$(TARGET).hex

post_compile:	$(TARGET).hex
	$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	$(abspath $(TOOLSPATH))/teensy_reboot

upload:	post_compile reboot

$(BUILDDIR)/%.o: %.c
	@echo -e "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o:	%.cpp
	@echo -e "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo -e "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

%.hex: %.elf
	@echo -e "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"
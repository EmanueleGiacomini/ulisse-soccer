#	Built by Emanuele Giacomini
# Need to install newest version of arm-none-eabi
# Please type in the terminal: sudo apt install libnewlib-dev libnewlib-arm-none-eabi

#Name of PRoJect
PRJNAME=main
$(info Project Name: $(PRJNAME))

PREFIX=..
#Headers directory
DINC=$(PREFIX)/include
#Source directory
DSRC=$(PREFIX)/src
#Build directory
DBUILD=$(DSRC)/build

DIR_ARDUINO=/home/emanuele/.local/share/umake/ide/arduino
TEENSYCORE=$(DIR_ARDUINO)/hardware/teensy/avr/cores/teensy3
$(info [Computed] Teensy Core Directory: $(TEENSYCORE))

# Source files from teensy core folder
_TEENSYLIBS=	memcpy-armv7m.S	\
							memset.S	\
							mk20dx128.c	\
							nonstd.c	\
							pins_teensy.c	

TEENSYLIBS=$(addprefix $(TEENSYCORE)/, $(_TEENSYLIBS))

#-------# COMPILER STUFF #-----------------------------------------------#
CNAME = arm-none-eabi-

CC=$(CNAME)gcc
$(info [Computed] CC: $(CC))
CFLAGS= -c -O2 -g -Wall -ffunction-sections -nostdlib -MMD \
        -fno-exceptions	\
        -mthumb -mcpu=cortex-m4 -fsingle-precision-constant	\
        -D__MK20DX256__ -DF_CPU=72000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH	\
#Include directories
CFLAGS+= -I$(TEENSYCORE) -I$(DINC) -I./
$(info [Computed] CFLAGS: $(CFLAGS))

LD=$(CNAME)ld
LDFLAGS= -O2 -Wl,--gc-sections,--relax -T

# Dependencies in include dir
ifndef _DEPS
_DEPS=$(notdir $(wildcard $(DINC)/*.h))
$(info [Computed] Dependencies: $(_DEPS))
else
$(info [User] Dependencies: $(_DEPS))
endif
DEPS=$(addprefix $(DINC)/, $(_DEPS))

# Objects in /build dir
ifndef _OBJS
_OBJS=$(notdir $(wildcard $(DSRC)/*.c))
_OBJS:=$(_OBJS:.c=.o)
$(info [Computed] Objects: $(_OBJS))
else
$(info [User] Objects: $(_OBJS))
endif
OBJS:=$(addprefix $(DBUILD)/, $(_OBJS))
OBJS+=$(addsuffix .o, $(addprefix $(DBUILD)/, $(_TEENSYLIBS)))
#-------# RULES #--------------------------------------------------------#
dir_guard=@mkdir -p build/

all: $(PRJNAME).elf

$(OBJS): $(DSRC)/*.c 
	$(dir_guard)
	$(CC) -o $@ $< $(CFLAGS)

$(PRJNAME).elf:	$(OBJS) 
	$(CC) -o $@ $< $(LDFLAGS) $(TEENSYCORE)/mk20dx256.ld


.PHONY: clean

clean:
	rm -rf $(DBUILD)

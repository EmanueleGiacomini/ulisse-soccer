PREFIX=../../
CC=gcc
INCLUDE_DIRS=-I$(PREFIX)/src/common -I$(PREFIX)/src/host
CC_OPTS=-Wall --std=gnu99 $(INCLUDE_DIRS)

LIBS=-lpthread -lreadline

OBJS=	packet_handler.o\
	ulisse_print_packets.o\
	ventry.o\
	controller.o\
	serial_linux.o\
	ulisse_client.o\
	ulisse_shell_globals.o
	
HEADERS = packet_header.h\
		packet_operations.h\
		packet_handler.h\
		ulisse_print_packets.h\
		ulisse_packets.h\
		ventry.h

BINS= host.bin
.phony: clean all

all: $(BINS)

#common objects
%.o: $(PREFIX)/src/common/%.c
	$(CC) $(CC_OPTS) -c $<

#client
%.o: $(PREFIX)/src/host/%.c
	$(CC) $(CC_OPTS) -c $< $(LIBS)

%.bin: %.o $(OBJS) $(INCLUDES)
	$(CC) $(CC_OPTS) -o $@ $< $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) $(BINS) *~ *.d
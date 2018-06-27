#include "serial_linux.h"
#include "../common/ulisse_packets.h"
#include "../common/ulisse_print_packets.h"
#include "../common/packet_handler.h"
#include "ulisse_client.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// macro to simplify error handling
#define handle_error(msg)   \
  do {perror(msg); exit(EXIT_FAILURE); } while(0)

const char *banner[] = {
  "Ulisse",
  "absolutley not generalized client for ulisse robot",
  "usage:",
  "   $> ulisse_host <parameters>",
  "parameters: ",
  "-serial-device <string>: the serial port (default /dev/ttyACM0)",
  "-baud <int>: the baudrate of the communication interface",
  "-rate <int>: the refresh rate [Hz] (default 20 Hz)",
  "             use low rate (e.g., 1 Hz) for wireless connection",
  0
};

void printBanner() {
  const char* const* line=banner;
  while(*line) {
    printf("%s\n", *line);
    line++;
  }
}

struct UlisseClient* client;
const char* serial_name = "/dev/ttyACM0";

int main(int argc, char* argv[]) {
  if(argc==1) {
    printBanner();
    return 0;
  }
  int c=1;
  const char* device=0;
  uint32_t baudrate = 115200;
  int rate=20;  // Refresh com rate -> default 20 Hz
  while(c<argc) {
    if(!strcmp(argv[c], "-rate")) {
      c++;
      rate=atoi(argv[c]);
    } else if(!strcmp(argv[c], "-serial-device")) {
      c++;
      device=argv[c];
    } else if(!strcmp(argv[c], "-baud")) {
      c++;
      baudrate=atoi(argv[c]);
    } else if(!strcmp(argv[c], "-h") || !strcmp(argv[c], "-help")) {
      printBanner();
      return 0;
    }
    c++;
  }

  printf("running with parameters\n");
  printf(" serial_device: %s\n", device);
  printf(" baud: %d\n", baudrate);
  printf(" rate: %d Hz\n", rate);

  client = UlisseClient_init(device, baudrate);
  if(!client) {
    printf("Failed\n");
    exit(-1);
  }
  printf("Success\n");

  printf("Synching\n");
  int sync_cycles=50;
  for(int i=0;i<sync_cycles;i++) {
    UlisseCliet_sync(client, 1);
    printf(".");
    fflush(stdout);
  }
  printf(" Done\n");
  UlisseClient_readConfiguration(client, 100);

  printf("Closing\n");
  UlisseClient_destroy(client);
}
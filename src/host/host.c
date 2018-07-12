#include "serial_linux.h"
#include "ulisse_packets.h"
#include "ulisse_print_packets.h"
#include "packet_handler.h"
#include "ulisse_client.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "controller.h"

#include <readline/readline.h>
#include <readline/history.h>

uint8_t exit_flag_thrd=0;

// macro to simplify error handling
#define handle_error(msg)   \
  do {perror(msg); exit(EXIT_FAILURE); } while(0)


// js_handler thread stuff
typedef struct pthread_args_s {
  js_handler* h;
} pthread_args_t;

void* js_handlerFn(void* x) {
  js_handler* h = ((pthread_args_t*)x)->h;
  while(!exit_flag_thrd) {
    js_handler_read(h);
  }
  //js_handler_destroy(h);
}

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
  "-js-device <string>: the serial port for the joystick (default /dev/input/js0)",
  0
};

void printBanner() {
  const char* const* line=banner;
  while(*line) {
    printf("%s\n", *line);
    line++;
  }
}

void* _runnerFn(void* _args) {

}

struct UlisseClient* client;
const char* serial_name = "/dev/ttyACM0";

// Handler to control joystick actions
js_handler jsh;

int main(int argc, char* argv[]) {
  
  int ret;

  if(argc==1) {
    printBanner();
    return 0;
  }
  int c=1;
  const char* device=serial_name;
  const char* js_device=STD_DEVICE;
  uint32_t baudrate = 115200;
  int rate=20;  // Refresh com rate
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
    } else if(!strcmp(argv[c], "-js-device")) {
      c++;
      js_device=argv[c];
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
  printf(" js_device: %s\n", js_device);

  client = UlisseClient_init(device, baudrate);
  if(!client) {
    printf("Failed\n");
    exit(-1);
  }
  printf("Success\n");
  
  // Start js_handler
  printf("starting new thread to handle joystick commandsleep(5);..\n");

  ret=js_handler_initialize(&jsh, js_device);
  if(ret<0) {
    fprintf(stderr, "Error in js_handler_initialize\n");sleep(5);
    exit(EXIT_FAILURE);
  }

  pthread_t js_thrd;
  pthread_args_t js_thrd_args = {&jsh};

  ret=pthread_create(&js_thrd, NULL, js_handlerFn, &js_thrd_args);
  // handle error

  while(1) {
    char *buffer = readline("ulisse> ");
    if(buffer) {
      printf("%s\n", buffer);
    } else {
      break;
    }
  }
  /**
  printf("Synching\n");
  int sync_cycles=50;
  for(int i=0;i<sync_cycles;i++) {
    UlisseClient_sync(client, 1);
    printf(".");
    fflush(stdout);
  }
  UlisseClient_readConfiguration(client, 100);
  printf("Done\n");
  **/

  printf("closing js_handler thread\n");
  exit_flag_thrd=1;
  ret=pthread_join(js_thrd, NULL);
  if(ret<0)handle_error("Error in pthread_join");  

  printf("Closing\n");
  UlisseClient_destroy(client);
}
#include "serial_linux.h"
#include "../common/ulisse_packets.h"
#include "../common/ulisse_print_packets.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// macro to simplify error handling
#define GENERIC_ERROR_HELPER(cond, errCode, msg) do {               \
        if (cond) {                                                 \
            fprintf(stderr, "%s: %s\n", msg, strerror(errCode));    \
            exit(EXIT_FAILURE);                                     \
        }                                                           \
    } while(0)

#define ERROR_HELPER(ret, msg)      GENERIC_ERROR_HELPER((ret < 0), errno, msg)



const char* serial_name = "/dev/ttyACM0";

int serial_write(int fd, char* buf, char* msg, int msg_len) {
    int write_bytes = 0;
    while(msg_len - write_bytes != 0) {
        write_bytes += write(fd, buf+write_bytes, msg_len-write_bytes);
        if( write_bytes < 0) return -1;
    }    
    printf("Written %d bytes\n", write_bytes);
    return 0;
}

int serial_read(int fd, char* buf, int msg_len) {
  int read_bytes = read(fd, buf, msg_len);
  printf("Read %d bytes\n", read_bytes);
  if(read_bytes < 0) return -1;
  if(!read_bytes) return -1;
  return 0;
  /*
  int read_bytes = 0;
  while(msg_len - read_bytes != 0) {
      read_bytes += read(fd, buf+read_bytes, msg_len-read_bytes);
      //printf("Read %d bytes\n", read_bytes);
      printf("Read: %s\n", buf);
      if(read_bytes<0) return -1;
  }
  */

}


void main(int argc, char* argv[]) {
    char buf[256];
    ParamControlPacket pcp = {0};
    INIT_PACKET(pcp, PARAM_CONTROL_PACKET_ID);
    pcp.action=2;
    pcp.param_type=1;   
    ParamControlPacket_print(buf, &pcp);
    printf("%s\n", buf);
    memset(buf, 0, 256);

    int ret;

    printf("Initializing serial interface with %s\n", serial_name);
    int fd = serial_open(serial_name);
    if(fd<0) {
        fprintf(stderr, "Could not open %s ", serial_name);
        exit(EXIT_FAILURE);
    }
    if(serial_set_interface_attribs(fd, 115200, 0)) {
        fprintf(stderr, "Error while setting interface attribs");
        exit(EXIT_FAILURE);
    }
    serial_set_blocking(fd, 1);
    printf("Serial interface ready!\n");
    sleep(1);
    /*
        SENDING STRUCT VIA SERIAL TO ROBOT
    */

    int msg_len = sizeof(pcp);
    while(1){
        memcpy(buf, (void*)&pcp, msg_len);
    
        printf("Writing %s to %s\n", buf, serial_name);
        ret = serial_write(fd, buf, argv[1], msg_len);
        ERROR_HELPER(ret, "Error in write");

        printf("Listening %s\n", serial_name);
        ret = serial_read(fd, buf, sizeof(buf));
        ERROR_HELPER(ret, "Error in read");
        printf("Recived %s\n", buf);
    }
}
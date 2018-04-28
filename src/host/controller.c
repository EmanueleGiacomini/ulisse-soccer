#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)

void controller_reader_func() {
	printf("[reader] Thread built!\n");
	struct js_event e;
	while(1) {
		int fd = open("/dev/input/js0", O_RDONLY);
		if(read(fd, &e, sizeof(e)) != sizeof(e) == -1) {
			handle_error("Error while reading /dev/input/js0");
		}
		/* do something interesting with processed event */
		//printf("Detected an event!\n");
		if(e.type==JS_EVENT_BUTTON) printf("Button pressed!\n");
		if(e.type==JS_EVENT_AXIS) printf("Axis pressed!\n");
	}	
}

#define PAD_LEFT_XNUM 3
#define PAD_LEFT_YNUM 4
#define PAD_RIGHT_XNUM 0
#define PAD_RIGHT_YNUM 1

typedef struct controller_pad {
	int16_t x;
	int16_t y;
}controller_pad;


typedef struct controller_button {
	uint8_t number;
	uint8_t state;
}controller_button;

void main() {
	/*
	pthread_t input_reader;
	if(pthread_create(&input_reader, NULL, &controller_reader_func, NULL)) {
		handle_error("Error while creating input_reader thread\n");
	}
	*/
	int fd = open("/dev/input/js0", O_RDONLY);
	
	struct js_event e;
	controller_pad left_pad, right_pad;
	while(1) {		
		if(read(fd, &e, sizeof(e)) != sizeof(e) == -1) {
			handle_error("Error while reading /dev/input/js0");
		}
		/* do something interesting with processed event */
		//printf("Detected an event!\n");
		/*
		if(e.type==JS_EVENT_BUTTON) printf("Button %d pressed!\n", e.number);
		if(e.type==JS_EVENT_AXIS) printf("Axis %d moved at %d!\n", e.number, e.value);
		*/
		if(e.type==JS_EVENT_AXIS) {
			if(e.number == PAD_LEFT_XNUM) left_pad.x = e.value;
			if(e.number == PAD_LEFT_YNUM) left_pad.y = e.value;
			if(e.number == PAD_RIGHT_XNUM) right_pad.x = e.value;
			if(e.number == PAD_RIGHT_YNUM) right_pad.y = e.value;
			printf("\t[lpad]=(%d, %d)", left_pad.x, left_pad.y);
			printf("\t[rpad]=(%d, %d)", right_pad.x, right_pad.y);
			printf("\n");
		}  
	}	
}

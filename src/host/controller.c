#include "controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)

/** initialize the js_handler. Returns 0 if succesful, -1 otherwhise **/
int js_handler_initialize(js_handler* h) {
	int ret;
	ret=open(STD_DEVICE, O_RDONLY);
	if(ret<0) return -1;
	h->fd=ret;
	return 0;
}

/** reads data from the joystick. each new event will be 
 *  enqueued in a given list.
 *  Operation is set in Blocking mode so the function will
 *  stand still until a new event is sent. **/
int js_handler_read(js_handler* h) {
	int ret;
	struct js_event e;

	ret=read(h->fd, &e, sizeof(e));
	if(ret<=0) return -1;
	if(ret==sizeof(struct js_event)) {
		printf("Received new event\n");
		if(e.type=JS_EVENT_AXIS) {
		// Received event is an axis event
		switch(e.number) {
			case PAD_LEFT_XNUM: {
				h->left_pad.x=e.value;
			} break;
			case PAD_LEFT_YNUM: {
				h->left_pad.y=e.value;
			} break;
			case PAD_RIGHT_XNUM: {
				h->right_pad.x=e.value;
			} break;
			case PAD_RIGHT_YNUM: {
				h->right_pad.y=e.value;
			} break;
		}
		} else if(e.type=JS_EVENT_BUTTON) {
			// Received event is a button event
			// update js_handler status
		}
	}
	
	return 0;
}



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

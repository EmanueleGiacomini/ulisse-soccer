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
int js_handler_initialize(js_handler* h, char* device) {
	int ret;
	ret=open(device, O_RDONLY);
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
	if(ret==sizeof(e)) {
		//printf("read %d bytes\n", ret);
		if(e.type==JS_EVENT_AXIS) {
		// Received event is an axis event
		//printf("Received new axis event\n");
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
		} else if(e.type==JS_EVENT_BUTTON) {
			//printf("Received new button event\n");
			// Received event is a button event
			// update js_handler status
		}
	}
	
	return 0;
}


static void js_pad_print(js_pad* p) {
	printf("x=%d\ny=%d\n", p->x, p->y);
}

static void js_button_print(js_button* p) {
	printf("state=%d\n", p->state);
}
/** print js_handler state **/
void js_handler_print(js_handler* h) {
	printf("[handler]{\nfd=%d\n", h->fd);
	printf(" [right_pad]=\n");
	js_pad_print(&h->left_pad);
	printf(" [left_pad]=\n");
	js_pad_print(&h->right_pad);
	printf("}\n");
}


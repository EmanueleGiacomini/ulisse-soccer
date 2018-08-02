#ifndef queue_h
#define queue_h

#include <stdint.h>

typedef struct queue queue;

queue* queue_init();
void queue_enqueue(queue* q, void* data);
void* queue_dequeue(queue* q);
uint8_t queue_size(queue* q);

#endif
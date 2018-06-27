#pragma once

typedef struct queue queue;

/** Return pointer to the new allocated queue struct **/
queue* queue_alloc();

/** Enqueue data in the queue q **/
void queue_enqueue(queue* q, void* data);

/** Dequeue an item from the queue q, then returns a pointer to it **/
void* queue_dequeue(queue* q);

/** Return the current size of the queue **/
unsigned int queue_size(queue* q);

/** deallocate the memory needed for the queue q **/
// OSS: It doesn't deallocate the items
void queue_destroy(queue* q);

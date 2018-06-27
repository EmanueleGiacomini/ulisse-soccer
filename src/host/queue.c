#include "queue.h"
#include <stdlib.h>

typedef struct queue_node {
  void* data;
  struct queue_node* next;
} queue_node;

typedef struct queue {
  queue_node* head;
  queue_node* tail;
  int size;
} queue;

/** Return pointer to the new allocated queue struct **/\
queue* queue_alloc() {
  return (queue*)calloc(sizeof(queue), 1);
}

static queue_node* queue_node_alloc(void* data) {
  queue_node* node = (queue_node*)calloc(sizeof(queue_node), 1);
  node->data=data;
  return node;
}

/** Enqueue data in the queue q **/
void queue_enqueue(queue* q, void* data) {
  if(data==NULL)
    return;
  q->tail->next=queue_node_alloc(data);
  q->tail=q->tail->next;
  ++q->size;
}

/** Dequeue an item from the queue q, then returns a pointer to it **/
void* queue_dequeue(queue* q) {
  if(!q->size)
    return NULL;
  queue_node* dequeued = q->head;
  void* deq_data = dequeued->data;
  q->head=q->head->next;
  --q->size;
  free(dequeued);
  return deq_data;  
}

/** Return the current size of the queue **/
unsigned int queue_size(queue* q) {
  return q->size;
}


static void queue_destroy_r(queue_node* n) {
  if(n==NULL) return;
  queue_destroy_r(n->next);
  free(n);
}

/** deallocate the memory needed for the queue q **/
// OSS: It doesn't deallocate the items
void queue_destroy(queue* q) {
  queue_destroy_r(q->head);
  free(q);
}
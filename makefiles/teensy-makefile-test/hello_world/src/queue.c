#include "queue.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct qnode {
  void* data;
  struct qnode* next;
} qnode;

typedef struct queue {
  qnode* head;
  qnode* tail;
  uint8_t size;
} queue;


queue* queue_init() {
  return (queue*)calloc(1,sizeof(queue));
}

void queue_enqueue(queue* q, void* data) {
  qnode* n = (qnode*)calloc(1,sizeof(qnode));
  n->data=data;
  if(!q->size) {
    q->head=n;
    q->tail=n;
    ++q->size;
    return;
  }
  q->tail->next=n;
  q->tail=n;
  ++q->size;
  return;
}

void* queue_dequeue(queue* q) {
  return NULL;
}

uint8_t queue_size(queue* q) {
  if(q==NULL)
    return 0;
  return q->size;
}
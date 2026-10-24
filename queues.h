/* queues.h
 * LinkedQueue          : FIFO queue built from a linked list (front + rear pointers).
 * PriorityQueueManager : three queues (Urgent, High, Normal). Always serves the first
 *                        non-empty queue, so urgent complaints go first and complaints
 *                        of equal priority are served in arrival order. */
#ifndef QUEUES_H
#define QUEUES_H

#include "complaint.h"

typedef struct QueueNode {
    Complaint *complaint;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
} LinkedQueue;

void       queue_init(LinkedQueue *q);
void       queue_enqueue(LinkedQueue *q, Complaint *c);   /* O(1) */
Complaint *queue_dequeue(LinkedQueue *q);                 /* O(1), NULL if empty */
Complaint *queue_peek(const LinkedQueue *q);
int        queue_is_empty(const LinkedQueue *q);
void       queue_free(LinkedQueue *q);                    /* frees nodes, not complaints */

typedef struct {
    LinkedQueue queues[PRIORITY_COUNT];
} PriorityQueueManager;

void       pqm_init(PriorityQueueManager *m);
void       pqm_enqueue(PriorityQueueManager *m, Complaint *c);
Complaint *pqm_dequeue_next(PriorityQueueManager *m);     /* Urgent -> High -> Normal */
int        pqm_count(const PriorityQueueManager *m, Priority p);
int        pqm_total(const PriorityQueueManager *m);
void       pqm_free(PriorityQueueManager *m);

#endif

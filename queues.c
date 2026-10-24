#include <stdlib.h>

#include "queues.h"
#include "util.h"

void queue_init(LinkedQueue *q)
{
    q->front = q->rear = NULL;
    q->size = 0;
}

/* Add at the rear. O(1). */
void queue_enqueue(LinkedQueue *q, Complaint *c)
{
    QueueNode *node = xmalloc(sizeof(QueueNode));
    node->complaint = c;
    node->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
}

/* Remove from the front. O(1). */
Complaint *queue_dequeue(LinkedQueue *q)
{
    QueueNode *node;
    Complaint *c;
    if (q->front == NULL)
        return NULL;
    node = q->front;
    c = node->complaint;
    q->front = node->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(node);
    q->size--;
    return c;
}

Complaint *queue_peek(const LinkedQueue *q)
{
    return q->front ? q->front->complaint : NULL;
}

int queue_is_empty(const LinkedQueue *q)
{
    return q->size == 0;
}

void queue_free(LinkedQueue *q)
{
    while (queue_dequeue(q) != NULL)
        ;
}

void pqm_init(PriorityQueueManager *m)
{
    int p;
    for (p = 0; p < PRIORITY_COUNT; p++)
        queue_init(&m->queues[p]);
}

void pqm_enqueue(PriorityQueueManager *m, Complaint *c)
{
    queue_enqueue(&m->queues[c->priority], c);
}

Complaint *pqm_dequeue_next(PriorityQueueManager *m)
{
    int p;
    for (p = 0; p < PRIORITY_COUNT; p++)            /* Urgent -> High -> Normal */
        if (!queue_is_empty(&m->queues[p]))
            return queue_dequeue(&m->queues[p]);
    return NULL;
}

int pqm_count(const PriorityQueueManager *m, Priority p)
{
    return m->queues[p].size;
}

int pqm_total(const PriorityQueueManager *m)
{
    int p, total = 0;
    for (p = 0; p < PRIORITY_COUNT; p++)
        total += m->queues[p].size;
    return total;
}

void pqm_free(PriorityQueueManager *m)
{
    int p;
    for (p = 0; p < PRIORITY_COUNT; p++)
        queue_free(&m->queues[p]);
}

#include <stdio.h>
#include <stdlib.h>

#include "status_list.h"
#include "util.h"

const char *STATUS_NAMES[STATUS_COUNT] = {"Filed", "Assigned", "In Progress", "Resolved"};

void history_init(StatusHistory *h)
{
    h->head = NULL;
    h->tail = NULL;
    h->size = 0;
}

/* Add a status at the end of the list. O(1) thanks to the tail pointer. */
void history_append(StatusHistory *h, Status status, const char *remarks, long day)
{
    StatusNode *node = xmalloc(sizeof(StatusNode));
    node->status = status;
    node->day = day;
    copy_str(node->remarks, REMARK_LEN, remarks);
    node->next = NULL;

    if (h->head == NULL) {
        h->head = node;
        h->tail = node;
    } else {
        h->tail->next = node;
        h->tail = node;
    }
    h->size++;
}

/* Remove the latest status (used by Undo). O(k), k = list length (max 4). */
int history_remove_last(StatusHistory *h)
{
    StatusNode *current;
    if (h->size <= 1)
        return 0;                               /* the first status can never be removed */
    current = h->head;
    while (current->next != h->tail)            /* find the node before the tail */
        current = current->next;
    free(h->tail);
    current->next = NULL;
    h->tail = current;
    h->size--;
    return 1;
}

Status history_last_status(const StatusHistory *h)
{
    return h->tail->status;
}

long history_last_day(const StatusHistory *h)
{
    return h->tail->day;
}

/* Traverse from head to tail and print every status. */
void history_display(const StatusHistory *h)
{
    const StatusNode *node = h->head;
    int step = 1;
    char date[16];
    while (node != NULL) {
        format_day(node->day, date);
        printf("  %d. %-12s %s", step, STATUS_NAMES[node->status], date);
        if (node->remarks[0] != '\0')
            printf(" - %s", node->remarks);
        printf("\n");
        node = node->next;
        step++;
    }
}

void history_free(StatusHistory *h)
{
    StatusNode *node = h->head;
    while (node != NULL) {
        StatusNode *next = node->next;
        free(node);
        node = next;
    }
    h->head = h->tail = NULL;
    h->size = 0;
}

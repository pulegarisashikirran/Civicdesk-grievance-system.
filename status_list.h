/* status_list.h - singly linked list holding the status history of ONE complaint */
#ifndef STATUS_LIST_H
#define STATUS_LIST_H

#define REMARK_LEN 64

typedef enum {
    STATUS_FILED = 0,
    STATUS_ASSIGNED,
    STATUS_IN_PROGRESS,
    STATUS_RESOLVED,
    STATUS_COUNT
} Status;

extern const char *STATUS_NAMES[STATUS_COUNT];

typedef struct StatusNode {
    Status status;
    long day;
    char remarks[REMARK_LEN];
    struct StatusNode *next;
} StatusNode;

typedef struct {
    StatusNode *head;
    StatusNode *tail;
    int size;
} StatusHistory;

void   history_init(StatusHistory *h);
void   history_append(StatusHistory *h, Status status, const char *remarks, long day); /* O(1) */
int    history_remove_last(StatusHistory *h);   /* 1 if removed; "Filed" is never removed */
Status history_last_status(const StatusHistory *h);
long   history_last_day(const StatusHistory *h);
void   history_display(const StatusHistory *h);
void   history_free(StatusHistory *h);

#endif

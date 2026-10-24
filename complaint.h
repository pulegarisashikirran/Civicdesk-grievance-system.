/* complaint.h - the Complaint record. Every complaint owns a StatusHistory (linked list). */
#ifndef COMPLAINT_H
#define COMPLAINT_H

#include "status_list.h"

#define NAME_LEN 32
#define AREA_LEN 32
#define DEPT_LEN 24

typedef enum {
    PRIORITY_URGENT = 0,
    PRIORITY_HIGH,
    PRIORITY_NORMAL,
    PRIORITY_COUNT
} Priority;

#define DEPARTMENT_COUNT 5

extern const char *PRIORITY_NAMES[PRIORITY_COUNT];
extern const char *DEPARTMENTS[DEPARTMENT_COUNT];

typedef struct Complaint {
    int  cid;                    /* unique ID (BST key)   */
    char name[NAME_LEN];         /* citizen name          */
    char area[AREA_LEN];         /* locality / ward       */
    char department[DEPT_LEN];
    Priority priority;
    long filed_on;               /* day number            */
    StatusHistory history;       /* linked list of statuses */
} Complaint;

Complaint *complaint_create(int cid, const char *name, const char *area,
                            const char *department, Priority priority, long filed_on);
Status complaint_status(const Complaint *c);
int    complaint_pending_days(const Complaint *c);  /* stops counting once Resolved */
void   complaint_free(Complaint *c);

#endif

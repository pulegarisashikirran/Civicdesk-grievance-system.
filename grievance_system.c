#include <stdlib.h>

#include "grievance_system.h"
#include "util.h"

void system_init(GrievanceSystem *sys)
{
    bst_init(&sys->bst);
    pqm_init(&sys->queues);
    undo_init(&sys->undo);
    sys->capacity = 16;
    sys->count = 0;
    sys->all = xmalloc((size_t)sys->capacity * sizeof(Complaint *));
}

int system_add_complaint(GrievanceSystem *sys, Complaint *c)
{
    if (!bst_insert(&sys->bst, c))                  /* rejects duplicate IDs */
        return 0;

    if (sys->count == sys->capacity) {              /* grow the array when it is full */
        Complaint **bigger = xmalloc((size_t)sys->capacity * 2 * sizeof(Complaint *));
        int i;
        for (i = 0; i < sys->count; i++)
            bigger[i] = sys->all[i];
        free(sys->all);
        sys->all = bigger;
        sys->capacity *= 2;
    }
    sys->all[sys->count++] = c;

    if (complaint_status(c) == STATUS_FILED)        /* only unprocessed complaints wait in a queue */
        pqm_enqueue(&sys->queues, c);
    return 1;
}

int system_new_id(const GrievanceSystem *sys)
{
    for (;;) {
        int cid = 1000 + rand() % 9000;             /* random IDs keep the BST reasonably balanced */
        if (bst_search(&sys->bst, cid) == NULL)
            return cid;
    }
}

void system_free(GrievanceSystem *sys)
{
    int i;
    pqm_free(&sys->queues);
    undo_free(&sys->undo);
    bst_free(&sys->bst);
    for (i = 0; i < sys->count; i++)
        complaint_free(sys->all[i]);
    free(sys->all);
    sys->all = NULL;
    sys->count = sys->capacity = 0;
}

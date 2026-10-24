/* grievance_system.h - ties the data structures together:
 *   BST (lookup by ID) + three queues (processing order) + undo stack + list of all complaints */
#ifndef GRIEVANCE_SYSTEM_H
#define GRIEVANCE_SYSTEM_H

#include "bst.h"
#include "queues.h"
#include "undo_stack.h"

typedef struct {
    ComplaintBST bst;
    PriorityQueueManager queues;
    UndoStack undo;
    Complaint **all;          /* every complaint, in filing order (for linear search / sorting) */
    int count;
    int capacity;
} GrievanceSystem;

void system_init(GrievanceSystem *sys);
int  system_add_complaint(GrievanceSystem *sys, Complaint *c);  /* 0 if the ID already exists */
int  system_new_id(const GrievanceSystem *sys);                 /* random unused 4-digit ID    */
void system_free(GrievanceSystem *sys);                         /* frees everything it owns    */

#endif

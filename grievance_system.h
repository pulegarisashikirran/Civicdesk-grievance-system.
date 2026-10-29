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

/* Data files: a plain text file with one line per complaint and one line per later status. */
int  system_save(const GrievanceSystem *sys, const char *path);  /* 0 on success, -1 on failure */
/* Replaces ALL current data with the file's content. Returns the number of complaints loaded,
 * or -1 on failure (a message goes into err and the current data is left untouched). */
int  system_load(GrievanceSystem *sys, const char *path, char *err, size_t err_size);

#endif

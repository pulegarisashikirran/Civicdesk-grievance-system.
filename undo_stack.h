/* undo_stack.h - Stack (LIFO, built from a linked list) used for Undo.
 * Each record stores: complaint ID, previous status, new status. */
#ifndef UNDO_STACK_H
#define UNDO_STACK_H

#include "status_list.h"

typedef struct {
    int cid;
    Status previous;
    Status current;
} UndoRecord;

typedef struct StackNode {
    UndoRecord record;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
    int size;
} UndoStack;

void undo_init(UndoStack *s);
void undo_push(UndoStack *s, UndoRecord record);       /* O(1) */
int  undo_pop(UndoStack *s, UndoRecord *out);          /* O(1), 0 if empty */
int  undo_peek(const UndoStack *s, UndoRecord *out);   /* O(1), 0 if empty */
int  undo_is_empty(const UndoStack *s);
void undo_free(UndoStack *s);

#endif

#include <stdlib.h>

#include "undo_stack.h"
#include "util.h"

void undo_init(UndoStack *s)
{
    s->top = NULL;
    s->size = 0;
}

void undo_push(UndoStack *s, UndoRecord record)
{
    StackNode *node = xmalloc(sizeof(StackNode));
    node->record = record;
    node->next = s->top;            /* new node points to the old top */
    s->top = node;
    s->size++;
}

int undo_pop(UndoStack *s, UndoRecord *out)
{
    StackNode *node;
    if (s->top == NULL)
        return 0;
    node = s->top;
    *out = node->record;
    s->top = node->next;
    free(node);
    s->size--;
    return 1;
}

int undo_peek(const UndoStack *s, UndoRecord *out)
{
    if (s->top == NULL)
        return 0;
    *out = s->top->record;
    return 1;
}

int undo_is_empty(const UndoStack *s)
{
    return s->top == NULL;
}

void undo_free(UndoStack *s)
{
    UndoRecord ignored;
    while (undo_pop(s, &ignored))
        ;
}

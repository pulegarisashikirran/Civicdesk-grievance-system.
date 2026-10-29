#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* ------------------------------------------------------------------ data files
 * Format (one record per line, fields separated by |):
 *   CIVICDESK 1
 *   C|id|name|area|department|priority(0-2)|filed_day        one line per complaint
 *   S|status(1-3)|day|remarks                                 later statuses of that complaint
 * "Filed" is implied by the C line, so only Assigned / In Progress / Resolved are stored.
 */
#define DATA_HEADER "CIVICDESK 1"

static void clean_text(const char *src, char *dst, size_t size)
{
    size_t i = 0;
    while (src[i] != '\0' && i + 1 < size) {        /* the separator and line breaks cannot be stored */
        dst[i] = (src[i] == '|' || src[i] == '\n' || src[i] == '\r') ? ' ' : src[i];
        i++;
    }
    dst[i] = '\0';
}

int system_save(const GrievanceSystem *sys, const char *path)
{
    FILE *f = fopen(path, "w");
    char name[NAME_LEN], area[AREA_LEN], dept[DEPT_LEN], remarks[REMARK_LEN];
    int i;

    if (f == NULL)
        return -1;
    fprintf(f, "%s\n", DATA_HEADER);
    for (i = 0; i < sys->count; i++) {
        const Complaint *c = sys->all[i];
        const StatusNode *node;
        clean_text(c->name, name, sizeof name);
        clean_text(c->area, area, sizeof area);
        clean_text(c->department, dept, sizeof dept);
        fprintf(f, "C|%d|%s|%s|%s|%d|%ld\n", c->cid, name, area, dept, (int)c->priority, c->filed_on);
        for (node = c->history.head->next; node != NULL; node = node->next) {
            clean_text(node->remarks, remarks, sizeof remarks);
            fprintf(f, "S|%d|%ld|%s\n", (int)node->status, node->day, remarks);
        }
    }
    if (ferror(f)) {
        fclose(f);
        return -1;
    }
    return fclose(f) == 0 ? 0 : -1;
}

/* Splits line in place at every | and stores up to max field pointers. Returns the count, or -1. */
static int split_fields(char *line, char **fields, int max)
{
    int n = 0;
    fields[n++] = line;
    for (; *line != '\0'; line++) {
        if (*line == '|') {
            if (n == max)
                return -1;
            *line = '\0';
            fields[n++] = line + 1;
        }
    }
    return n;
}

static int parse_long(const char *text, long *out)
{
    char *end;
    if (text[0] == '\0')
        return 0;
    *out = strtol(text, &end, 10);
    return *end == '\0';
}

static void set_error(char *err, size_t size, const char *message, int line)
{
    if (err == NULL || size == 0)
        return;
    if (line > 0)
        snprintf(err, size, "line %d: %s", line, message);
    else
        snprintf(err, size, "%s", message);
}

/* Adds a finished complaint to the system being built. Returns 0 if its ID is a duplicate. */
static int finish_complaint(GrievanceSystem *tmp, Complaint **current, int *loaded)
{
    int ok = 1;
    if (*current != NULL) {
        if (system_add_complaint(tmp, *current)) {
            (*loaded)++;
        } else {
            complaint_free(*current);
            ok = 0;
        }
        *current = NULL;
    }
    return ok;
}

int system_load(GrievanceSystem *sys, const char *path, char *err, size_t err_size)
{
    FILE *f = fopen(path, "r");
    GrievanceSystem tmp;
    Complaint *current = NULL;
    char line[512];
    char *fields[8];
    const char *why = "";
    int line_no = 1, loaded = 0, failed = 0;

    if (f == NULL) {
        set_error(err, err_size, "the file could not be opened", 0);
        return -1;
    }
    if (fgets(line, sizeof line, f) == NULL || strncmp(line, DATA_HEADER, strlen(DATA_HEADER)) != 0) {
        fclose(f);
        set_error(err, err_size, "this is not a CivicDesk data file", 0);
        return -1;
    }

    system_init(&tmp);                          /* build the new data on the side first */
    while (!failed && fgets(line, sizeof line, f) != NULL) {
        size_t len = strlen(line);
        int nf;
        line_no++;
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len == 0)
            continue;

        nf = split_fields(line, fields, 8);
        if (nf == 7 && strcmp(fields[0], "C") == 0) {
            long id, priority, filed;
            if (!finish_complaint(&tmp, &current, &loaded)) {
                failed = 1;
                why = "duplicate complaint ID";
            } else if (!parse_long(fields[1], &id) || !parse_long(fields[5], &priority) ||
                       !parse_long(fields[6], &filed) || id < 1 || id > 999999 ||
                       priority < 0 || priority >= PRIORITY_COUNT || filed < 0 ||
                       fields[2][0] == '\0' || fields[3][0] == '\0' || fields[4][0] == '\0') {
                failed = 1;
                why = "invalid complaint record";
            } else {
                current = complaint_create((int)id, fields[2], fields[3], fields[4],
                                           (Priority)priority, filed);
            }
        } else if (nf == 4 && strcmp(fields[0], "S") == 0 && current != NULL) {
            long status, day;
            if (!parse_long(fields[1], &status) || !parse_long(fields[2], &day) ||
                status < 1 || status >= STATUS_COUNT ||
                (Status)status <= complaint_status(current) ||
                day < history_last_day(&current->history)) {
                failed = 1;
                why = "invalid status record";
            } else {
                history_append(&current->history, (Status)status, fields[3], day);
            }
        } else {
            failed = 1;
            why = "unrecognised line";
        }
    }
    fclose(f);

    if (!failed && !finish_complaint(&tmp, &current, &loaded)) {
        failed = 1;
        why = "duplicate complaint ID";
    }
    if (failed) {
        complaint_free(current);                /* NULL is fine */
        system_free(&tmp);
        set_error(err, err_size, why, line_no);
        return -1;
    }

    system_free(sys);                           /* success: swap in the new data */
    *sys = tmp;
    return loaded;
}

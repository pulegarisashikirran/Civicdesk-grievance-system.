#include <stdlib.h>

#include "complaint.h"
#include "util.h"

const char *PRIORITY_NAMES[PRIORITY_COUNT] = {"Urgent", "High", "Normal"};
const char *DEPARTMENTS[DEPARTMENT_COUNT] = {
    "Water Supply", "Electricity", "Roads", "Sanitation", "Street Lights"
};

Complaint *complaint_create(int cid, const char *name, const char *area,
                            const char *department, Priority priority, long filed_on)
{
    Complaint *c = xmalloc(sizeof(Complaint));
    c->cid = cid;
    copy_str(c->name, NAME_LEN, name);
    copy_str(c->area, AREA_LEN, area);
    copy_str(c->department, DEPT_LEN, department);
    c->priority = priority;
    c->filed_on = filed_on;
    history_init(&c->history);
    history_append(&c->history, STATUS_FILED, "Complaint registered", filed_on);
    return c;
}

Status complaint_status(const Complaint *c)
{
    return history_last_status(&c->history);
}

int complaint_pending_days(const Complaint *c)
{
    long end = (complaint_status(c) == STATUS_RESOLVED) ? history_last_day(&c->history)
                                                        : today_day();
    return (int)(end - c->filed_on);
}

void complaint_free(Complaint *c)
{
    if (c == NULL)
        return;
    history_free(&c->history);
    free(c);
}

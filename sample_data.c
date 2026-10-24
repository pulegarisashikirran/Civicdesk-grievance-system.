#include <stdlib.h>

#include "sample_data.h"
#include "util.h"

typedef struct {
    int id;
    const char *name;
    const char *area;
    const char *department;
    Priority priority;
    int days_ago;
    int steps;            /* how many statuses after "Filed": 0..3 */
} SampleRow;

#define SAMPLE_COUNT 12

static const SampleRow SAMPLE_ROWS[SAMPLE_COUNT] = {
    {4821, "Ravi",    "Shadnagar",     "Water Supply",  PRIORITY_URGENT, 2,  0},
    {1307, "Anita",   "Gachibowli",    "Electricity",   PRIORITY_HIGH,   5,  0},
    {7752, "Kiran",   "Madhapur",      "Roads",         PRIORITY_NORMAL, 9,  0},
    {2194, "Sneha",   "Kukatpally",    "Sanitation",    PRIORITY_NORMAL, 1,  0},
    {9036, "Imran",   "Uppal",         "Water Supply",  PRIORITY_URGENT, 3,  0},
    {5518, "Lakshmi", "Secunderabad",  "Street Lights", PRIORITY_NORMAL, 12, 0},
    {3340, "Suresh",  "Banjara Hills", "Electricity",   PRIORITY_HIGH,   7,  0},
    {6671, "Priya",   "Miyapur",       "Roads",         PRIORITY_HIGH,   15, 2},
    {1889, "Arjun",   "Gachibowli",    "Sanitation",    PRIORITY_URGENT, 20, 3},
    {8203, "Fatima",  "Uppal",         "Street Lights", PRIORITY_NORMAL, 10, 1},
    {4067, "Mahesh",  "Shadnagar",     "Water Supply",  PRIORITY_HIGH,   4,  0},
    {7129, "Divya",   "Kukatpally",    "Electricity",   PRIORITY_NORMAL, 6,  0},
};

Complaint **load_sample_complaints(int *count)
{
    Complaint **result = xmalloc(SAMPLE_COUNT * sizeof(Complaint *));
    long today = today_day();
    int i, s;

    for (i = 0; i < SAMPLE_COUNT; i++) {
        const SampleRow *row = &SAMPLE_ROWS[i];
        long filed = today - row->days_ago;
        Complaint *c = complaint_create(row->id, row->name, row->area, row->department,
                                        row->priority, filed);
        for (s = 1; s <= row->steps; s++)       /* Assigned, In Progress, Resolved */
            history_append(&c->history, (Status)s, "Sample data", filed + s);
        result[i] = c;
    }
    *count = SAMPLE_COUNT;
    return result;
}

/* ---------------------------------------------- random data for benchmarks */
static const char *NAMES[] = {"Ravi", "Anita", "Kiran", "Sneha", "Imran", "Lakshmi",
                              "Suresh", "Priya", "Arjun", "Fatima", "Mahesh", "Divya",
                              "Rohit", "Sana", "Venkat", "Meena"};
static const char *AREAS[] = {"Shadnagar", "Gachibowli", "Madhapur", "Kukatpally",
                              "Uppal", "Secunderabad", "Banjara Hills", "Miyapur"};

#define NAME_TOTAL ((int)(sizeof(NAMES) / sizeof(NAMES[0])))
#define AREA_TOTAL ((int)(sizeof(AREAS) / sizeof(AREAS[0])))
#define ID_MIN 1000
#define ID_MAX 99999

/* xorshift generator: gives the same numbers on every compiler and machine */
static unsigned long long rng_state;

static unsigned rng_next(void)
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return (unsigned)(rng_state >> 16);
}

Complaint **generate_random_complaints(int n, unsigned long seed)
{
    int pool_size = ID_MAX - ID_MIN + 1;
    int *pool = xmalloc((size_t)pool_size * sizeof(int));
    Complaint **result = xmalloc((size_t)n * sizeof(Complaint *));
    long today = today_day();
    int i;

    rng_state = (unsigned long long)seed * 2654435761ULL + 88172645463325252ULL;
    for (i = 0; i < pool_size; i++)
        pool[i] = ID_MIN + i;

    for (i = 0; i < n; i++) {                       /* partial Fisher-Yates shuffle -> unique IDs */
        int j = i + (int)(rng_next() % (unsigned)(pool_size - i));
        int tmp = pool[i];
        pool[i] = pool[j];
        pool[j] = tmp;
        result[i] = complaint_create(pool[i],
                                     NAMES[rng_next() % NAME_TOTAL],
                                     AREAS[rng_next() % AREA_TOTAL],
                                     DEPARTMENTS[rng_next() % DEPARTMENT_COUNT],
                                     (Priority)(rng_next() % PRIORITY_COUNT),
                                     today - (long)(rng_next() % 61));
    }
    free(pool);
    return result;
}

void free_complaints(Complaint **items, int n)
{
    int i;
    for (i = 0; i < n; i++)
        complaint_free(items[i]);
    free(items);
}

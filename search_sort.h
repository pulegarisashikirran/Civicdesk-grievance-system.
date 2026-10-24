/* search_sort.h
 * Searching : linear_search(), binary_search()
 * Sorting   : bubble, selection, insertion, merge, quick sort
 * Every sort works IN PLACE on an array of Complaint pointers, ordered by a comparator.
 * Benchmark helpers time the algorithms on the same data. */
#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "complaint.h"

/* ------------------------------------------------------------ comparators */
typedef int (*CmpFn)(const Complaint *a, const Complaint *b);   /* <0, 0, >0 */

int cmp_pending_asc(const Complaint *a, const Complaint *b);
int cmp_pending_desc(const Complaint *a, const Complaint *b);   /* longest pending first */
int cmp_area(const Complaint *a, const Complaint *b);
int cmp_id(const Complaint *a, const Complaint *b);
int cmp_priority(const Complaint *a, const Complaint *b);

#define SORT_KEY_COUNT 4
extern const char *SORT_KEY_NAMES[SORT_KEY_COUNT];
extern const CmpFn SORT_KEY_COMPARATORS[SORT_KEY_COUNT];

/* -------------------------------------------------------------- searching */
typedef int (*MatchFn)(const Complaint *c, const void *arg);    /* 1 if it matches */

int match_name(const Complaint *c, const void *arg);            /* name contains text */
int match_area(const Complaint *c, const void *arg);            /* area contains text */
int match_id(const Complaint *c, const void *arg);              /* arg is an int*     */

/* Scans items[0..n-1]; copies matches into out (size >= n); returns how many matched. */
int linear_search(Complaint **items, int n, MatchFn match, const void *arg, Complaint **out);

/* sorted_items must be sorted by complaint ID. Returns the complaint or NULL. */
Complaint *binary_search(Complaint **sorted_items, int n, int target_id);

/* ---------------------------------------------------------------- sorting */
typedef void (*SortFn)(Complaint **items, int n, CmpFn cmp);

void bubble_sort(Complaint **items, int n, CmpFn cmp);
void selection_sort(Complaint **items, int n, CmpFn cmp);
void insertion_sort(Complaint **items, int n, CmpFn cmp);
void merge_sort(Complaint **items, int n, CmpFn cmp);
void quick_sort(Complaint **items, int n, CmpFn cmp);

#define SORT_ALGORITHM_COUNT 5
extern const char *SORT_NAMES[SORT_ALGORITHM_COUNT];
extern const SortFn SORT_FUNCTIONS[SORT_ALGORITHM_COUNT];

Complaint **copy_array(Complaint **items, int n);               /* caller free()s */

/* ------------------------------------------------------------- benchmarks */
typedef struct {
    int n;
    double ms[SORT_ALGORITHM_COUNT];
} SortBenchRow;

typedef struct {
    int n;
    int lookups;
    int bst_height;
    double linear_ms;
    double binary_ms;
    double bst_ms;
} SearchBenchResult;

void benchmark_sorting(const int *sizes, int count, SortBenchRow *rows);
SearchBenchResult benchmark_searching(int n, int lookups);

#endif

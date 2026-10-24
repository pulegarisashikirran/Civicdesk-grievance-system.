#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "search_sort.h"
#include "bst.h"
#include "sample_data.h"
#include "util.h"

/* ------------------------------------------------------------ comparators */
int cmp_pending_asc(const Complaint *a, const Complaint *b)
{
    int x = complaint_pending_days(a);
    int y = complaint_pending_days(b);
    return (x > y) - (x < y);
}

int cmp_pending_desc(const Complaint *a, const Complaint *b)
{
    return cmp_pending_asc(b, a);
}

int cmp_area(const Complaint *a, const Complaint *b)
{
    return ci_compare(a->area, b->area);
}

int cmp_id(const Complaint *a, const Complaint *b)
{
    return (a->cid > b->cid) - (a->cid < b->cid);
}

int cmp_priority(const Complaint *a, const Complaint *b)
{
    return (int)a->priority - (int)b->priority;
}

const char *SORT_KEY_NAMES[SORT_KEY_COUNT] = {
    "Pending days (longest first)", "Area (A-Z)", "Complaint ID", "Priority"
};
const CmpFn SORT_KEY_COMPARATORS[SORT_KEY_COUNT] = {
    cmp_pending_desc, cmp_area, cmp_id, cmp_priority
};

/* -------------------------------------------------------------- searching */
int match_name(const Complaint *c, const void *arg)
{
    return ci_contains(c->name, (const char *)arg);
}

int match_area(const Complaint *c, const void *arg)
{
    return ci_contains(c->area, (const char *)arg);
}

int match_id(const Complaint *c, const void *arg)
{
    return c->cid == *(const int *)arg;
}

/* Check every item from index 0 to n-1. Works on unsorted data. O(n) time, O(1) space. */
int linear_search(Complaint **items, int n, MatchFn match, const void *arg, Complaint **out)
{
    int i, found = 0;
    for (i = 0; i < n; i++)
        if (match(items[i], arg))
            out[found++] = items[i];
    return found;
}

/* Iterative binary search on a list sorted by ID. O(log n) time, O(1) space. */
Complaint *binary_search(Complaint **sorted_items, int n, int target_id)
{
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int mid_id = sorted_items[mid]->cid;
        if (mid_id == target_id)
            return sorted_items[mid];
        if (mid_id < target_id)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return NULL;
}

/* ---------------------------------------------------------------- sorting */
static void swap(Complaint **a, Complaint **b)
{
    Complaint *t = *a;
    *a = *b;
    *b = t;
}

/* Swap adjacent out-of-order items. Stops early if a pass makes no swap. */
void bubble_sort(Complaint **items, int n, CmpFn cmp)
{
    int i, j;
    for (i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            if (cmp(items[j], items[j + 1]) > 0) {
                swap(&items[j], &items[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

/* Repeatedly pick the smallest remaining item and put it in place. */
void selection_sort(Complaint **items, int n, CmpFn cmp)
{
    int i, j;
    for (i = 0; i < n - 1; i++) {
        int smallest = i;
        for (j = i + 1; j < n; j++)
            if (cmp(items[j], items[smallest]) < 0)
                smallest = j;
        if (smallest != i)
            swap(&items[i], &items[smallest]);
    }
}

/* Grow a sorted prefix by inserting each new item into its correct place. */
void insertion_sort(Complaint **items, int n, CmpFn cmp)
{
    int i, j;
    for (i = 1; i < n; i++) {
        Complaint *current = items[i];
        j = i - 1;
        while (j >= 0 && cmp(items[j], current) > 0) {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = current;
    }
}

/* Merge sort on the range [lo, hi). tmp is a work buffer of the same size. */
static void merge_range(Complaint **a, Complaint **tmp, int lo, int hi, CmpFn cmp)
{
    int mid, i, j, k;
    if (hi - lo < 2)
        return;
    mid = lo + (hi - lo) / 2;
    merge_range(a, tmp, lo, mid, cmp);
    merge_range(a, tmp, mid, hi, cmp);

    i = lo;
    j = mid;
    k = lo;
    while (i < mid && j < hi) {
        if (cmp(a[i], a[j]) <= 0)           /* <= keeps the sort stable */
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }
    while (i < mid)
        tmp[k++] = a[i++];
    while (j < hi)
        tmp[k++] = a[j++];
    for (k = lo; k < hi; k++)
        a[k] = tmp[k];
}

void merge_sort(Complaint **items, int n, CmpFn cmp)
{
    Complaint **tmp;
    if (n < 2)
        return;
    tmp = xmalloc((size_t)n * sizeof(Complaint *));
    merge_range(items, tmp, 0, n, cmp);
    free(tmp);
}

/* Hoare partition around the middle element. Returns the split index. */
static int partition(Complaint **a, int lo, int hi, CmpFn cmp)
{
    Complaint *pivot = a[lo + (hi - lo) / 2];
    int i = lo - 1;
    int j = hi + 1;
    for (;;) {
        do { i++; } while (cmp(a[i], pivot) < 0);
        do { j--; } while (cmp(a[j], pivot) > 0);
        if (i >= j)
            return j;
        swap(&a[i], &a[j]);
    }
}

/* Recurse into the smaller side, loop over the larger: stack depth stays O(log n). */
static void quick_range(Complaint **a, int lo, int hi, CmpFn cmp)
{
    while (lo < hi) {
        int p = partition(a, lo, hi, cmp);
        if (p - lo < hi - p) {
            quick_range(a, lo, p, cmp);
            lo = p + 1;
        } else {
            quick_range(a, p + 1, hi, cmp);
            hi = p;
        }
    }
}

void quick_sort(Complaint **items, int n, CmpFn cmp)
{
    quick_range(items, 0, n - 1, cmp);
}

const char *SORT_NAMES[SORT_ALGORITHM_COUNT] = {
    "Bubble Sort", "Selection Sort", "Insertion Sort", "Merge Sort", "Quick Sort"
};
const SortFn SORT_FUNCTIONS[SORT_ALGORITHM_COUNT] = {
    bubble_sort, selection_sort, insertion_sort, merge_sort, quick_sort
};

Complaint **copy_array(Complaint **items, int n)
{
    Complaint **copy = xmalloc((size_t)n * sizeof(Complaint *));
    if (n > 0)
        memcpy(copy, items, (size_t)n * sizeof(Complaint *));
    return copy;
}

/* ------------------------------------------------------------- benchmarks */
static double elapsed_ms(clock_t start)
{
    return (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
}

void benchmark_sorting(const int *sizes, int count, SortBenchRow *rows)
{
    int s, a;
    for (s = 0; s < count; s++) {
        int n = sizes[s];
        Complaint **data = generate_random_complaints(n, 42);
        rows[s].n = n;
        for (a = 0; a < SORT_ALGORITHM_COUNT; a++) {
            Complaint **work = copy_array(data, n);       /* every algorithm gets the same input */
            clock_t start = clock();
            SORT_FUNCTIONS[a](work, n, cmp_pending_asc);
            rows[s].ms[a] = elapsed_ms(start);
            free(work);
        }
        free_complaints(data, n);
    }
}

SearchBenchResult benchmark_searching(int n, int lookups)
{
    SearchBenchResult r;
    Complaint **data = generate_random_complaints(n, 7);
    Complaint **by_id = copy_array(data, n);
    Complaint **out = xmalloc((size_t)n * sizeof(Complaint *));
    int *targets = xmalloc((size_t)lookups * sizeof(int));
    ComplaintBST tree;
    clock_t start;
    int i;

    merge_sort(by_id, n, cmp_id);                         /* input for binary search */
    bst_init(&tree);
    for (i = 0; i < n; i++)                               /* random order -> reasonably balanced */
        bst_insert(&tree, data[i]);
    for (i = 0; i < lookups; i++)
        targets[i] = data[(i * 37) % n]->cid;             /* existing IDs spread over the data */

    start = clock();
    for (i = 0; i < lookups; i++)
        linear_search(data, n, match_id, &targets[i], out);
    r.linear_ms = elapsed_ms(start);

    start = clock();
    for (i = 0; i < lookups; i++)
        binary_search(by_id, n, targets[i]);
    r.binary_ms = elapsed_ms(start);

    start = clock();
    for (i = 0; i < lookups; i++)
        bst_search(&tree, targets[i]);
    r.bst_ms = elapsed_ms(start);

    r.n = n;
    r.lookups = lookups;
    r.bst_height = bst_height(&tree);

    bst_free(&tree);
    free(targets);
    free(out);
    free(by_id);
    free_complaints(data, n);
    return r;
}

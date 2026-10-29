/*
 * test_civicdesk.c
 * Automated checks for the data structures, algorithms and system rules.
 * Build and run:  make test     (or see the README for the gcc command)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grievance_system.h"
#include "sample_data.h"
#include "search_sort.h"
#include "util.h"

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            printf("    FAILED: %s (line %d)\n", #cond, __LINE__);               \
            failed_here = 1;                                                     \
        }                                                                        \
    } while (0)

#define RUN(test)                                                                \
    do {                                                                         \
        int failed_here = 0;                                                     \
        int *flag = &failed_here;                                                \
        (void)flag;                                                              \
        printf("  %-58s", #test);                                                \
        failed_here = test();                                                    \
        tests_run++;                                                             \
        if (failed_here) { tests_failed++; printf("FAIL\n"); }                   \
        else printf("ok\n");                                                     \
    } while (0)

static void load_into(GrievanceSystem *sys)
{
    int n, i;
    Complaint **samples = load_sample_complaints(&n);
    system_init(sys);
    for (i = 0; i < n; i++)
        system_add_complaint(sys, samples[i]);
    free(samples);
}

/* True if both arrays hold equivalent complaints under cmp (same order of keys). */
static int same_order(Complaint **a, Complaint **b, int n, CmpFn cmp)
{
    int i;
    for (i = 0; i < n; i++)
        if (cmp(a[i], b[i]) != 0)
            return 0;
    return 1;
}

/* ------------------------------------------------------------- sorting */
static int test_all_sorts_match_reference(void)
{
    static const int sizes[] = {0, 1, 2, 50, 400};
    int failed_here = 0, s, k, a;
    for (s = 0; s < 5; s++) {
        int n = sizes[s];
        Complaint **data = generate_random_complaints(n, 1);
        for (k = 0; k < SORT_KEY_COUNT; k++) {
            Complaint **reference = copy_array(data, n);
            insertion_sort(reference, n, SORT_KEY_COMPARATORS[k]);   /* simple reference sort */
            for (a = 0; a < SORT_ALGORITHM_COUNT; a++) {
                Complaint **work = copy_array(data, n);
                SORT_FUNCTIONS[a](work, n, SORT_KEY_COMPARATORS[k]);
                CHECK(same_order(work, reference, n, SORT_KEY_COMPARATORS[k]));
                free(work);
            }
            free(reference);
        }
        free_complaints(data, n);
    }
    return failed_here;
}

static int test_quick_sort_on_sorted_and_reversed_input(void)
{
    int failed_here = 0, i, n = 2000;
    Complaint **data = generate_random_complaints(n, 3);
    Complaint **work = copy_array(data, n);
    quick_sort(work, n, cmp_id);
    for (i = 1; i < n; i++)
        CHECK(work[i - 1]->cid < work[i]->cid);
    quick_sort(work, n, cmp_id);                                  /* already sorted */
    for (i = 1; i < n; i++)
        CHECK(work[i - 1]->cid < work[i]->cid);
    for (i = 0; i < n / 2; i++) {                                 /* reverse it */
        Complaint *t = work[i];
        work[i] = work[n - 1 - i];
        work[n - 1 - i] = t;
    }
    quick_sort(work, n, cmp_id);
    for (i = 1; i < n; i++)
        CHECK(work[i - 1]->cid < work[i]->cid);
    free(work);
    free_complaints(data, n);
    return failed_here;
}

static int test_merge_sort_is_stable(void)
{
    int failed_here = 0, i, n = 300;
    Complaint **data = generate_random_complaints(n, 9);
    Complaint **work = copy_array(data, n);
    merge_sort(work, n, cmp_id);                                  /* order by ID first */
    merge_sort(work, n, cmp_priority);                            /* then by priority  */
    for (i = 1; i < n; i++)                                       /* equal priority -> IDs still ascending */
        if (work[i - 1]->priority == work[i]->priority)
            CHECK(work[i - 1]->cid < work[i]->cid);
    free(work);
    free_complaints(data, n);
    return failed_here;
}

/* ----------------------------------------------------------- searching */
static int test_bst_and_binary_search_find_every_complaint(void)
{
    GrievanceSystem sys;
    int failed_here = 0, i, n;
    Complaint **sorted;

    load_into(&sys);
    sorted = bst_inorder(&sys.bst, &n);
    CHECK(n == sys.count);
    for (i = 1; i < n; i++)
        CHECK(sorted[i - 1]->cid < sorted[i]->cid);
    for (i = 0; i < sys.count; i++) {
        CHECK(bst_search(&sys.bst, sys.all[i]->cid) == sys.all[i]);
        CHECK(binary_search(sorted, n, sys.all[i]->cid) == sys.all[i]);
    }
    free(sorted);
    system_free(&sys);
    return failed_here;
}

static int test_missing_id_returns_null(void)
{
    GrievanceSystem sys;
    int failed_here = 0, n;
    Complaint **sorted;
    load_into(&sys);
    sorted = bst_inorder(&sys.bst, &n);
    CHECK(bst_search(&sys.bst, 1) == NULL);
    CHECK(binary_search(sorted, n, 1) == NULL);
    free(sorted);
    system_free(&sys);
    return failed_here;
}

static int test_linear_search_by_area(void)
{
    GrievanceSystem sys;
    int failed_here = 0, found;
    Complaint **out;
    load_into(&sys);
    out = xmalloc((size_t)sys.count * sizeof(Complaint *));
    found = linear_search(sys.all, sys.count, match_area, "uppal", out);   /* case-insensitive */
    CHECK(found == 2);
    CHECK(out[0]->cid == 9036 && out[1]->cid == 8203);
    found = linear_search(sys.all, sys.count, match_name, "ANI", out);
    CHECK(found == 1 && out[0]->cid == 1307);
    free(out);
    system_free(&sys);
    return failed_here;
}

/* ----------------------------------------------------------------- BST */
static int test_duplicate_id_rejected(void)
{
    ComplaintBST tree;
    Complaint *a = complaint_create(10, "A", "X", "Roads", PRIORITY_NORMAL, today_day());
    Complaint *b = complaint_create(10, "B", "Y", "Roads", PRIORITY_NORMAL, today_day());
    int failed_here = 0;
    bst_init(&tree);
    CHECK(bst_insert(&tree, a) == 1);
    CHECK(bst_insert(&tree, b) == 0);
    CHECK(tree.size == 1);
    bst_free(&tree);
    complaint_free(a);
    complaint_free(b);
    return failed_here;
}

static int test_sorted_insertion_makes_a_straight_line(void)
{
    ComplaintBST tree;
    Complaint *items[10];
    int failed_here = 0, i;
    bst_init(&tree);
    for (i = 0; i < 10; i++) {
        items[i] = complaint_create(i + 1, "A", "X", "Roads", PRIORITY_NORMAL, today_day());
        bst_insert(&tree, items[i]);
    }
    CHECK(bst_height(&tree) == 10);                               /* worst case: O(n) */
    bst_free(&tree);
    for (i = 0; i < 10; i++)
        complaint_free(items[i]);
    return failed_here;
}

/* -------------------------------------------------------------- queues */
static int test_linked_queue_is_fifo(void)
{
    LinkedQueue q;
    Complaint *c[3];
    int failed_here = 0, i;
    queue_init(&q);
    for (i = 0; i < 3; i++) {
        c[i] = complaint_create(i + 1, "A", "X", "Roads", PRIORITY_NORMAL, today_day());
        queue_enqueue(&q, c[i]);
    }
    CHECK(queue_peek(&q) == c[0]);
    CHECK(queue_dequeue(&q) == c[0]);
    CHECK(queue_dequeue(&q) == c[1]);
    CHECK(queue_dequeue(&q) == c[2]);
    CHECK(queue_dequeue(&q) == NULL);
    CHECK(queue_is_empty(&q));
    for (i = 0; i < 3; i++)
        complaint_free(c[i]);
    return failed_here;
}

static int test_priority_order_then_fifo_inside_a_priority(void)
{
    GrievanceSystem sys;
    int failed_here = 0, count = 0, last_rank = -1;
    int urgent_ids[4], urgent = 0;
    Complaint *c;

    load_into(&sys);
    CHECK(pqm_total(&sys.queues) == 9);
    while ((c = pqm_dequeue_next(&sys.queues)) != NULL) {
        CHECK((int)c->priority >= last_rank);                    /* Urgent, High, Normal */
        last_rank = (int)c->priority;
        if (c->priority == PRIORITY_URGENT)
            urgent_ids[urgent++] = c->cid;
        count++;
    }
    CHECK(count == 9);
    CHECK(urgent == 2 && urgent_ids[0] == 4821 && urgent_ids[1] == 9036);   /* arrival order */
    CHECK(pqm_total(&sys.queues) == 0);
    system_free(&sys);
    return failed_here;
}

/* ------------------------------------------------ status list and undo */
static int test_status_history_append_and_remove_last(void)
{
    StatusHistory h;
    int failed_here = 0;
    history_init(&h);
    history_append(&h, STATUS_FILED, "", 100);
    history_append(&h, STATUS_ASSIGNED, "", 101);
    history_append(&h, STATUS_IN_PROGRESS, "", 102);
    CHECK(history_last_status(&h) == STATUS_IN_PROGRESS);
    CHECK(history_remove_last(&h) == 1);
    CHECK(history_last_status(&h) == STATUS_ASSIGNED);
    CHECK(h.size == 2);
    history_free(&h);
    return failed_here;
}

static int test_filed_status_can_never_be_removed(void)
{
    StatusHistory h;
    int failed_here = 0;
    history_init(&h);
    history_append(&h, STATUS_FILED, "", 100);
    CHECK(history_remove_last(&h) == 0);
    CHECK(h.size == 1);
    history_free(&h);
    return failed_here;
}

static int test_stack_is_lifo(void)
{
    UndoStack s;
    UndoRecord a = {1, STATUS_FILED, STATUS_ASSIGNED};
    UndoRecord b = {2, STATUS_ASSIGNED, STATUS_RESOLVED};
    UndoRecord out;
    int failed_here = 0;
    undo_init(&s);
    CHECK(undo_is_empty(&s));
    undo_push(&s, a);
    undo_push(&s, b);
    CHECK(undo_peek(&s, &out) && out.cid == 2);
    CHECK(undo_pop(&s, &out) && out.cid == 2);
    CHECK(undo_pop(&s, &out) && out.cid == 1);
    CHECK(undo_pop(&s, &out) == 0);
    undo_free(&s);
    return failed_here;
}

static int test_undo_two_updates_in_reverse_order(void)
{
    GrievanceSystem sys;
    int failed_here = 0;
    Complaint *c;
    UndoRecord r, r1 = {4821, STATUS_FILED, STATUS_ASSIGNED},
                  r2 = {4821, STATUS_ASSIGNED, STATUS_IN_PROGRESS};

    load_into(&sys);
    c = bst_search(&sys.bst, 4821);
    history_append(&c->history, STATUS_ASSIGNED, "", today_day());
    undo_push(&sys.undo, r1);
    history_append(&c->history, STATUS_IN_PROGRESS, "", today_day());
    undo_push(&sys.undo, r2);

    undo_pop(&sys.undo, &r);
    history_remove_last(&bst_search(&sys.bst, r.cid)->history);
    CHECK(complaint_status(c) == STATUS_ASSIGNED);
    undo_pop(&sys.undo, &r);
    history_remove_last(&bst_search(&sys.bst, r.cid)->history);
    CHECK(complaint_status(c) == STATUS_FILED);
    system_free(&sys);
    return failed_here;
}

/* -------------------------------------------------------------- system */
static int test_resolved_complaint_stops_counting_pending_days(void)
{
    GrievanceSystem sys;
    int failed_here = 0;
    Complaint *c;
    load_into(&sys);
    c = bst_search(&sys.bst, 1889);          /* filed 20 days ago, resolved on day 3 */
    CHECK(complaint_status(c) == STATUS_RESOLVED);
    CHECK(complaint_pending_days(c) == 3);
    system_free(&sys);
    return failed_here;
}

static int test_only_unprocessed_complaints_enter_the_queues(void)
{
    GrievanceSystem sys;
    int failed_here = 0;
    load_into(&sys);
    CHECK(sys.count == 12);
    CHECK(pqm_total(&sys.queues) == 9);      /* 12 samples - 3 already processed */
    system_free(&sys);
    return failed_here;
}

static int test_duplicate_complaint_is_rejected_by_the_system(void)
{
    GrievanceSystem sys;
    int failed_here = 0;
    Complaint *dup;
    load_into(&sys);
    dup = complaint_create(4821, "X", "Y", "Roads", PRIORITY_NORMAL, today_day());
    CHECK(system_add_complaint(&sys, dup) == 0);
    CHECK(sys.count == 12);
    complaint_free(dup);
    system_free(&sys);
    return failed_here;
}

static int test_generated_ids_are_unused(void)
{
    GrievanceSystem sys;
    int failed_here = 0, i;
    load_into(&sys);
    for (i = 0; i < 50; i++)
        CHECK(bst_search(&sys.bst, system_new_id(&sys)) == NULL);
    system_free(&sys);
    return failed_here;
}

static int test_system_grows_beyond_initial_capacity(void)
{
    GrievanceSystem sys;
    int failed_here = 0, i, n = 100;
    Complaint **extra = generate_random_complaints(n, 11);
    system_init(&sys);
    for (i = 0; i < n; i++)
        CHECK(system_add_complaint(&sys, extra[i]) == 1);
    CHECK(sys.count == n);
    CHECK(sys.bst.size == n);
    CHECK(pqm_total(&sys.queues) == n);
    free(extra);                              /* the system owns the complaints now */
    system_free(&sys);
    return failed_here;
}

/* ------------------------------------------------ extra tests: empty cases and consistency */

static int test_empty_system_starts_clean(void)
{
    GrievanceSystem sys;
    int failed_here = 0;

    system_init(&sys);

    CHECK(sys.count == 0);
    CHECK(sys.bst.size == 0);
    CHECK(pqm_total(&sys.queues) == 0);
    CHECK(undo_is_empty(&sys.undo));

    system_free(&sys);
    return failed_here;
}

static int test_empty_bst_search_and_traversal(void)
{
    ComplaintBST tree;
    Complaint **items;
    int failed_here = 0, n = -1;

    bst_init(&tree);

    CHECK(bst_search(&tree, 1234) == NULL);
    CHECK(bst_height(&tree) == 0);

    items = bst_inorder(&tree, &n);
    CHECK(n == 0);

    free(items);
    bst_free(&tree);
    return failed_here;
}

static int test_priority_queue_counts_are_consistent(void)
{
    GrievanceSystem sys;
    int failed_here = 0, p, total = 0;

    load_into(&sys);

    for (p = 0; p < PRIORITY_COUNT; p++)
        total += pqm_count(&sys.queues, (Priority)p);

    CHECK(total == pqm_total(&sys.queues));
    CHECK(total == 9);

    system_free(&sys);
    return failed_here;
}

static int test_history_last_day_after_removal(void)
{
    StatusHistory h;
    int failed_here = 0;

    history_init(&h);
    history_append(&h, STATUS_FILED, "", 100);
    history_append(&h, STATUS_ASSIGNED, "", 101);
    history_append(&h, STATUS_IN_PROGRESS, "", 102);

    CHECK(history_last_day(&h) == 102);
    CHECK(history_remove_last(&h) == 1);
    CHECK(history_last_day(&h) == 101);
    CHECK(history_last_status(&h) == STATUS_ASSIGNED);

    history_free(&h);
    return failed_here;
}

/* ------------------------------------------------ saving and loading */

#define TEST_FILE "civicdesk_test_data.tmp"

static void write_text_file(const char *text)
{
    FILE *f = fopen(TEST_FILE, "w");
    if (f != NULL) {
        fputs(text, f);
        fclose(f);
    }
}

static int test_save_and_load_round_trip(void)
{
    GrievanceSystem a, b;
    int failed_here = 0, i;
    Complaint *c;
    char error[100];

    load_into(&a);
    c = pqm_dequeue_next(&a.queues);                                /* process one complaint */
    history_append(&c->history, STATUS_ASSIGNED, "Test remark", today_day());

    CHECK(system_save(&a, TEST_FILE) == 0);
    system_init(&b);
    CHECK(system_load(&b, TEST_FILE, error, sizeof error) == 12);
    CHECK(b.count == a.count);
    CHECK(pqm_total(&b.queues) == pqm_total(&a.queues));

    for (i = 0; i < a.count; i++) {
        const Complaint *x = a.all[i];
        const Complaint *y = bst_search(&b.bst, x->cid);
        CHECK(y != NULL);
        if (y == NULL)
            continue;
        CHECK(strcmp(x->name, y->name) == 0);
        CHECK(strcmp(x->area, y->area) == 0);
        CHECK(strcmp(x->department, y->department) == 0);
        CHECK(x->priority == y->priority);
        CHECK(x->filed_on == y->filed_on);
        CHECK(complaint_status(x) == complaint_status(y));
        CHECK(x->history.size == y->history.size);
    }

    while ((c = pqm_dequeue_next(&a.queues)) != NULL) {             /* same processing order */
        Complaint *d = pqm_dequeue_next(&b.queues);
        CHECK(d != NULL && d->cid == c->cid);
    }
    CHECK(pqm_total(&b.queues) == 0);

    system_free(&a);
    system_free(&b);
    remove(TEST_FILE);
    return failed_here;
}

static int test_load_rejects_invalid_files_and_keeps_data(void)
{
    static const char *bad_files[] = {
        "HELLO\n",
        "CIVICDESK 1\nC|5|A|X|Roads|2|100\nC|5|B|Y|Roads|1|100\n",
        "CIVICDESK 1\nC|5|A|X|Roads|9|100\n",
        "CIVICDESK 1\nS|1|100|Orphan status\n",
        "CIVICDESK 1\nC|5|A|X|Roads|2|100\nS|3|101|\nS|2|102|\n",
        "CIVICDESK 1\nsomething else\n"
    };
    GrievanceSystem sys;
    int failed_here = 0, i;
    char error[100];

    load_into(&sys);
    for (i = 0; i < 6; i++) {
        write_text_file(bad_files[i]);
        CHECK(system_load(&sys, TEST_FILE, error, sizeof error) == -1);
        CHECK(error[0] != '\0');
        CHECK(sys.count == 12);                                    /* current data untouched */
        CHECK(bst_search(&sys.bst, 4821) != NULL);
    }
    write_text_file("CIVICDESK 1\n");                              /* a valid but empty file */
    CHECK(system_load(&sys, TEST_FILE, error, sizeof error) == 0);
    CHECK(sys.count == 0);

    system_free(&sys);
    remove(TEST_FILE);
    return failed_here;
}

static int test_load_missing_file_fails(void)
{
    GrievanceSystem sys;
    int failed_here = 0;
    char error[100];

    load_into(&sys);
    CHECK(system_load(&sys, "this_file_does_not_exist.tmp", error, sizeof error) == -1);
    CHECK(sys.count == 12);
    system_free(&sys);
    return failed_here;
}

static int test_save_replaces_separator_characters(void)
{
    GrievanceSystem a, b;
    int failed_here = 0;
    Complaint *c = complaint_create(77, "A|B", "Area|1", "Roads", PRIORITY_HIGH, today_day());
    const Complaint *y;
    char error[100];

    system_init(&a);
    history_append(&c->history, STATUS_ASSIGNED, "x|y\nz", today_day());
    system_add_complaint(&a, c);

    CHECK(system_save(&a, TEST_FILE) == 0);
    system_init(&b);
    CHECK(system_load(&b, TEST_FILE, error, sizeof error) == 1);
    y = bst_search(&b.bst, 77);
    CHECK(y != NULL);
    if (y != NULL) {
        CHECK(strcmp(y->name, "A B") == 0);
        CHECK(strcmp(y->history.tail->remarks, "x y z") == 0);
    }

    system_free(&a);
    system_free(&b);
    remove(TEST_FILE);
    return failed_here;
}

int main(void)
{
    printf("CivicDesk tests\n");
    RUN(test_all_sorts_match_reference);
    RUN(test_quick_sort_on_sorted_and_reversed_input);
    RUN(test_merge_sort_is_stable);
    RUN(test_bst_and_binary_search_find_every_complaint);
    RUN(test_missing_id_returns_null);
    RUN(test_linear_search_by_area);
    RUN(test_duplicate_id_rejected);
    RUN(test_sorted_insertion_makes_a_straight_line);
    RUN(test_linked_queue_is_fifo);
    RUN(test_priority_order_then_fifo_inside_a_priority);
    RUN(test_status_history_append_and_remove_last);
    RUN(test_filed_status_can_never_be_removed);
    RUN(test_stack_is_lifo);
    RUN(test_undo_two_updates_in_reverse_order);
    RUN(test_resolved_complaint_stops_counting_pending_days);
    RUN(test_only_unprocessed_complaints_enter_the_queues);
    RUN(test_duplicate_complaint_is_rejected_by_the_system);
    RUN(test_generated_ids_are_unused);
    RUN(test_system_grows_beyond_initial_capacity);

    RUN(test_empty_system_starts_clean);
    RUN(test_empty_bst_search_and_traversal);
    RUN(test_priority_queue_counts_are_consistent);
    RUN(test_history_last_day_after_removal);

    RUN(test_save_and_load_round_trip);
    RUN(test_load_rejects_invalid_files_and_keeps_data);
    RUN(test_load_missing_file_fails);
    RUN(test_save_replaces_separator_characters);

    printf("\n%d tests run, %d failed.\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}

/*
 * CivicDesk: Public Grievance Redressal System
 * Menu-driven console program built on: queue, linked list, stack, BST, searching, sorting.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "grievance_system.h"
#include "sample_data.h"
#include "search_sort.h"
#include "util.h"

static GrievanceSystem g_system;

static void cleanup(void)
{
    system_free(&g_system);
}

/* ------------------------------------------------------------ input helpers */
/* Reads one line. If the input stream ends, the program exits (cleanup runs via atexit). */
static void read_line(const char *prompt, char *buf, size_t size)
{
    size_t len, start = 0;

    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, (int)size, stdin) == NULL) {
        printf("\nExiting CivicDesk.\n");
        exit(0);
    }
    len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r' || buf[len - 1] == ' '))
        buf[--len] = '\0';
    while (buf[start] == ' ')
        start++;
    if (start > 0)
        memmove(buf, buf + start, strlen(buf + start) + 1);
}

static int ask_int(const char *prompt)
{
    char buf[32];
    for (;;) {
        size_t i, len;
        int valid;
        read_line(prompt, buf, sizeof buf);
        len = strlen(buf);
        valid = (len > 0 && len <= 9);
        for (i = 0; i < len && valid; i++)
            if (!isdigit((unsigned char)buf[i]))
                valid = 0;
        if (valid)
            return atoi(buf);
        printf("  Please enter a number.\n");
    }
}

static void ask_text(const char *prompt, char *buf, size_t size)
{
    for (;;) {
        read_line(prompt, buf, size);
        if (buf[0] != '\0')
            return;
        printf("  This field cannot be empty.\n");
    }
}

/* Shows a numbered list and returns the chosen index (0-based). */
static int ask_choice(const char *title, const char *const *options, int count)
{
    int i;
    printf("%s\n", title);
    for (i = 0; i < count; i++)
        printf("  %d. %s\n", i + 1, options[i]);
    for (;;) {
        char buf[32];
        int value = 0;
        read_line("  Choose: ", buf, sizeof buf);
        if (buf[0] != '\0' && strlen(buf) <= 3 && isdigit((unsigned char)buf[0]))
            value = atoi(buf);
        if (value >= 1 && value <= count)
            return value - 1;
        printf("  Invalid choice, try again.\n");
    }
}

static void print_table(Complaint **list, int n)
{
    int i;
    if (n == 0) {
        printf("  No complaints found.\n");
        return;
    }
    printf("  %-6s %-12s %-14s %-14s %-8s %-12s %s\n",
           "ID", "Name", "Area", "Department", "Priority", "Status", "Pending");
    printf("  ----------------------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        const Complaint *c = list[i];
        printf("  %-6d %-12s %-14s %-14s %-8s %-12s %3dd\n",
               c->cid, c->name, c->area, c->department,
               PRIORITY_NAMES[c->priority], STATUS_NAMES[complaint_status(c)],
               complaint_pending_days(c));
    }
    printf("  (%d complaint(s))\n", n);
}

static void print_one(Complaint *c)
{
    Complaint *list[1];
    list[0] = c;
    print_table(list, c ? 1 : 0);
}

/* ------------------------------------------------------------ menu actions */
static void file_complaint(void)
{
    char name[NAME_LEN], area[AREA_LEN];
    int department, priority;
    Complaint *c;

    printf("\n--- File a new complaint ---\n");
    ask_text("Citizen name: ", name, sizeof name);
    ask_text("Area: ", area, sizeof area);
    department = ask_choice("Department:", DEPARTMENTS, DEPARTMENT_COUNT);
    priority = ask_choice("Priority:", PRIORITY_NAMES, PRIORITY_COUNT);

    c = complaint_create(system_new_id(&g_system), name, area, DEPARTMENTS[department],
                         (Priority)priority, today_day());
    system_add_complaint(&g_system, c);
    printf("\n  Complaint filed. Your complaint ID is %d (queued as %s).\n",
           c->cid, PRIORITY_NAMES[priority]);
}

static void process_next(void)
{
    Complaint *c;
    char remark[REMARK_LEN];

    printf("\n--- Process next complaint ---\n");
    c = pqm_dequeue_next(&g_system.queues);
    if (c == NULL) {
        printf("  No complaints are waiting in the queues.\n");
        return;
    }
    snprintf(remark, sizeof remark, "Assigned to %s department", c->department);
    history_append(&c->history, STATUS_ASSIGNED, remark, today_day());
    printf("  Now processing:\n");
    print_one(c);
}

static void update_status(void)
{
    Complaint *c;
    Status current, chosen;
    const char *options[STATUS_COUNT];
    char title[64], remarks[REMARK_LEN];
    UndoRecord record;
    int count = 0, s, pick;

    printf("\n--- Update complaint status ---\n");
    c = bst_search(&g_system.bst, ask_int("Complaint ID: "));
    if (c == NULL) {
        printf("  Complaint not found.\n");
        return;
    }
    current = complaint_status(c);
    if (current == STATUS_FILED) {
        printf("  This complaint is still waiting in the queue. Use 'Process next complaint' first.\n");
        return;
    }
    if (current == STATUS_RESOLVED) {
        printf("  This complaint is already resolved.\n");
        return;
    }
    for (s = (int)current + 1; s < STATUS_COUNT; s++)       /* only forward steps are allowed */
        options[count++] = STATUS_NAMES[s];

    snprintf(title, sizeof title, "Current status: %s. Move to:", STATUS_NAMES[current]);
    pick = ask_choice(title, options, count);
    chosen = (Status)((int)current + 1 + pick);
    read_line("Remarks (optional): ", remarks, sizeof remarks);

    history_append(&c->history, chosen, remarks, today_day());
    record.cid = c->cid;
    record.previous = current;
    record.current = chosen;
    undo_push(&g_system.undo, record);                      /* remember for undo */
    printf("  Status updated: %s -> %s\n", STATUS_NAMES[current], STATUS_NAMES[chosen]);
}

static void undo_last(void)
{
    UndoRecord record;
    Complaint *c;

    printf("\n--- Undo last status update ---\n");
    if (!undo_pop(&g_system.undo, &record)) {
        printf("  Nothing to undo.\n");
        return;
    }
    c = bst_search(&g_system.bst, record.cid);
    history_remove_last(&c->history);
    printf("  Undone for complaint %d: '%s' removed, status is back to '%s'.\n",
           record.cid, STATUS_NAMES[record.current], STATUS_NAMES[record.previous]);
}

static void search_menu(void)
{
    static const char *const methods[] = {
        "Complaint ID (BST search)",
        "Complaint ID (binary search on sorted list)",
        "Citizen name (linear search)",
        "Area (linear search)"
    };
    int method;

    printf("\n--- Search complaints ---\n");
    method = ask_choice("Search by:", methods, 4);

    if (method == 0) {
        print_one(bst_search(&g_system.bst, ask_int("Complaint ID: ")));
    } else if (method == 1) {
        int n, id = ask_int("Complaint ID: ");
        Complaint **sorted = bst_inorder(&g_system.bst, &n);   /* in-order list is sorted by ID */
        print_one(binary_search(sorted, n, id));
        free(sorted);
    } else {
        char text[AREA_LEN];
        Complaint **found = xmalloc((size_t)g_system.count * sizeof(Complaint *));
        int n;
        if (method == 2) {
            ask_text("Name contains: ", text, sizeof text);
            n = linear_search(g_system.all, g_system.count, match_name, text, found);
        } else {
            ask_text("Area contains: ", text, sizeof text);
            n = linear_search(g_system.all, g_system.count, match_area, text, found);
        }
        print_table(found, n);
        free(found);
    }
}

static void view_history(void)
{
    Complaint *c;
    printf("\n--- Status history ---\n");
    c = bst_search(&g_system.bst, ask_int("Complaint ID: "));
    if (c == NULL) {
        printf("  Complaint not found.\n");
        return;
    }
    printf("  Complaint %d | %s | %s | %s\n", c->cid, c->name, c->department, c->area);
    history_display(&c->history);
}

static void reports_menu(void)
{
    int key, algorithm;
    Complaint **work;
    clock_t start;
    double ms;

    printf("\n--- Sort and view reports ---\n");
    key = ask_choice("Sort by:", SORT_KEY_NAMES, SORT_KEY_COUNT);
    algorithm = ask_choice("Algorithm:", SORT_NAMES, SORT_ALGORITHM_COUNT);

    work = copy_array(g_system.all, g_system.count);        /* the original order stays untouched */
    start = clock();
    SORT_FUNCTIONS[algorithm](work, g_system.count, SORT_KEY_COMPARATORS[key]);
    ms = (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    printf("\n");
    print_table(work, g_system.count);
    if (ms <= 0.0)
        printf("  Sorted by %s using %s in under 1 ms (too fast to measure)\n",
               SORT_KEY_NAMES[key], SORT_NAMES[algorithm]);
    else
        printf("  Sorted by %s using %s in %.3f ms\n",
               SORT_KEY_NAMES[key], SORT_NAMES[algorithm], ms);
    free(work);
}

static void benchmark_menu(void)
{
    static const int sizes[] = {1000, 5000, 10000};
    SortBenchRow rows[3];
    SearchBenchResult r;
    int i, a;

    printf("\n--- Benchmark (random data, times in milliseconds) ---\n");
    printf("Running sorting benchmark, please wait...\n");
    fflush(stdout);
    benchmark_sorting(sizes, 3, rows);

    printf("\n  %-8s", "n");
    for (a = 0; a < SORT_ALGORITHM_COUNT; a++)
        printf("%16s", SORT_NAMES[a]);
    printf("\n");
    for (i = 0; i < 3; i++) {
        printf("  %-8d", rows[i].n);
        for (a = 0; a < SORT_ALGORITHM_COUNT; a++)
            printf("%16.2f", rows[i].ms[a]);
        printf("\n");
    }

    r = benchmark_searching(20000, 500);
    printf("\n  Searching: %d lookups in %d complaints (BST height = %d)\n",
           r.lookups, r.n, r.bst_height);
    printf("  %-16s%10.2f ms\n", "Linear Search", r.linear_ms);
    printf("  %-16s%10.2f ms\n", "Binary Search", r.binary_ms);
    printf("  %-16s%10.2f ms\n", "BST Search", r.bst_ms);
}

static void ask_file_name(char *path, size_t size)
{
    read_line("File name (press Enter for civicdesk_data.txt): ", path, size);
    if (path[0] == '\0')
        copy_str(path, size, "civicdesk_data.txt");
}

static void save_data(void)
{
    char path[200];
    printf("\n--- Save complaints to a file ---\n");
    ask_file_name(path, sizeof path);
    if (system_save(&g_system, path) == 0)
        printf("  Saved %d complaints to %s\n", g_system.count, path);
    else
        printf("  Could not write to %s\n", path);
}

static void load_data(void)
{
    char path[200], answer[16], error[100];
    int n;

    printf("\n--- Load complaints from a file ---\n");
    ask_file_name(path, sizeof path);
    read_line("This replaces all current complaints and clears undo. Continue? (y/n): ",
              answer, sizeof answer);
    if (answer[0] != 'y' && answer[0] != 'Y') {
        printf("  Cancelled. Nothing was changed.\n");
        return;
    }
    n = system_load(&g_system, path, error, sizeof error);
    if (n < 0)
        printf("  Could not load %s (%s). Nothing was changed.\n", path, error);
    else
        printf("  Loaded %d complaints from %s\n", n, path);
}

/* ------------------------------------------------------------ main loop */
static void print_menu(void)
{
    printf("\n==================== CivicDesk ====================\n");
    printf(" 1. File a new complaint\n");
    printf(" 2. Process next complaint\n");
    printf(" 3. Update complaint status\n");
    printf(" 4. Undo last status update\n");
    printf(" 5. Search complaints (ID / name / area)\n");
    printf(" 6. View status history of a complaint\n");
    printf(" 7. Sort and view reports\n");
    printf(" 8. Benchmark sorting and searching\n");
    printf(" 9. Save complaints to a file\n");
    printf("10. Load complaints from a file\n");
    printf(" 0. Exit\n");
    printf("===================================================\n");
    printf(" Waiting in queues -> Urgent: %d | High: %d | Normal: %d\n",
           pqm_count(&g_system.queues, PRIORITY_URGENT),
           pqm_count(&g_system.queues, PRIORITY_HIGH),
           pqm_count(&g_system.queues, PRIORITY_NORMAL));
}

int main(void)
{
    Complaint **samples;
    int i, n;
    char choice[16];

    srand((unsigned)time(NULL));
    system_init(&g_system);
    atexit(cleanup);

    samples = load_sample_complaints(&n);
    for (i = 0; i < n; i++)
        system_add_complaint(&g_system, samples[i]);
    free(samples);                                  /* the system now owns the complaints */

    printf("CivicDesk: Public Grievance Redressal System\n");
    printf("Loaded %d sample complaints.\n", g_system.count);

    for (;;) {
        print_menu();
        read_line(" Enter your choice (0-10): ", choice, sizeof choice);
        if (strcmp(choice, "1") == 0)      file_complaint();
        else if (strcmp(choice, "2") == 0) process_next();
        else if (strcmp(choice, "3") == 0) update_status();
        else if (strcmp(choice, "4") == 0) undo_last();
        else if (strcmp(choice, "5") == 0) search_menu();
        else if (strcmp(choice, "6") == 0) view_history();
        else if (strcmp(choice, "7") == 0) reports_menu();
        else if (strcmp(choice, "8") == 0) benchmark_menu();
        else if (strcmp(choice, "9") == 0) save_data();
        else if (strcmp(choice, "10") == 0) load_data();
        else if (strcmp(choice, "0") == 0) {
            printf("Thank you for using CivicDesk.\n");
            break;
        } else
            printf("  Invalid choice. Enter a number from 0 to 10.\n");
    }
    return 0;
}

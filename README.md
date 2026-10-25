# CivicDesk — Public Grievance Redressal System (C)

CivicDesk is a menu-driven complaint-management application and data-structures project written in **C**. Citizens' complaints are registered, prioritised through three linked-list queues, tracked through a per-complaint status history, indexed in a binary search tree, and reported with classical searching and sorting algorithms. Every status change can be reversed with an undo stack. All names, areas and complaints in the sample data are fictional, and nothing is sent to any real government system.

Live demo: https://pulegarisashikirran.github.io/civicdesk-grievance-system/ 
Repository: https://github.com/pulegarisashikirran/civicdesk-grievance-system

## What is implemented

* Console application in standard **C99** with no external libraries; every data structure is built by hand with `malloc` and pointers (no STL, no ready-made containers).
* Complaint registration with a generated unique ID, citizen name, area, department and priority (Urgent / High / Normal).
* Three FIFO queues (one per priority) with a "process next complaint" action that always serves the highest non-empty priority.
* A singly linked list per complaint that records the full status trail: Filed → Assigned → In Progress → Resolved, with dates and remarks.
* Undo of the last status update using a linked-list stack.
* A binary search tree keyed by complaint ID for lookup and sorted listing (in-order traversal).
* Search by complaint ID (BST search and binary search), and by citizen name or area (linear search, case-insensitive).
* Reports sorted by pending days, area, complaint ID or priority using five implemented algorithms: bubble, selection, insertion, merge and quick sort, all working on the same comparator-based interface.
* A benchmark that times all five sorts and all three search methods on the same random data.
* Pending-day calculation that stops counting once a complaint is resolved.
* 12 pre-loaded sample complaints (some already in progress or resolved) so every feature can be demonstrated immediately.
* 19 automated unit tests covering the data structures, algorithms and system rules.
* Complete memory clean-up on exit: every node and complaint is freed (checked with AddressSanitizer).

## Data Structures & Algorithms — Topics Used, Locations & Logic

CivicDesk is architected around six foundational data structures and classical algorithms. Each one is used for the job it is best suited to.

### Quick Reference Matrix

| Required Topic | File Location | Data Structure / Function | Role in CivicDesk | Time / Space Complexity |
|---|---|---|---|---|
| Linear Search | `search_sort.c` | `linear_search()` | Finds complaints by citizen name or area in the full complaint array | O(n) Time / O(1) Space |
| Binary Search | `search_sort.c` | `binary_search()` | Finds a complaint by ID in the array sorted by ID | O(log n) Time / O(1) Space |
| Singly Linked List (SLL) | `status_list.c` | `StatusNode`, `StatusHistory` | Chronological status trail of each complaint | O(1) Append / O(k) Remove last / O(k) Space (k ≤ 4) |
| Queue (Linked List) | `queues.c` | `QueueNode`, `LinkedQueue` | Strict first-in, first-out waiting line for complaints | O(1) Enqueue/Dequeue / O(n) Space |
| Priority handling (3 queues) | `queues.c` | `PriorityQueueManager` | Serves Urgent before High before Normal, FIFO inside each level | O(1) per operation (3 fixed queues) |
| Stack (Linked List) | `undo_stack.c` | `StackNode`, `UndoStack` | Remembers status updates so the latest one is undone first | O(1) Push/Pop/Peek / O(n) Space |
| Binary Search Tree (BST) | `bst.c` | `BSTNode`, `ComplaintBST` | Indexes every complaint by ID; in-order traversal gives sorted output | O(log n) Avg / O(n) Worst Search & Insert / O(n) Space |
| Bubble Sort | `search_sort.c` | `bubble_sort()` | Sorts reports using adjacent swaps with early termination | O(n²) Worst/Avg, O(n) Best / O(1) Space |
| Selection Sort | `search_sort.c` | `selection_sort()` | Sorts reports by selecting successive minimum values in place | O(n²) Comparisons, O(n) Swaps / O(1) Space |
| Insertion Sort | `search_sort.c` | `insertion_sort()` | Sorts reports by inserting each item into a sorted prefix | O(n²) Worst, O(n) Best / O(1) Space |
| Merge Sort | `search_sort.c` | `merge_sort()` | Stable divide-and-conquer sorting for large complaint lists | O(n log n) Time / O(n) Space |
| Quick Sort | `search_sort.c` | `quick_sort()` | In-place Hoare-partitioned sorting; smaller side recursed first | O(n log n) Avg, O(n²) Worst / O(log n) Space |

### Detailed Breakdown of Topics, Usage & Underlying Logic

#### 1. Linear Search

* **Where Used:**
  * `search_sort.c` (`linear_search`, with the match functions `match_name`, `match_area`, `match_id`)
  * Called from the Search menu in `main.c` to find complaints by citizen name or area.
  * Also used by the benchmark as the baseline for ID lookup.
* **Logic:**
  * Starts at index `0` and steps through every complaint up to `n − 1`.
  * Calls a match function through a function pointer (for example "does the name contain this text, ignoring case?"). Every match is copied into an output array.
  * Does not require the data to be ordered or indexed, which is why it is used for fields that are not the BST key.
  * Worst and average time complexity: O(n); space complexity: O(1).

#### 2. Binary Search

* **Where Used:**
  * `search_sort.c` (`binary_search`)
  * Called from the Search menu in `main.c` on the array returned by `bst_inorder()`, which is already sorted by complaint ID.
* **Logic:**
  * Operates only on an array sorted by complaint ID.
  * Keeps `low = 0` and `high = n − 1`. At each step it computes `mid = low + (high − low) / 2` (written this way to avoid integer overflow) and compares `sorted_items[mid]->cid` with the target.
  * If equal, the complaint is returned. If the middle ID is smaller, `low = mid + 1`; if larger, `high = mid − 1`. If the range becomes empty, `NULL` is returned.
  * Halves the candidate range in every iteration: O(log n) time. The iterative version needs only O(1) extra space.

#### 3. Singly Linked List (SLL)

* **Where Used:**
  * `status_list.c` (`StatusNode`, `StatusHistory`)
  * One `StatusHistory` is embedded inside every `Complaint` in `complaint.h`.
* **Logic:**
  * Each node holds `status`, `remarks`, `day` and a single `next` pointer.
  * `history_append()` links a new node after `tail` and moves `tail`, so adding a status takes O(1) time with no array resizing.
  * `history_display()` walks from `head` through `next` until `NULL`, printing the exact order in which things happened.
  * `history_remove_last()` (used by undo) walks to the node before `tail`, frees the tail and moves `tail` back. This is O(k) with k ≤ 4 statuses. The first node, "Filed", can never be removed.

#### 4. Queue Using Linked Lists (FIFO)

* **Where Used:**
  * `queues.c` (`QueueNode`, `LinkedQueue`)
* **Logic:**
  * Keeps `front` and `rear` pointers and a `size` counter.
  * `queue_enqueue()` links a node after `rear` in O(1). `queue_dequeue()` unlinks and frees the node at `front` in O(1), and resets `rear` to `NULL` when the queue becomes empty.
  * Guarantees fairness: among complaints of the same priority, the one filed earlier is always processed first.

#### 5. Priority Handling with Three Queues

* **Where Used:**
  * `queues.c` (`PriorityQueueManager`)
  * Called through `grievance_system.c` when a complaint is filed (enqueue) and from `main.c` when "Process next complaint" is chosen (`pqm_dequeue_next`).
* **Logic:**
  * Holds one `LinkedQueue` each for **Urgent**, **High** and **Normal** in an array indexed by the `Priority` enum.
  * `pqm_enqueue()` places the complaint in the queue matching its priority.
  * `pqm_dequeue_next()` checks Urgent, then High, then Normal, and returns the front of the first non-empty queue.
  * Result: urgent complaints are never stuck behind routine ones, and order inside each level stays strictly first-in, first-out. No heap is needed because the number of priority levels is fixed.

#### 6. Stack Using Linked Lists (LIFO)

* **Where Used:**
  * `undo_stack.c` (`StackNode`, `UndoStack`)
  * Integrated in `main.c`: `update_status` pushes, `undo_last` pops.
* **Logic:**
  * `top` points to the most recent node. `undo_push()` allocates a node whose `next` is the old top and then makes it the new top, in O(1).
  * Every status update pushes a record `(complaint ID, previous status, new status)`.
  * "Undo" pops the most recent record, finds the complaint through the BST, and calls `history_remove_last()` on its status list. Last in, first out means the latest change is always the first one reversed.
  * `undo_peek()` and `undo_is_empty()` are also O(1).

#### 7. Binary Search Tree (BST)

* **Where Used:**
  * `bst.c` (`BSTNode`, `ComplaintBST`)
  * One tree holds every complaint; `main.c` uses it for ID search, status updates, undo and history.
* **Logic:**
  * Each node points to one complaint. IDs smaller than the node go to the left subtree, larger IDs to the right.
  * `bst_insert()` walks down iteratively and rejects duplicate IDs. `bst_search()` follows a single root-to-node path.
  * `bst_inorder()` visits left, node, right, which always yields complaints sorted by ID with no separate sorting step. This array also feeds binary search.
  * New complaint IDs are generated randomly (not 1, 2, 3, …) so the tree stays reasonably balanced. Inserting IDs in sorted order would turn it into a straight line and degrade operations to O(n); this behaviour is covered by a unit test. Self-balancing trees (AVL / Red-Black) are listed under future improvements.
  * Average search and insert: O(log n); worst case O(n).

#### 8–12. Five Sorting Algorithms

Implemented in `search_sort.c` and interactively selectable in the **Sort and view reports** menu. Every sort has the same signature, `void sort(Complaint **items, int n, CmpFn cmp)`, sorts the array in place, and orders by a comparator function pointer. The menu always sorts a copy, so the original data is never modified.

1. **Bubble Sort (`bubble_sort`):**
   * Repeatedly compares adjacent elements and swaps them if they are out of order.
   * Includes an early-termination optimisation: if a full pass makes no swap, sorting stops (O(n) best case). Worst and average: O(n²).
2. **Selection Sort (`selection_sort`):**
   * Splits the array into a sorted prefix and an unsorted suffix.
   * Each pass finds the smallest element in the unsorted part and swaps it into position. Only O(n) swaps, but O(n²) comparisons.
3. **Insertion Sort (`insertion_sort`):**
   * Builds the sorted array from left to right, shifting larger elements right until the correct slot opens for the current element.
   * Adaptive: O(n) on nearly sorted data, O(n²) worst case.
4. **Merge Sort (`merge_sort`):**
   * Splits the range at `mid = lo + (hi − lo) / 2`, sorts each half recursively and merges the two sorted halves into a temporary buffer.
   * Takes the left element when keys are equal (`<= 0`), so equal keys keep their order (stable). Guaranteed O(n log n) on every input, with O(n) extra space for the buffer.
5. **Quick Sort (`quick_sort`):**
   * Divide-and-conquer with Hoare partitioning around the middle element, working in place.
   * Always recurses into the smaller partition first and loops over the larger one, which keeps the recursion depth at O(log n). Average time O(n log n); worst case O(n²). Hoare partitioning handles many equal keys (such as equal pending days) without degrading.

Reports can be sorted by **pending days** (longest first), **area**, **complaint ID** or **priority**. The chosen algorithm and its running time in milliseconds are printed under every report.

#### How the structures work together

The BST is the master index of all complaints. The three queues hold only the complaints still waiting to be processed. Each complaint carries its own status linked list, and the undo stack remembers recent status updates so they can be reversed. A plain array of all complaints supports linear search and the sorting reports. The binary search runs over the BST's in-order array. `grievance_system.c` owns all of these and frees them together on exit.

## How a Complaint Flows Through the System

1. A citizen's complaint is entered through **File a new complaint**. A random unused 4-digit ID is generated.
2. The complaint is inserted into the **BST** and placed in the **Urgent / High / Normal queue**. Its **status list** starts with "Filed".
3. **Process next complaint** takes the first complaint from the highest non-empty queue and appends "Assigned" to its status list.
4. **Update complaint status** moves a processed complaint to "In Progress" or "Resolved". The new status is appended to the linked list and a record is pushed on the **undo stack**. Complaints still waiting in a queue cannot be updated, so the queues always stay consistent.
5. If a wrong status was entered, **Undo** pops the stack and removes that status.
6. **Search** finds complaints by ID (BST or binary search) or by name or area (linear search), and **View status history** prints the full trail.
7. **Sort and view reports** ranks complaints by the chosen field with the chosen algorithm and shows how long it took.

## How Pending Days and Status Tracking Work

CivicDesk tracks how long every complaint has been open. Dates are stored as day numbers (days since 1 January 1970) and converted back to `YYYY-MM-DD` only for display.

1. **Open complaints:**
   * Pending days = today's day number − the day the complaint was filed.
   * The value is recalculated every time it is displayed or used as a sort key.
2. **Resolved complaints:**
   * Once the latest status is "Resolved", counting stops at the day of that status.
   * Pending days = resolution day − filing day, so resolved complaints no longer look overdue.
3. **Live visibility:**
   * The main menu always shows how many complaints are waiting in each queue.
   * Every table shows ID, name, area, department, priority, current status and pending days. **View status history** shows the dated trail, including remarks entered by the officer.

## Menu Options

| Option | Purpose |
|---|---|
| `1` File a new complaint | Register a complaint, generate its ID, add it to the BST and the right queue |
| `2` Process next complaint | Dequeue the highest-priority complaint and mark it "Assigned" |
| `3` Update complaint status | Move a processed complaint to In Progress or Resolved, with optional remarks |
| `4` Undo last status update | Pop the stack and remove the latest status |
| `5` Search complaints | Search by ID (BST), ID (binary search), citizen name or area (linear search) |
| `6` View status history | Print the full status trail of one complaint |
| `7` Sort and view reports | Sort by pending days, area, ID or priority with any of the five algorithms |
| `8` Benchmark | Time all sorts and searches on random data |
| `9` Exit | Quit the program |

## Complexity Summary

### Searching Algorithms

| Algorithm | Best Time | Average Time | Worst Time | Space |
|---|---|---|---|---|
| Linear Search | O(1) | O(n) | O(n) | O(1) |
| Binary Search (iterative) | O(1) | O(log n) | O(log n) | O(1) |
| BST Search | O(1) | O(log n) | O(n) | O(1) |

### Sorting Algorithms

| Algorithm | Best Time | Average Time | Worst Time | Auxiliary Space |
|---|---|---|---|---|
| Bubble Sort | O(n)\* | O(n²) | O(n²) | O(1) |
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1) |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n)\*\* |

\* Bubble sort reaches O(n) only with the early-exit check on already sorted data.
\*\* Recursion always continues into the smaller partition first, so the stack depth stays O(log n).

## Local development

Requires a C compiler that supports C99 (GCC, Clang, or MinGW on Windows). No libraries to install.

```bash
git clone https://github.com/pulegarisashikirran/civicdesk-grievance-system.git
cd civicdesk-grievance-system
```

**Build and run with `make`:**

```bash
make
./civicdesk
```

**Or compile directly with GCC (no `make` needed):**

```bash
gcc -std=c99 -Wall -Wextra -O2 -o civicdesk main.c util.c status_list.c complaint.c bst.c queues.c undo_stack.c search_sort.c sample_data.c grievance_system.c
./civicdesk
```

On Windows (MinGW), the program is created as `civicdesk.exe` and started with `civicdesk`. In Code::Blocks or Dev-C++, create a project and add every `.c` and `.h` file.

**Run the automated tests:**

```bash
make test
```

or

```bash
gcc -std=c99 -Wall -Wextra -O2 -o test_civicdesk test_civicdesk.c util.c status_list.c complaint.c bst.c queues.c undo_stack.c search_sort.c sample_data.c grievance_system.c
./test_civicdesk
```

**Optional memory check (Linux / macOS / MinGW with sanitizer support):**

```bash
gcc -std=c99 -g -fsanitize=address,undefined -o civicdesk_check main.c util.c status_list.c complaint.c bst.c queues.c undo_stack.c search_sort.c sample_data.c grievance_system.c
./civicdesk_check
```

The benchmark option (menu item 8) takes a couple of seconds because it deliberately runs the O(n²) sorts on up to 10,000 complaints.

### Benchmark results

_Run menu option 8 on your own machine and paste the table here. Times vary by computer and compiler settings._

| Complaints (n) | Bubble | Selection | Insertion | Merge | Quick |
|---|---|---|---|---|---|
| 1000 | | | | | |
| 5000 | | | | | |
| 10000 | | | | | |

## Sample output

_Add screenshots of the menu, processing a complaint, a status history, an undo, a search and a sorted report here._

## Main files

```
main.c                Menu-driven interface (input handling, printing, menu actions)
grievance_system.h/.c Connects BST + queues + undo stack + complaint array; owns and frees them
complaint.h/.c        Complaint record (owns a StatusHistory), priorities, departments
status_list.h/.c      Singly linked list for each complaint's status history
queues.h/.c           LinkedQueue (FIFO) and PriorityQueueManager (Urgent / High / Normal)
undo_stack.h/.c       Linked-list stack used for undo
bst.h/.c              Binary Search Tree of complaints keyed by ID
search_sort.h/.c      linear_search, binary_search, five sorts, comparators, benchmarks
sample_data.h/.c      12 sample complaints and a random complaint generator for benchmarks
util.h/.c             Safe allocation, string helpers, date conversion
test_civicdesk.c      19 automated tests for the structures, algorithms and system rules
Makefile              Builds civicdesk and test_civicdesk
README.md             This document
LICENSE               MIT License
```

## Important limits / future improvement

* All data lives in memory and is lost when the program closes. Saving to a file (for example a binary or CSV file) would make complaints persistent.
* The BST is not self-balancing. An AVL or Red-Black tree would guarantee O(log n) even for sorted IDs.
* Priority has only three fixed levels. A heap-based priority queue would allow finer ranking, for example by pending days within a priority.
* Undo covers status updates only; filing and processing a complaint cannot be undone.
* Text fields have fixed maximum lengths (names and areas up to 31 characters, remarks up to 63) and longer input is cut off.
* The interface is a console menu. A web or desktop front end would let citizens file and track complaints themselves.

## Scope and handoff status

The requested direction is a complaint-management project built only on the topics covered in the Data Structures course: searching, sorting, linked lists, stack, queue and BST, at a moderate difficulty level. The system is a classroom demonstration and not a replacement for a real government grievance portal.

**Implemented in this version:** complaint registration; three-level priority queues; status history with linked lists; undo with a stack; BST index; BST, binary and linear search; five sorting algorithms with timing; benchmark; sample data; automated tests; full memory clean-up.

**Verified:** the 19 tests in `test_civicdesk.c` pass, including under AddressSanitizer and UndefinedBehaviorSanitizer with no errors or memory leaks. They check that every sorting algorithm produces the same order as a reference sort on arrays of 0, 1, 2, 50 and 400 items for every sort field, that quick sort handles sorted and reversed input, that merge sort is stable, that BST search and binary search agree for every complaint, that queues serve Urgent before High before Normal and keep first-in-first-out order inside a level, that undo restores earlier statuses in reverse order and never removes "Filed", that duplicate IDs are rejected, and that the system grows past its initial capacity. A full pass through every menu option was also run, including invalid input and end-of-input handling.

**Still pending / sensible next steps:** persistence to a file, a self-balancing tree, a graphical or web interface, and more tests for the menu input handling.

## Project purpose

Educational demonstration of queues, singly linked lists, stacks, binary search trees, linear and binary search, and five classical sorting algorithms applied to a realistic public-service problem, implemented in C with manual memory management. It does not handle real citizen data.

## License

This project is licensed under the [MIT License](LICENSE).

## Author

**Pulegari Shashi Kiran Reddy**
B.Tech CSE, 2nd Year — Symbiosis Institute of Technology, Hyderabad
GitHub: [pulegarisashikirran](https://github.com/pulegarisashikirran)

_Built for the Data Structures course, Assessment 3 (October 2026)._

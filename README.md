# CivicDesk: Public Grievance Redressal System

A console-based grievance management system built in Python as a **Data Structures project** (Assessment 3). Citizens file complaints, complaints are prioritised and processed in order, every complaint keeps a full status history, mistakes can be undone, and officers can search and sort records using classical algorithms.

Every core feature is built on one of six foundational topics: **searching, sorting, linked lists, stack, queue and binary search tree (BST)**. No external libraries are used.

**Repository:** `https://github.com/pulegarisashikirran/civicdesk-grievance-system`

---

## Problem Statement

In many public offices, citizen complaints (water supply, electricity, roads, sanitation) are tracked on paper or in unstructured sheets. This causes three problems: urgent complaints wait behind routine ones, citizens cannot trace what happened to their complaint, and officers waste time finding records. This project addresses all three using the right data structure for each job.

---

## What is Implemented

- File a complaint with ID, citizen name, area, department and priority (Urgent / High / Normal).
- Three-level priority processing using separate FIFO queues.
- Complete status history per complaint (Filed → Assigned → In Progress → Resolved).
- Undo the last status update using a stack.
- Fast lookup of any complaint by ID using a BST.
- Search by ID, citizen name or area (linear, binary and BST search).
- Sort complaints by pending days, area or ID using five sorting algorithms.
- Timing comparison of the sorting and searching algorithms on large sample data.
- Pre-loaded sample data for quick demonstration.

---

## Project Structure

```
civicdesk-grievance-system/
├── main.py             # Menu-driven interface
├── complaint.py        # Complaint class
├── bst.py              # Binary Search Tree (complaints keyed by ID)
├── queues.py           # Linked-list queue + priority queue manager
├── status_list.py      # Singly linked list for status history
├── undo_stack.py       # Stack for undo
├── search_sort.py      # Linear/binary search + 5 sorting algorithms
├── sample_data.py      # Sample complaints for demo and benchmarking
├── report/             # Project report and presentation slides
└── README.md
```

---

## Data Structures & Algorithms — Topics Used, Locations & Logic

### Quick Reference Matrix

| Required Topic | File Location | Data Structure / Function | Role in the System | Time / Space Complexity |
|---|---|---|---|---|
| Queue | `queues.py` | `LinkedQueue`, `PriorityQueueManager` | Processes complaints first-come, first-served within each priority level | O(1) enqueue / dequeue, O(n) space |
| Singly Linked List | `status_list.py` | `StatusNode`, `StatusHistory` | Stores the ordered status trail of each complaint | O(1) append (tail pointer), O(k) traversal |
| Stack | `undo_stack.py` | `UndoStack` | Reverts the most recent status update | O(1) push / pop, O(n) space |
| Binary Search Tree | `bst.py` | `ComplaintBST` | Stores all complaints by ID for fast lookup and sorted listing | O(log n) average, O(n) worst case |
| Linear Search | `search_sort.py` | `linear_search()` | Finds complaints by citizen name or area | O(n) time, O(1) space |
| Binary Search | `search_sort.py` | `binary_search()` | Finds a complaint in a sorted list by ID | O(log n) time, O(1) space |
| Sorting | `search_sort.py` | `bubble_sort()`, `selection_sort()`, `insertion_sort()`, `merge_sort()`, `quick_sort()` | Generates sorted reports (by pending days, area, ID) | See complexity tables below |

---

### Detailed Breakdown

#### 1. Queue (FIFO)

- **Where used:** `queues.py`
- **Logic:**
  - Implemented with a linked list that keeps `front` and `rear` pointers.
  - `enqueue()` adds at the rear; `dequeue()` removes from the front, both in O(1).
  - The system keeps **three queues**: Urgent, High and Normal.
  - "Process next complaint" always serves the first non-empty queue in that order, so urgent complaints are never stuck behind routine ones, while complaints of equal priority are served strictly in arrival order.
  - No heap is needed, as priority is handled by the fixed number of queues.

#### 2. Singly Linked List

- **Where used:** `status_list.py`
- **Logic:**
  - Each complaint owns a linked list of status nodes (`status`, `date`, `remarks`, `next`).
  - A `tail` pointer makes appending a new status O(1).
  - Traversing from the head prints the full history in the order events happened.
  - Removing the last node (for undo) needs a traversal to find the previous node, which is O(k), where k is the number of statuses (at most 5), so it is effectively constant.

#### 3. Stack (LIFO)
CivicDesk: Public Grievance Redressal System


A console-based grievance management system built in Python as a Data Structures project (Assessment 3). Citizens file complaints, complaints are prioritised and processed in order, every complaint keeps a full status history, mistakes can be undone, and officers can search and sort records using classical algorithms.


Every core feature is built on one of six foundational topics: searching, sorting, linked lists, stack, queue and binary search tree (BST). No external libraries are used.


Repository: https://github.com/pulegarisashikirran/civicdesk-grievance-system



Problem Statement


In many public offices, citizen complaints (water supply, electricity, roads, sanitation) are tracked on paper or in unstructured sheets. This causes three problems: urgent complaints wait behind routine ones, citizens cannot trace what happened to their complaint, and officers waste time finding records. This project addresses all three using the right data structure for each job.



What is Implemented


File a complaint with ID, citizen name, area, department and priority (Urgent / High / Normal).
Three-level priority processing using separate FIFO queues.
Complete status history per complaint (Filed → Assigned → In Progress → Resolved).
Undo the last status update using a stack.
Fast lookup of any complaint by ID using a BST.
Search by ID, citizen name or area (linear, binary and BST search).
Sort complaints by pending days, area or ID using five sorting algorithms.
Timing comparison of the sorting and searching algorithms on large sample data.
Pre-loaded sample data for quick demonstration.



Project Structure


civicdesk-grievance-system/
├── main.py             # Menu-driven interface
├── complaint.py        # Complaint class
├── bst.py              # Binary Search Tree (complaints keyed by ID)
├── queues.py           # Linked-list queue + priority queue manager
├── status_list.py      # Singly linked list for status history
├── undo_stack.py       # Stack for undo
├── search_sort.py      # Linear/binary search + 5 sorting algorithms
├── sample_data.py      # Sample complaints for demo and benchmarking
├── report/             # Project report and presentation slides
└── README.md




Data Structures & Algorithms — Topics Used, Locations & Logic


Quick Reference Matrix




Required Topic
File Location
Data Structure / Function
Role in the System
Time / Space Complexity




Queue
queues.py
LinkedQueue, PriorityQueueManager
Processes complaints first-come, first-served within each priority level
O(1) enqueue / dequeue, O(n) space


Singly Linked List
status_list.py
StatusNode, StatusHistory
Stores the ordered status trail of each complaint
O(1) append (tail pointer), O(k) traversal


Stack
undo_stack.py
UndoStack
Reverts the most recent status update
O(1) push / pop, O(n) space


Binary Search Tree
bst.py
ComplaintBST
Stores all complaints by ID for fast lookup and sorted listing
O(log n) average, O(n) worst case


Linear Search
search_sort.py
linear_search()
Finds complaints by citizen name or area
O(n) time, O(1) space


Binary Search
search_sort.py
binary_search()
Finds a complaint in a sorted list by ID
O(log n) time, O(1) space


Sorting
search_sort.py
bubble_sort(), selection_sort(), insertion_sort(), merge_sort(), quick_sort()
Generates sorted reports (by pending days, area, ID)
See complexity tables below





Detailed Breakdown


1. Queue (FIFO)


Where used: queues.py
Logic:

Implemented with a linked list that keeps front and rear pointers.
enqueue() adds at the rear; dequeue() removes from the front, both in O(1).
The system keeps three queues: Urgent, High and Normal.
"Process next complaint" always serves the first non-empty queue in that order, so urgent complaints are never stuck behind routine ones, while complaints of equal priority are served strictly in arrival order.
No heap is needed, as priority is handled by the fixed number of queues.




2. Singly Linked List


Where used: status_list.py
Logic:

Each complaint owns a linked list of status nodes (status, date, remarks, next).
A tail pointer makes appending a new status O(1).
Traversing from the head prints the full history in the order events happened.
Removing the last node (for undo) needs a traversal to find the previous node, which is O(k), where k is the number of statuses (at most 5), so it is effectively constant.




3. Stack (LIFO)


Where used: undo_stack.py
Logic:

Every status update pushes a record (complaint_id, previous_status, new_status) onto the stack.
"Undo" pops the top record and removes the latest status from that complaint's linked list.
LIFO order guarantees the most recent action is the first one reversed.
push, pop and peek are all O(1).




4. Binary Search Tree


Where used: bst.py
Logic:

Each node holds a complaint; smaller IDs go to the left subtree, larger IDs to the right.
insert() walks down the tree iteratively and rejects duplicate IDs.
search(id) follows one path from the root, giving O(log n) on a balanced tree.
In-order traversal (left, root, right) returns all complaints sorted by ID without any extra sorting step.
Limitation: if IDs are inserted in sorted order the tree becomes a straight line and operations degrade to O(n). The system uses non-sequential sample IDs to avoid this; self-balancing trees (AVL / Red-Black) are listed under future scope.




5. Linear Search


Where used: search_sort.py, called from the Search menu
Logic:

Scans each complaint from index 0 to n − 1 and checks a condition (name or area matches).
Works on unsorted data, so it is used for fields that are not the sorting key.
Worst and average time O(n); space O(1).




6. Binary Search


Where used: search_sort.py
Logic:

Works only on a sorted list. Compares the target with the middle element and discards half the list each step.
Used on the in-order list of complaints (sorted by ID).
Iterative version, so auxiliary space is O(1); worst case O(log n).




7. Sorting Algorithms


Where used: search_sort.py, called from the Reports menu
Logic:

Bubble sort: repeatedly swaps adjacent out-of-order items; stops early if a pass makes no swaps.
Selection sort: repeatedly selects the smallest remaining item and places it in position.
Insertion sort: grows a sorted prefix by inserting each new item into its correct place.
Merge sort: divides the list in half, sorts each half, and merges; guaranteed O(n log n).
Quick sort: partitions around a pivot and recursively sorts both sides; fastest on average.
All five sort by any chosen key (pending days, area, ID), and a benchmark option times each one on the same data.





Complexity Summary


Searching Algorithms




Algorithm
Best Time
Average Time
Worst Time
Space




Linear Search
O(1)
O(n)
O(n)
O(1)


Binary Search (iterative)
O(1)
O(log n)
O(log n)
O(1)


BST Search
O(1)
O(log n)
O(n)
O(1)




Sorting Algorithms




Algorithm
Best Time
Average Time
Worst Time
Auxiliary Space




Bubble Sort
O(n)*
O(n²)
O(n²)
O(1)


Selection Sort
O(n²)
O(n²)
O(n²)
O(1)


Insertion Sort
O(n)
O(n²)
O(n²)
O(1)


Merge Sort
O(n log n)
O(n log n)
O(n log n)
O(n)


Quick Sort
O(n log n)
O(n log n)
O(n²)
O(log n) average**




* Bubble sort reaches O(n) only with the early-exit optimisation on already sorted data.
** Quick sort's O(log n) space is the average recursion depth; it can reach O(n) in the worst case.



How a Complaint Flows Through the System


Filed: the complaint is created, inserted into the BST, and enqueued in the Urgent / High / Normal queue. Its status list starts with "Filed".
Processed: the officer selects "Process next". The queue manager dequeues the highest-priority complaint, and "Assigned" is appended to its linked list.
Updated: the officer moves the status forward (In Progress, Resolved). Each update is appended to the linked list and recorded on the undo stack.
Corrected: if a wrong status was entered, undo pops the stack and removes that status.
Tracked: citizens and officers search by ID (BST / binary search) or by name and area (linear search), and view the status history.
Reported: officers sort complaints by pending days, area or ID to see what needs attention.



Menu Options


1. File a new complaint
2. Process next complaint
3. Update complaint status
4. Undo last status update
5. Search complaints (by ID / name / area)
6. View status history of a complaint
7. Sort and view reports
8. Benchmark sorting and searching
9. Exit




How to Run


Requirements: Python 3.8 or higher. No external libraries.


git clone https://github.com/pulegarisashikirran/civicdesk-grievance-system.git
cd civicdesk-grievance-system
python main.py



Sample complaints are loaded automatically on start so every option can be tried immediately.



Sample Output


Add screenshots of the menu, a processed complaint, a status history, and the benchmark table here.



Limitations & Future Scope


Data is held in memory; saving to a file or database would make it persistent.
The BST is not self-balancing; AVL or Red-Black trees would guarantee O(log n).
Priority is limited to three fixed levels; a heap-based priority queue would allow finer ranking.
A web or mobile front end would let citizens file and track complaints directly.
SMS / e-mail alerts could notify citizens when the status changes.




Built for the Data Structures course, Assessment 3 (October 2026).
- **Where used:** `undo_stack.py`
- **Logic:**
  - Every status update pushes a record `(complaint_id, previous_status, new_status)` onto the stack.
  - "Undo" pops the top record and removes the latest status from that complaint's linked list.
  - LIFO order guarantees the most recent action is the first one reversed.
  - `push`, `pop` and `peek` are all O(1).

#### 4. Binary Search Tree

- **Where used:** `bst.py`
- **Logic:**
  - Each node holds a complaint; smaller IDs go to the left subtree, larger IDs to the right.
  - `insert()` walks down the tree iteratively and rejects duplicate IDs.
  - `search(id)` follows one path from the root, giving O(log n) on a balanced tree.
  - **In-order traversal** (left, root, right) returns all complaints sorted by ID without any extra sorting step.
  - **Limitation:** if IDs are inserted in sorted order the tree becomes a straight line and operations degrade to O(n). The system uses non-sequential sample IDs to avoid this; self-balancing trees (AVL / Red-Black) are listed under future scope.

#### 5. Linear Search

- **Where used:** `search_sort.py`, called from the Search menu
- **Logic:**
  - Scans each complaint from index 0 to n − 1 and checks a condition (name or area matches).
  - Works on unsorted data, so it is used for fields that are not the sorting key.
  - Worst and average time O(n); space O(1).

#### 6. Binary Search

- **Where used:** `search_sort.py`
- **Logic:**
  - Works only on a **sorted** list. Compares the target with the middle element and discards half the list each step.
  - Used on the in-order list of complaints (sorted by ID).
  - Iterative version, so auxiliary space is O(1); worst case O(log n).

#### 7. Sorting Algorithms

- **Where used:** `search_sort.py`, called from the Reports menu
- **Logic:**
  - **Bubble sort:** repeatedly swaps adjacent out-of-order items; stops early if a pass makes no swaps.
  - **Selection sort:** repeatedly selects the smallest remaining item and places it in position.
  - **Insertion sort:** grows a sorted prefix by inserting each new item into its correct place.
  - **Merge sort:** divides the list in half, sorts each half, and merges; guaranteed O(n log n).
  - **Quick sort:** partitions around a pivot and recursively sorts both sides; fastest on average.
  - All five sort by any chosen key (pending days, area, ID), and a benchmark option times each one on the same data.

---

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
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) average\*\* |

\* Bubble sort reaches O(n) only with the early-exit optimisation on already sorted data.
\*\* Quick sort's O(log n) space is the average recursion depth; it can reach O(n) in the worst case.

---

## How a Complaint Flows Through the System

1. **Filed:** the complaint is created, inserted into the **BST**, and enqueued in the **Urgent / High / Normal queue**. Its status list starts with "Filed".
2. **Processed:** the officer selects "Process next". The **queue manager** dequeues the highest-priority complaint, and "Assigned" is appended to its **linked list**.
3. **Updated:** the officer moves the status forward (In Progress, Resolved). Each update is appended to the linked list and recorded on the **undo stack**.
4. **Corrected:** if a wrong status was entered, **undo** pops the stack and removes that status.
5. **Tracked:** citizens and officers **search** by ID (BST / binary search) or by name and area (linear search), and view the **status history**.
6. **Reported:** officers **sort** complaints by pending days, area or ID to see what needs attention.

---

## Menu Options

```
1. File a new complaint
2. Process next complaint
3. Update complaint status
4. Undo last status update
5. Search complaints (by ID / name / area)
6. View status history of a complaint
7. Sort and view reports
8. Benchmark sorting and searching
9. Exit
```

---

## How to Run

Requirements: **Python 3.8 or higher**. No external libraries.

```bash
git clone https://github.com/pulegarisashikirran/civicdesk-grievance-system.git
cd civicdesk-grievance-system
python main.py
```

Sample complaints are loaded automatically on start so every option can be tried immediately.

---

## Sample Output

_Add screenshots of the menu, a processed complaint, a status history, and the benchmark table here._

---

## Limitations & Future Scope

- Data is held in memory; saving to a file or database would make it persistent.
- The BST is not self-balancing; AVL or Red-Black trees would guarantee O(log n).
- Priority is limited to three fixed levels; a heap-based priority queue would allow finer ranking.
- A web or mobile front end would let citizens file and track complaints directly.
- SMS / e-mail alerts could notify citizens when the status changes.

---

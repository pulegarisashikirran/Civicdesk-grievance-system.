CivicDesk — Public Grievance Redressal System


CivicDesk is a menu-driven complaint-management application and data-structures project. Citizens' complaints are registered, prioritised through three linked-list queues, tracked through a per-complaint status history, indexed in a binary search tree, and reported with classical searching and sorting algorithms. Every action can be reversed with an undo stack. All names, areas and complaints in the sample data are fictional, and nothing is sent to any real government system.


Repository: https://github.com/pulegarisashikirran/civicdesk-grievance-system


What is implemented


Console application in pure Python 3 with zero external dependencies.
Complaint registration with a generated unique ID, citizen name, area, department and priority (Urgent / High / Normal).
Three FIFO queues (one per priority) with a "process next complaint" action that always serves the highest non-empty priority.
A singly linked list per complaint that records the full status trail: Filed → Assigned → In Progress → Resolved, with dates and remarks.
Undo of the last status update using a stack.
A binary search tree keyed by complaint ID for lookup and sorted listing (in-order traversal).
Search by complaint ID (BST search and binary search), and by citizen name or area (linear search).
Reports sorted by pending days, area, complaint ID or priority using five implemented algorithms: bubble, selection, insertion, merge and quick sort.
A benchmark that times all five sorts and all three search methods on the same random data.
Pending-day calculation that stops counting once a complaint is resolved.
12 pre-loaded sample complaints (some already in progress or resolved) so every feature can be demonstrated immediately.
17 automated unit tests covering the data structures, algorithms and system rules.


Data Structures & Algorithms — Topics Used, Locations & Logic


CivicDesk is architected around six foundational data structures and classical algorithms. Each one is used for the job it is best suited to.


Quick Reference Matrix




Required Topic
File Location
Data Structure / Function
Role in CivicDesk
Time / Space Complexity




Linear Search
search_sort.py
linear_search()
Finds complaints by citizen name or area in the full complaint list
O(n) Time / O(1) Space


Binary Search
search_sort.py
binary_search()
Finds a complaint by ID in the list sorted by ID
O(log n) Time / O(1) Space


Singly Linked List (SLL)
status_list.py
StatusNode, StatusHistory
Chronological status trail of each complaint
O(1) Append / O(k) Remove last / O(k) Space (k ≤ 4)


Queue (Linked List)
queues.py
QueueNode, LinkedQueue
Strict first-in, first-out waiting line for complaints
O(1) Enqueue/Dequeue / O(n) Space


Priority handling (3 queues)
queues.py
PriorityQueueManager
Serves Urgent before High before Normal, FIFO inside each level
O(1) per operation (3 fixed queues)


Stack (LIFO)
undo_stack.py
UndoStack
Remembers status updates so the latest one can be undone first
O(1) Push/Pop/Peek / O(n) Space


Binary Search Tree (BST)
bst.py
BSTNode, ComplaintBST
Indexes every complaint by ID; in-order traversal gives sorted output
O(log n) Avg / O(n) Worst Search & Insert / O(n) Space


Bubble Sort
search_sort.py
bubble_sort()
Sorts reports using adjacent swaps with early termination
O(n²) Worst/Avg, O(n) Best / O(1) Space


Selection Sort
search_sort.py
selection_sort()
Sorts reports by selecting successive minimum values in place
O(n²) Comparisons, O(n) Swaps / O(1) Space


Insertion Sort
search_sort.py
insertion_sort()
Sorts reports by inserting each item into a sorted prefix
O(n²) Worst, O(n) Best / O(1) Space


Merge Sort
search_sort.py
merge_sort()
Stable divide-and-conquer sorting for large complaint lists
O(n log n) Time / O(n) Space


Quick Sort
search_sort.py
quick_sort()
In-place Hoare-partitioned sorting; smaller side recursed first
O(n log n) Avg, O(n²) Worst / O(log n) Space




Detailed Breakdown of Topics, Usage & Underlying Logic


1. Linear Search


Where Used:

search_sort.py (linear_search)
Called from the Search menu in main.py to find complaints by citizen name or area.
Also used by the benchmark as the baseline for ID lookup.


Logic:

Starts at index 0 and steps through every complaint up to n − 1.
Evaluates a predicate such as lambda c: text in c.name.lower(). Every matching complaint is collected.
Does not require the data to be ordered or indexed, which is why it is used for fields that are not the BST key.
Worst and average time complexity: O(n); space complexity: O(1).




2. Binary Search


Where Used:

search_sort.py (binary_search)
Called from the Search menu in main.py on the list returned by bst.inorder(), which is already sorted by complaint ID.


Logic:

Operates only on a list sorted by complaint ID.
Keeps low = 0 and high = len − 1. At each step it computes mid = (low + high) // 2 and compares sorted_items[mid].cid with the target.
If equal, the complaint is returned. If the middle ID is smaller, low = mid + 1; if larger, high = mid − 1.
Halves the candidate range in every iteration: O(log n) time. The iterative version needs only O(1) extra space.




3. Singly Linked List (SLL)


Where Used:

status_list.py (StatusNode, StatusHistory)
One StatusHistory is created inside every Complaint in complaint.py.


Logic:

Each node holds status, remarks, date and a single next reference.
append() links a new node after tail and moves tail, so adding a status takes O(1) time with no array resizing.
display() walks from head through next until None, printing the exact order in which things happened.
remove_last() (used by undo) walks to the node before tail, unlinks the tail and moves tail back. This is O(k) with k ≤ 4 statuses. The first node, "Filed", can never be removed.




4. Queue Using Linked Lists (FIFO)


Where Used:

queues.py (QueueNode, LinkedQueue)


Logic:

Keeps front and rear pointers and a size counter.
enqueue(item) links a node after rear in O(1). dequeue() unlinks the node at front in O(1), and resets rear to None when the queue becomes empty.
Guarantees fairness: among complaints of the same priority, the one filed earlier is always processed first.




5. Priority Handling with Three Queues


Where Used:

queues.py (PriorityQueueManager)
Called from main.py when a complaint is filed (enqueue) and when "Process next complaint" is chosen (dequeue_next).


Logic:

Holds one LinkedQueue each for Urgent, High and Normal.
enqueue(complaint) places the complaint in the queue matching its priority.
dequeue_next() checks Urgent, then High, then Normal, and returns the front of the first non-empty queue.
Result: urgent complaints are never stuck behind routine ones, and order inside each level stays strictly first-in, first-out. No heap is needed because the number of priority levels is fixed.




6. Stack Using a List (LIFO)


Where Used:

undo_stack.py (UndoStack)
Integrated in main.py: update_status pushes, undo_last pops.


Logic:

The top of the stack is the end of an internal list.
Every status update pushes a record (complaint_id, previous_status, new_status) in O(1).
"Undo" pops the most recent record, finds the complaint through the BST, and calls remove_last() on its status list. Last in, first out means the latest change is always the first one reversed.
peek() and is_empty() are also O(1).




7. Binary Search Tree (BST)


Where Used:

bst.py (BSTNode, ComplaintBST)
One tree holds every complaint; main.py uses it for ID search, status updates, undo and history.


Logic:

Each node holds one complaint. IDs smaller than the node go to the left subtree, larger IDs to the right.
insert() walks down iteratively and rejects duplicate IDs. search(id) follows a single root-to-node path.
inorder() visits left, node, right, which always yields complaints sorted by ID with no separate sorting step. This list also feeds binary search.
New complaint IDs are generated randomly (not 1, 2, 3, …) so the tree stays reasonably balanced. Inserting IDs in sorted order would turn it into a straight line and degrade operations to O(n); this behaviour is covered by a unit test. Self-balancing trees (AVL / Red-Black) are listed under future improvements.
Average search and insert: O(log n); worst case O(n).




8–12. Five Sorting Algorithms


Implemented in search_sort.py and interactively selectable in the Sort and view reports menu. Every function takes (items, key) and returns a new sorted list, so the original data is never modified.


Bubble Sort (bubble_sort):

Repeatedly compares adjacent elements and swaps them if they are out of order.
Includes an early-termination optimisation: if a full pass makes no swap, sorting stops (O(n) best case). Worst and average: O(n²).


Selection Sort (selection_sort):

Splits the list into a sorted prefix and an unsorted suffix.
Each pass finds the smallest element in the unsorted part and swaps it into position. Only O(n) swaps, but O(n²) comparisons.


Insertion Sort (insertion_sort):

Builds the sorted list from left to right, shifting larger elements right until the correct slot opens for the current element.
Adaptive: O(n) on nearly sorted data, O(n²) worst case.


Merge Sort (merge_sort):

Splits the list at mid = len // 2, sorts each half recursively and merges the two sorted halves with two pointers.
Uses <= when merging so equal keys keep their order (stable). Guaranteed O(n log n) on every input, with O(n) extra space.


Quick Sort (quick_sort):

Divide-and-conquer with Hoare partitioning around the middle element, working in place.
Always recurses into the smaller partition first and loops over the larger one, which keeps the recursion depth at O(log n). Average time O(n log n); worst case O(n²). Hoare partitioning handles many equal keys (such as equal pending days) without degrading.




Reports can be sorted by pending days (longest first), area, complaint ID or priority. The chosen algorithm and its running time in milliseconds are printed under every report.


How the structures work together


The BST is the master index of all complaints. The three queues hold only the complaints still waiting to be processed. Each complaint carries its own status linked list, and the undo stack remembers recent status updates so they can be reversed. A plain list of all complaints supports linear search and the sorting reports. The binary search runs over the BST's in-order list.


How a Complaint Flows Through the System


A citizen's complaint is entered through File a new complaint. A random unused 4-digit ID is generated.
The complaint is inserted into the BST and placed in the Urgent / High / Normal queue. Its status list starts with "Filed".
Process next complaint takes the first complaint from the highest non-empty queue and appends "Assigned" to its status list.
Update complaint status moves a processed complaint to "In Progress" or "Resolved". The new status is appended to the linked list and a record is pushed on the undo stack. Complaints still waiting in a queue cannot be updated, so the queues always stay consistent.
If a wrong status was entered, Undo pops the stack and removes that status.
Search finds complaints by ID (BST or binary search) or by name or area (linear search), and View status history prints the full trail.
Sort and view reports ranks complaints by the chosen field with the chosen algorithm and shows how long it took.


How Pending Days and Status Tracking Work


CivicDesk tracks how long every complaint has been open:


Open complaints:

Pending days = today's date − the date the complaint was filed.
The value is recalculated every time it is displayed or used as a sort key.


Resolved complaints:

Once the latest status is "Resolved", counting stops at the date of that status.
Pending days = resolution date − filing date, so resolved complaints no longer look overdue.


Live visibility:

The main menu always shows how many complaints are waiting in each queue.
Every table shows ID, name, area, department, priority, current status and pending days. View status history shows the dated trail, including remarks entered by the officer.




Menu Options




Option
Purpose




1 File a new complaint
Register a complaint, generate its ID, add it to the BST and the right queue


2 Process next complaint
Dequeue the highest-priority complaint and mark it "Assigned"


3 Update complaint status
Move a processed complaint to In Progress or Resolved, with optional remarks


4 Undo last status update
Pop the stack and remove the latest status


5 Search complaints
Search by ID (BST), ID (binary search), citizen name or area (linear search)


6 View status history
Print the full status trail of one complaint


7 Sort and view reports
Sort by pending days, area, ID or priority with any of the five algorithms


8 Benchmark
Time all sorts and searches on random data


9 Exit
Quit the program




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
O(log n)**




* Bubble sort reaches O(n) only with the early-exit check on already sorted data.
** Recursion always continues into the smaller partition first, so the stack depth stays O(log n).


Local development


Requires Python 3.8 or newer. No packages to install.


git clone https://github.com/pulegarisashikirran/civicdesk-grievance-system.git
cd civicdesk-grievance-system
python main.py



Run the automated tests:


python test_civicdesk.py



The benchmark option (menu item 8) takes about 10 seconds because it deliberately runs the O(n²) sorts on up to 2,000 complaints.


Benchmark results


Run menu option 8 on your own machine and paste the table here. Times vary by computer.




Complaints (n)
Bubble
Selection
Insertion
Merge
Quick




500







1000







2000









Sample output


Add screenshots of the menu, processing a complaint, a status history, an undo, a search and a sorted report here.


Main files


main.py             Menu-driven interface and the GrievanceSystem that connects all structures
complaint.py        Complaint class (owns a StatusHistory), priorities, departments, status flow
bst.py              Binary Search Tree of complaints keyed by ID
queues.py           LinkedQueue (FIFO) and PriorityQueueManager (Urgent / High / Normal)
status_list.py      Singly linked list for each complaint's status history
undo_stack.py       Stack used for undo
search_sort.py      linear_search, binary_search, five sorts, benchmark helpers
sample_data.py      12 sample complaints and a random complaint generator for benchmarks
test_civicdesk.py   17 automated tests for the structures, algorithms and system rules
README.md           This document
LICENSE             MIT License



Important limits / future improvement


All data lives in memory and is lost when the program closes. Saving to a file or a database (for example SQLite) would make complaints persistent.
The BST is not self-balancing. An AVL or Red-Black tree would guarantee O(log n) even for sorted IDs.
Priority has only three fixed levels. A heap-based priority queue would allow finer ranking, for example by pending days within a priority.
Undo covers status updates only; filing and processing a complaint cannot be undone.
The interface is a console menu. A web front end would let citizens file and track complaints themselves.
SMS or e-mail alerts could notify a citizen whenever the status of their complaint changes.


Scope and handoff status


The requested direction is a complaint-management project built only on the topics covered in the Data Structures course: searching, sorting, linked lists, stack, queue and BST, at a moderate difficulty level. The system is a classroom demonstration and not a replacement for a real government grievance portal.


Implemented in this version: complaint registration; three-level priority queues; status history with linked lists; undo with a stack; BST index; BST, binary and linear search; five sorting algorithms with timing; benchmark; sample data; automated tests.


Verified: the 17 unit tests in test_civicdesk.py pass. They check that every sorting algorithm matches Python's built-in ordering on lists of 0, 1, 2, 50 and 400 items (including heavy duplicate keys) and that quick sort handles sorted and reversed input, that BST search and binary search agree for every complaint, that queues serve Urgent before High before Normal and keep first-in-first-out order inside a level, that undo restores earlier statuses in reverse order and never removes "Filed", and that duplicate IDs are rejected.


Still pending / sensible next steps: persistence, a self-balancing tree, a web interface, and more tests for the menu input handling.


Project purpose


Educational demonstration of queues, singly linked lists, stacks, binary search trees, linear and binary search, and five classical sorting algorithms applied to a realistic public-service problem. It does not handle real citizen data.


License


This project is licensed under the MIT License.




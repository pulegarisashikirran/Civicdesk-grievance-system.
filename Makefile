CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2
MODULES = util.c status_list.c complaint.c bst.c queues.c undo_stack.c \
          search_sort.c sample_data.c grievance_system.c

all: civicdesk test_civicdesk

civicdesk: main.c $(MODULES)
	$(CC) $(CFLAGS) -o $@ main.c $(MODULES)

test_civicdesk: test_civicdesk.c $(MODULES)
	$(CC) $(CFLAGS) -o $@ test_civicdesk.c $(MODULES)

test: test_civicdesk
	./test_civicdesk

clean:
	rm -f civicdesk test_civicdesk civicdesk.exe test_civicdesk.exe *.o

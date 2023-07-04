
#define MAX_NAME_LEN 8
#define MEMORY_SIZE 2048
#ifndef LINKEDLISTS_H
#define LINKEDLISTS_H

// QUEUE STRUCTS
typedef struct {
    int time_arrived;
    char process_name[MAX_NAME_LEN + 1];
    int service_time;
    int memory_requirement;
    int time_remaining;
    int finish_time;
    int pid;
     
} data_t;

typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct{
    node_t *head;
    node_t *foot;
    int count;
} list_t;

// MEMORY STRUCTS
typedef struct {
    char process_name[MAX_NAME_LEN + 1];
    int start;
    int end;
} memory_data_t;

typedef struct memory memory_t;

struct memory {
    memory_data_t data;
    memory_t *next;
    memory_t *prev;
};

// QUEUE FUNCTIONS

// Malloc an empty linked list
list_t *make_empty_list(void);

/* Malloc a node and assign `value` to node->data inserting at the 
foot of the list 
*/
void *insert_at_foot(list_t *list, data_t value);

// Sorted insert into list
void *sorted_insert(list_t *list, data_t value);

// Remove a node from a list
void *pop_from_list(list_t *list, node_t *node);

// Remove any node from a list
void *pop_from_list_any(list_t *list, char* name);

// Print the list
void print_list(list_t *list);

// Free all nodes of the list
void free_list(list_t *list);

// MEMORY FUNCTIONS

// Malloc an empty memory list
memory_t *make_empty_memory(void);

// Split a memory node
memory_t *split_memory_node(memory_t *parent, int offset);

// Print Memory
void *print_memory(memory_t *memory);

// Allocate memory
int allocate_memory(memory_t *memory, int *address, data_t temp);

// Deallocate Memory
void *deallocate_memory(memory_t *memory, data_t temp);

#endif
/*
*
* This is adapted from my personal use document for LinkedList data-structures that I have built over my past COMP subjects.
* Some functions are adapted, and/or added.
*
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlists.h"


// Malloc an empty linked list
list_t *make_empty_list(void) {
	list_t* list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = NULL;
    list->foot = NULL;
    list->count = 0;
	return list;
}

/* Malloc a node and assign `value` to node->data inserting at the 
foot of the list 
*/
void *insert_at_foot(list_t *list, data_t value) {
	node_t *temp;
	temp = (node_t*)malloc(sizeof(*temp));
	assert(list!=NULL && temp!=NULL);
	temp->data = value;
	temp->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = temp;
	} else {
		list->foot->next = temp;
		list->foot = temp;
	}
    
    list->count++;
    return 0;
}

void *sorted_insert(list_t *list, data_t value) {
    node_t *temp;
	temp = (node_t*)malloc(sizeof(*temp));
    assert(list!=NULL && temp!=NULL);
    temp->data = value;
	temp->next = NULL;

    if (list->foot == NULL) {
        list->head = list->foot = temp;
    } 
    else if (list->head->data.service_time > temp->data.service_time) {
        temp->next = list->head;
        list->head = temp;
    }
    else {
        node_t *curr = list->head;
        while (curr->next != NULL && curr->next->data.service_time < temp->data.service_time) {
            curr = curr->next;
        }
        if(curr->next == NULL) {
            temp->next = curr->next;
            curr->next = temp;
        }
        else if(curr->next->data.service_time > temp->data.service_time) {
            temp->next = curr->next;
            curr->next = temp;
        }
        else {
            if (curr->next->data.time_arrived > temp->data.time_arrived) {
                temp->next = curr->next;
                curr->next = temp;
            }
            else if (curr->next->data.time_arrived < temp->data.time_arrived) {
                curr = curr->next;
                temp->next = curr->next;
                curr->next = temp;
            }
            else {
                if (strcmp(curr->next->data.process_name, temp->data.process_name) < 0) {
                    curr = curr->next;
                }
                temp->next = curr->next;
                curr->next = temp;
            }
        }
    }
    list->count++;
    return 0;
}

// Remove a node from a list
void *pop_from_list(list_t *list, node_t *node) {
    
    if(list->head == NULL){
        return 0;
    }

    node_t *prev;
    node_t *curr;
    curr = list->head;


    if(list->head == node) {
        if(list->head == list->foot) {
            list->foot = NULL;
        }
        list->head = list->head->next;
        list->count--;
        free(curr);
        return 0;
    }

    curr = list->head;

    while(curr != NULL && node) {
        if(curr == node) {
            prev->next = curr->next;
            list->count--;
            free(curr);
            return 0;
        } 
        else  {
            prev = curr;
            curr = curr->next;
        }
    }
    return 0;
}

void *pop_from_list_any(list_t *list, char* name) {
    if(list->head == NULL){
        return 0;
    }
    node_t *prev;
    node_t *curr;
    curr = list->head;

    if(strcmp(list->head->data.process_name, name) == 0) {
        if(list->head == list->foot) {
            list->foot = NULL;
        }
        list->head = list->head->next;
        list->count--;
        free(curr);
        return 0;
    }

    while(curr) {
        if(strcmp(curr->data.process_name, name) == 0) {
            prev->next = curr->next;
            list->count--;
            if(curr->next == NULL) {
                list->foot = prev;
            }
            free(curr);
            return 0;
        } 
        else  {
            prev = curr;
            curr = curr->next;
        }
    }
    return 0;

}

// Print List
void print_list(list_t *list) {
    printf("=========== PRINTING LIST =========== \n");
    assert(list!=NULL);
    node_t *curr;
    curr = list->head;
    int counter = 0;
    while (curr != NULL) {
        printf("NODE %d: %d %s %d %d %d %d\n", counter, curr->data.time_arrived, curr->data.process_name, curr->data.service_time, curr->data.memory_requirement, curr->data.time_remaining, curr->data.finish_time);
        curr = curr -> next;
        counter ++;
    }
    printf("===================================== \n");
}

// Free all nodes of the list
void free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

// Malloc an empty memory list
memory_t *make_empty_memory(void) {
	memory_t* memory;
	memory = (memory_t*) malloc(sizeof(*memory));
	assert(memory != NULL);
    strcpy(memory->data.process_name , "");
    memory->data.start = 0;
    memory->data.end = MEMORY_SIZE - 1;
    memory->next = NULL;
    memory->prev = NULL;
	return memory;
}

// Split a memory node
memory_t *split_memory_node(memory_t *parent, int offset) {
    memory_t* child;
    child = (memory_t*) malloc(sizeof(*child));
    assert(child!=NULL);
    strcpy(child->data.process_name, "");
    child->data.start = parent->data.start + offset;
    child->data.end = parent->data.end;
    parent->data.end = child->data.start - 1;
    child->next = parent->next;
    parent->next = child;
    child->prev = parent;
    return child;

}

// Print Memory
void *print_memory(memory_t *memory) {
    memory_t *curr;
    curr = memory;
    printf("=========== PRINTING LIST =========== \n");
    while(curr) {
        printf("Name: %s Start: %d End: %d Size: %d\n", curr->data.process_name, curr->data.start, curr->data.end, curr->data.end - curr->data.start + 1);
        curr = curr->next;
    }
    printf("===================================== \n");
    return memory;
}

// Allocate memory
int allocate_memory(memory_t *memory, int *address, data_t temp) {
    int best_fit = MEMORY_SIZE;
    memory_t *curr;
    curr = memory;

    // Find the best fit block of memory
    while(curr){
        int size = curr->data.end - curr->data.start + 1;
        if(size > 0) {
            if(size == temp.memory_requirement && strcmp(curr->data.process_name, "") == 0) {
                strcpy(curr->data.process_name, temp.process_name);
                *address = curr->data.start;
                return 1;
            }
            else if(size >= temp.memory_requirement && size < best_fit && strcmp(curr->data.process_name, "") == 0) {
                best_fit = size;
            }
        }
        curr = curr->next;
    }
    curr = memory;

    // Update the found block, splitting the block if necessary
    while(curr) {
        int size = curr->data.end - curr->data.start + 1;
        if(size == best_fit && strcmp(curr->data.process_name, "") == 0 && temp.memory_requirement <= size) {
            strcpy(curr->data.process_name, temp.process_name);
            *address = curr->data.start;
            split_memory_node(curr, temp.memory_requirement);
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

// Deallocate Memory
void *deallocate_memory(memory_t *memory, data_t temp) {
    if (memory == NULL) {
        return memory;
    }

    if (strcmp(memory->data.process_name, temp.process_name) == 0) {
        strcpy(memory->data.process_name, "");
        if(strcmp(memory->next->data.process_name, "") == 0) {
            memory_t *temp;
            temp = memory->next;
            memory->data.end = memory->next->data.end;
            memory->next = memory->next->next;
            if(memory->next) {
                memory->next->prev = memory;
            }
            free(temp);
        }
        return memory;
    }

    memory_t *curr;
    curr = memory;
    while (curr) {
        if(strcmp(curr->data.process_name, temp.process_name) == 0) {
            strcpy(curr->data.process_name, "");

            if(curr->next) {
                if(strcmp(curr->next->data.process_name, "") == 0) {
                memory_t *temp;
                temp = curr->next;
                curr->data.end = curr->next->data.end;
                curr->next = curr->next->next;
                if(curr->next) {
                    curr->next->prev = curr;
                }
                free(temp);
                }
            }

            if(strcmp(curr->prev->data.process_name, "") == 0) {
                curr = curr->prev;
                memory_t *temp;
                temp = curr->next;
                curr->data.end = curr->next->data.end;
                curr->next = curr->next->next;
                if(curr->next) {
                    curr->next->prev = curr;
                }
                free(temp);
            }
            return memory;
        }
        curr = curr->next;
    }
    return memory;
}
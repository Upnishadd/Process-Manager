#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include "utility.h"
#include "linkedlists.h"

// Read file
void read_file(char *ptr, size_t ptr_buffer_length, FILE* stream, list_t* processes) {
    const char delim[DELIM_SIZE] = " ";
    while(getline(&ptr, &ptr_buffer_length, stream) > 0) {
        // Replace newline with null terminator
        int len = strlen(ptr);
        ptr[len - 1] = '\0';
        node_t temp;

        char *current = strtok(ptr, delim);
        temp.data.time_arrived = atoi(current);
        current = strtok(NULL, delim);
        strcpy(temp.data.process_name, current);
        current = strtok(NULL, delim);
        temp.data.service_time = atoi(current);
        current = strtok(NULL, delim);
        temp.data.memory_requirement = atoi(current);
        temp.data.time_remaining = temp.data.service_time;
        insert_at_foot(processes, temp.data);
    }
    free(ptr);
}

// Find and print turnaround time
void turnaround_time(list_t *list) {
    int count = 0;
    int sum = 0;

    node_t *curr = list->head;

    while(curr) {
        sum += (curr->data.finish_time - curr->data.time_arrived);
        count++;
        curr = curr->next;
    }
    double avrg = (double) sum / count;
    printf("Turnaround time %d\n", (int)ceil(avrg));
}

// Find and print time overhead
void time_overhead(list_t *list) {
    double max = 0;
    double count = 0;
    double sum = 0;

    node_t *curr = list->head;

    while(curr) {
        double time = (double)(curr->data.finish_time - curr->data.time_arrived) / curr->data.service_time;
        sum += time;
        if(time > max) {
            max = time;
        }
        count++; 
        curr = curr->next;
    }
    double avrg = sum / count;
    max = round(max * 100) / 100;
    avrg = round(avrg * 100) / 100;
    printf("Time overhead %.2lf %.2lf\n", max, avrg);
}

// Move process to input
void move_to_input(list_t* processes, int simulation_time, list_t* input) {
    // Move from Process -> Input
    node_t *curr_process;
    node_t *next_process;
    curr_process = processes->head;
    while(curr_process) {
        next_process = curr_process->next;
        if (curr_process->data.time_arrived <= simulation_time) {
            data_t temp = curr_process->data;
            insert_at_foot(input, temp);
            pop_from_list_any(processes, curr_process->data.process_name);
        }
        curr_process = next_process;
    }
}

// Move process to ready
void move_to_ready(list_t* input, int simulation_time, list_t* ready, memory_t *memory, bool preemptive, bool memory_inf) {
    node_t *curr_input;
    node_t *next_input;
    curr_input = input->head;
    while(curr_input) {
        next_input = curr_input->next;
        int address;
        if(memory_inf) {
            data_t temp = curr_input->data;
            if(preemptive) {
                insert_at_foot(ready, temp);
            }
            else {
                sorted_insert(ready, temp);
            }
            pop_from_list_any(input, curr_input->data.process_name);
        }
        else if (allocate_memory(memory, &address, curr_input->data) == 1) {
            printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, curr_input->data.process_name, address);
            data_t temp = curr_input->data;
            if(preemptive) {
                insert_at_foot(ready, temp);
            }
            else {
                sorted_insert(ready, temp);
            }
            pop_from_list_any(input, curr_input->data.process_name);
        }

        curr_input = next_input;
    }
}

// Move process to running
void move_to_running(list_t* ready, int simulation_time, list_t*running, bool preemptive) {
    if (preemptive) {
        if (running->head) {
            data_t curr_running = running->head->data;
            insert_at_foot(ready, curr_running);
            pop_from_list(running, running->head); 
        }
        data_t temp = ready->head->data;
        insert_at_foot(running, temp);
        pop_from_list(ready, ready->head);
        printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, running->head->data.process_name, running->head->data.time_remaining);
    
    }

    else {

        if(running->head == NULL) {
            data_t temp = ready->head->data;
            insert_at_foot(running, temp);
            pop_from_list(ready, ready->head); 
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, running->head->data.process_name, running->head->data.time_remaining);
        }

    }
}

// Suspend a process
void suspend_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end) {
    write(stdin_pipe, &big_end, sizeof(big_end));
    kill(running->head->data.pid, SIGTSTP);
    int wstatus;
    waitpid(running->head->data.pid, &wstatus, WUNTRACED);
}

// Continue / Resume a process
void cont_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end) {
    char ptr;
    size_t size_of_ptr = 1;
    write(stdin_pipe, &big_end, sizeof(big_end));
    kill(running->head->data.pid, SIGCONT);
    read(stdout_pipe, &ptr, size_of_ptr);
}

// Terminate a Process
void terminate_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end, int simulation_time) {
    write(stdin_pipe, &big_end, sizeof(big_end));
    kill(running->head->data.pid, SIGTERM);
    char ptr[SHA_SIZE+1];
    ptr[SHA_SIZE] = '\0';
    read(stdout_pipe, &ptr, sizeof(ptr));
    printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s", simulation_time, running->head->data.process_name, ptr);
}
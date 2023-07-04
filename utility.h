#define DELIM_SIZE 2
#define SHA_SIZE 65
#include "linkedlists.h"
#ifndef UTILITY_H
#define UTILITY_H

// Read file
void read_file(char *ptr, size_t ptr_buffer_length, FILE* stream, list_t* processes);

// Find and print turnaround time
void turnaround_time(list_t *list);

// Find and print time overhead
void time_overhead(list_t *list);

// Move process to input
void move_to_input(list_t* processes, int simulation_time, list_t* input);

// Move process to ready
void move_to_ready(list_t* input, int simulation_time, list_t* ready, memory_t *memory, bool preemptive, bool memory_inf);

// Move process to running
void move_to_running(list_t* ready, int simulation_time, list_t*running, bool preemptive);

// Suspend a process
void suspend_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end);

// Continue / Resume a process
void cont_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end);

// Terminate a Process
void terminate_process(int stdin_pipe, int stdout_pipe, list_t* running, uint32_t big_end, int simulation_time);

#endif
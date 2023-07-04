#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "utility.h"
#include "linkedlists.h"

#define INPUT_END 1
#define OUTPUT_END 0
#define PIPE_SIZE 2

#define IMPLEMENTS_REAL_PROCESS

int main(int argc, char *argv[]) {

    FILE* stream;
    bool preemptive, memory_inf;
    int quantam;

    char *ptr = NULL;
    size_t ptr_buffer_length = 0;

    int cycle = 0, simulation_time = 0;

    int stdin_pipe[PIPE_SIZE];
    int stdout_pipe[PIPE_SIZE];

    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // As command line args can be given in any order, run through all arguments to appropriately assign values
    for (int i = 1; i < argc; i++) {
        // Set File Stream
        if(strcmp(argv[i], "-f") == 0) {
            stream = fopen(argv[i+1], "r");
            assert(stream != NULL);
        }
        // Set Schedular Type
        else if (strcmp(argv[i], "-s") == 0) {
            if (strcmp(argv[i + 1], "SJF") == 0) {
                preemptive = false;
            }
            else if (strcmp(argv[i + 1], "RR") == 0) {
                preemptive = true;
            }
        }
        // Set Memory Requirment
        else if (strcmp(argv[i], "-m") == 0) {
            if (strcmp(argv[i + 1], "infinite") == 0) {
                memory_inf = true;
            }
            else if (strcmp(argv[i + 1], "best-fit") == 0) {
                memory_inf = false;
            }
        }
        // Set Quantum
        else if (strcmp(argv[i], "-q") == 0) {
            quantam = atoi(argv[i+1]);
        }
    }
    list_t* processes = make_empty_list();
    list_t* input = make_empty_list();
    list_t* ready = make_empty_list();
    list_t* running = make_empty_list();
    list_t* finished = make_empty_list();
    memory_t* memory = make_empty_memory();
    
    // Go through the file 
    read_file(ptr, ptr_buffer_length, stream, processes); 

    while (true) {
        simulation_time = cycle * quantam;

        uint32_t big_end = htonl((uint32_t) simulation_time);
        int least_significant_byte = (simulation_time & 0xFF);

        //Finished Processes
        if(running->head && running->head->data.time_remaining <= 0) {
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, running->head->data.process_name, ready->count + input->count);
            
            terminate_process(stdin_pipe[INPUT_END], stdout_pipe[OUTPUT_END], running, big_end, simulation_time);
            data_t temp = running->head->data;
            temp.finish_time = simulation_time;
            deallocate_memory(memory, temp);
            insert_at_foot(finished, temp);
            pop_from_list(running, running->head);
        }

        // End Loop
        if((processes->head == NULL) && (input->head == NULL) && (ready->head == NULL) && (running->head == NULL)) {
            break;
        }

        // Move from Process -> Input
        move_to_input(processes, simulation_time, input);

        // Move from input -> ready
        /*if(simulation_time == 921) {
            print_list(input);
            print_list(ready);
            print_memory(memory);
        }*/
        move_to_ready(input, simulation_time, ready, memory, preemptive, memory_inf);
        // Suspend a process if the process requires more CPU time and there are other READY processes
        if (running->head && ready->head && running->head->data.time_remaining > 0 && preemptive 
        && running->head->data.service_time > running->head->data.time_remaining) {
            suspend_process(stdin_pipe[INPUT_END], stdout_pipe[OUTPUT_END], running, big_end);
        }

        if(ready->head) {
             // Move from ready to running
            move_to_running(ready, simulation_time, running, preemptive);
            
            // If new to running
            if(running->head->data.service_time == running->head->data.time_remaining) {
                int child_pid;
                char ptr;
                size_t size_of_ptr = 1;
                do {
                    child_pid = fork();
                    if (child_pid == -1) {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                    else if(child_pid == 0) {
                        dup2(stdin_pipe[OUTPUT_END], STDIN_FILENO);
                        dup2(stdout_pipe[INPUT_END], STDOUT_FILENO);
                        write(stdin_pipe[INPUT_END], &big_end, sizeof(big_end));
                        //char *args[] = {"./process", "-v", running->head->data.process_name , NULL};
                        char *args[] = {"./process", running->head->data.process_name , NULL};
                        execvp(args[0], args);
                    } else {
                        read(stdout_pipe[OUTPUT_END], &ptr, size_of_ptr);
                    }
                    running->head->data.pid = child_pid;
                } while ((uint8_t) ptr != least_significant_byte);
            }
        }

        if(running->head) {
           if(running->head->data.service_time > running->head->data.time_remaining && running->head->data.time_remaining > 0) {
                cont_process(stdin_pipe[INPUT_END], stdout_pipe[OUTPUT_END], running, big_end);
            }
            running->head->data.time_remaining = running->head->data.time_remaining - quantam;
        }

        cycle ++;
    }

    turnaround_time(finished);
    time_overhead(finished);
    printf("Makespan %d\n", simulation_time);


    // Cleanup of all allocations, pipes and fopens
    close(stdin_pipe[INPUT_END]);
    close(stdout_pipe[INPUT_END]);
    close(stdout_pipe[OUTPUT_END]);
    close(stdin_pipe[OUTPUT_END]);
    
    free(ptr);
    free_list(processes);
    free_list(input);
    free_list(ready);
    free_list(running);
    free_list(finished);
    free(memory);
    free(processes);
    free(input);
    free(ready);
    free(running);
    free(finished);
    fclose(stream);
    return 0;
}
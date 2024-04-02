#ifndef PCB_H
#define PCB_H

# // system includes
#include <stdio.h>
// project
#include "parameters.h"

struct _pcb {
    // these are determined before execution
    int pid;
    int arrival;
    int burst;
    int priority;
    int remaining;
    // these are stats determined during runtime
    int* waiting_time;
    int* completion_time;
    int* turn_around_time;
    int* response_time;
    int* n_context;
    

};
typedef struct _pcb PCB;

struct _queue_member {
    queue_member *before;
    queue_member *after;
    PCB* pcb;
};
typedef struct _queue_member queue_member;



// some important global vars
queue_member* queue_start = NULL;
queue_member* queue_last = NULL;
#define queue  queue_start


// read the file specified in the arguments
queue_member* read_workload(const char* filepath, enum algorithm algo);

#endif
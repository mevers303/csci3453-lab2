#ifndef PCB_H
#define PCB_H

// system includes
#include <stdio.h>
#include <stdlib.h>

// project includes
#include "parameters.h"




/**********************************************
 *                                            *
 *               TYPES + VARS                 *
 *                                            *    
***********************************************/



// PCB type
struct _pcb {
    int pid;
    int arrival;
    int burst;
    int priority;
    int remaining;
};
typedef struct _pcb PCB;
struct _pcb test;


// QUEUE type
struct _queue_member {
    struct _queue_member *before;
    struct _queue_member *after;
    PCB* pcb;
    // these are stats determined during runtime
    float start_time;
    float waiting_time;
    float completion_time;
    float turn_around_time;
    float response_time;
    float n_context;
    float last_burst_start;
    float last_burst_end;
    float running_burst_time;
    float remaining_burst_time;
};
typedef struct _queue_member queue_member;

// create a blank queue_member we can use for mem_copy()
queue_member blank_queue_member = {
    .before = NULL,
    .after = NULL,
    .pcb = NULL,
    .start_time = -1,
    .waiting_time = -1,
    .completion_time = -1,
    .turn_around_time = -1,
    .response_time = -1,
    .n_context = -1,
    .last_burst_start = -1,
    .last_burst_end = -1,
    .running_burst_time = -1,
    .remaining_burst_time = -1
};


////////////////
//// QUEUES ////
////////////////
// ACTIVE queue
queue_member* queue_first = NULL;
queue_member* queue_last = NULL;
int queue_size = 0;
// INPUT queue
queue_member* input_queue_first = NULL;
queue_member* input_queue_last = NULL;
int input_queue_size = 0;
// COMPLETED queue
queue_member* completed_queue_first = NULL;
queue_member* completed_queue_last = NULL;
int completed_queue_size = 0;






/**********************************************
 *                                            *
 *                 FUNCTIONS                  *
 *                                            *    
***********************************************/

queue_member* load_input_file(const char* filepath);
void add_pcb_to_input_queue(PCB* new_pcb);
void receive_next_job();
void insert_pcb_into_queue(queue_member* to_be_inserted, queue_member* insert_before);
void calc_q_stats(queue_member* to_be_inserted, queue_member* insert_before);
void do_tick();
void context_switch();


#endif

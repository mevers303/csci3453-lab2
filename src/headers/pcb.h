// pcb.h
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2


#ifndef PCB_H
#define PCB_H



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
    int n_context;
    float last_burst_start;
    float last_burst_end;
    float running_time;
    float remaining_time;
};
typedef struct _queue_member queue_member;

// create a blank queue_member we can use for mem_copy()
extern queue_member blank_queue_member;


////////////////
//// QUEUES ////
////////////////
// ACTIVE queue
extern queue_member* current_process;
extern queue_member* queue_last;
extern int queue_size;
// INPUT queue
extern queue_member* input_queue_first;
extern queue_member* input_queue_last;
extern int input_queue_size;
// COMPLETED queue
extern queue_member* completed_queue_first;
extern queue_member* completed_queue_last;
extern int completed_queue_size;






/**********************************************
 *                                            *
 *                 FUNCTIONS                  *
 *                                            *    
***********************************************/

// loads the input queue
queue_member* load_input_file(const char* filepath);
// creates a pcb queue item and adds it to the queue
void add_pcb_to_input_queue(PCB* new_pcb);
// adds a pcb queue item to the active queue
void insert_pcb_into_queue(queue_member* to_be_inserted, queue_member* insert_after);
// simulates a new job arriving
void receive_next_job();
// frees memory
void cleanup();


#endif

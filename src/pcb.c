// pcb.c
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2

// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// project includes
#include "headers/parameters.h"
#include "headers/processes.h"
#include "headers/pcb.h"



/**********************************************
 *                                            *
 *               TYPES + VARS                 *
 *                                            *    
***********************************************/

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
    .running_time = -1,
    .remaining_time = -1
};


////////////////
//// QUEUES ////
////////////////
// ACTIVE queue
queue_member* current_process = NULL;
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


// loads the input queue
queue_member* load_input_file(const char* filepath) {
    
    // open file
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("  -> Could not open <%s>\n", filepath);
        exit(1);
    }


    int line = 0;
    int format_matches = 0;

    while (format_matches != EOF) {
        line++;

        // data from line
        int _pid = 0;
        int _arrival = 0;
        int _burst = 0;
        int _priority = 0;

        // read line
        int format_matches = fscanf(fp, "%i %i %i %i", &_pid, &_arrival, &_burst, &_priority);
        if (format_matches == EOF) {
            break;
        }
        if (format_matches != 4 || _pid < 0 || _arrival < 0 || _burst < 0 || _priority < 0) {
            printf("  -> Improper formatting on line %i (%i matches), skipping this line\n", line, format_matches);
            continue;
        }

        // we have a valid PCB, create struct instance
        PCB* new_pcb = malloc(sizeof(PCB));
        new_pcb->arrival = _arrival;
        new_pcb->burst = _burst;
        new_pcb->pid = _pid;
        new_pcb->priority = _priority;
        new_pcb->remaining = _burst;

        //now insert it into the queue
        add_pcb_to_input_queue(new_pcb);

    }

    //
    fclose(fp);
    return current_process;

}


// creates a pcb queue item and adds it to the queue
void add_pcb_to_input_queue(PCB* new_pcb) {

    // we need this PCB as a queue object
    queue_member* new_queued_pcb = malloc(sizeof(queue_member));
    memcpy(new_queued_pcb, (void*)&blank_queue_member, sizeof(queue_member));
    new_queued_pcb->pcb = new_pcb;

    // is it the very first queue item
    if (input_queue_size == 0) {
        input_queue_first = input_queue_last = new_queued_pcb;
        input_queue_size = 1;
        return;
    // proper arrival time
    } else if (new_queued_pcb->pcb->arrival >= input_queue_last->pcb->arrival) {
        new_queued_pcb->before = input_queue_last;
        input_queue_last->after = new_queued_pcb;
        input_queue_last = new_queued_pcb;
        input_queue_size++;
        return;
    // TODO: bad arrival time, search backwards and insert
    } else if (new_queued_pcb->pcb->arrival < input_queue_last->pcb->arrival) {
        printf("  -> Unhandled input error: arrival time is non-sequential.\n");
        exit(1);
    // this should never happen
    } else {
        printf("  -> Unhandled input error: unknown input condition: add_pcb_to_input_queue()\n");
        exit(1);
    }

}




// inserts a PCB queue item after a specified item in the active queue
void insert_pcb_into_queue(queue_member* to_be_inserted, queue_member* insert_after) {

    // debug print
    printf("  -> Inserting into queue...\n");

    // adding into the ready queue
    if (insert_after != NULL) {

        // debug print
        printf("  -> Inserting PID %i into ready queue...\n", to_be_inserted->pcb->pid);

        // next let's swap them into the queue
        to_be_inserted->before = insert_after;
        to_be_inserted->after = insert_after->after;
        // Old PCB takes new PCB as predecesssor
        insert_after->after = to_be_inserted;
        // increase counter
        queue_size++;

        // was the last queue item changed?
        if (insert_after == queue_last) {
            queue_last = to_be_inserted;
        }

    // we need to add as the current process or to the front of the ready queue
    } else {

        // the queue is empty
        if (!queue_size) {

            // debug print
            printf("  -> No running process, starting PID %i...\n", to_be_inserted->pcb->pid);

            current_process = queue_last = to_be_inserted;
            queue_size = 1;

        // the queue is not empty and it's round robin, we need to swap it in behind current_process
        } else if (algo == RR) {

            // debug print
            printf("  -> Detected running process, round robin operation...\n");

            // there are items in the ready queue, recurse this function and insert after current
            if (current_process->after != NULL) {
                // debug print
                printf("  -> Inserting PID %i into front of ready queue..\n", to_be_inserted->pcb->pid);
                insert_pcb_into_queue(to_be_inserted, current_process);
                return;
            // no items in ready queue behind current process
            } else {
                // debug print
                printf("  -> Ready queue empty, inserting PID %i..\n", to_be_inserted->pcb->pid);
                current_process->after = to_be_inserted;
                to_be_inserted->before = current_process;
                queue_last = to_be_inserted;
                queue_size++;
            }

        // the queue is not empty, we need to swap it in the front
        } else {

            // debug print
            printf("  -> Detected running process, performing context switch to PID %i..\n", to_be_inserted->pcb->pid);

            current_process->before = to_be_inserted;
            to_be_inserted->after = current_process;
            current_process = to_be_inserted;
            queue_size++;

        }

    }

    // debug print
    printf("  -> Insert successful\n");

}



// simulates a new job arriving
// pops input_queue, inserts to active_queue
void receive_next_job() {

    // pull queue item off input queue
    queue_member* new_queued_pcb = input_queue_first;
    // debug print
    printf("  -> Receiving PID %i from input queue...\n", input_queue_first->pcb->pid);
    if (new_queued_pcb == NULL) {
        printf("ERROR: receive_next_job() new_queued_pcb is NULL\n");
        exit(1);
    }
    input_queue_first = input_queue_first->after;
    if (input_queue_first != NULL) {
        input_queue_first->before = NULL;
    }
    input_queue_size--;


    // if active queue is empty, just set it and be done
    if (!queue_size) {
        // debug print
        printf("  -> No running process detected, inserting as current process\n");
        current_process = queue_last = new_queued_pcb;
        queue_size = 1;
        return;
    }


    // if FCFS just insert it at the end
    if (algo == FCFS) {
        // debug print
        printf("  -> Inserting at end of queue (FCFS)...\n");
        insert_pcb_into_queue(new_queued_pcb, queue_last);
        return;
    }


    // now for SRTF
    if (algo == SRTF) {

        // start at front
        queue_member* current_queue_item = current_process;
        
        // loop to compare run times and insert
        while (current_queue_item != NULL) {

            // we found a shorter remaining time, insert it into the linked list before current item
            if (new_queued_pcb->pcb->remaining < current_queue_item->pcb->remaining) {
                // debug print
                printf("  -> SRTF: Inserting into ready queue after PID %i...\n", current_queue_item->before->pcb->pid);
                insert_pcb_into_queue(new_queued_pcb, current_queue_item->before);
                return;
            }

            // loop to next one
            current_queue_item = current_queue_item->after;

        }
    
        // if this line is executing that means we made it all the way the to the end of the queue without finding a shorter job time, insert it at the end
        insert_pcb_into_queue(new_queued_pcb, queue_last);
        return;

    }



    // now for RR
    if (algo == RR) {

        // start at front
        queue_member* current_queue_item = current_process;
        
        // loop to compare priorities
        while (current_queue_item != NULL) {
            
            // we found a lower priority, insert it before the current list item
            if (new_queued_pcb->pcb->priority < current_queue_item->pcb->priority) {
                // debug print
                printf("  -> Round Robin: Inserting into ready queue after PID %i...\n", current_queue_item->before->pcb->pid);
                insert_pcb_into_queue(new_queued_pcb, current_queue_item->before);
                return;
            }

        }
    
        // if this line is executing that means we made it all the way the to the end of the queue without finding a smaller priority, insert it at the end
        insert_pcb_into_queue(new_queued_pcb, queue_last);
        return;

    }



    // this should never execute
    printf("  -> Unknown error: unknown state or no algorithm in receive_next_job()\n");
    exit(1);
    
}



// frees memory
void cleanup() {

    queue_member* this;

    // active queue (should be empty at end of main())
    this = current_process;
    while (this != NULL) {
        queue_member* next = this->after;
        free(this->pcb);
        free(this);
        this = next;
    }

    // input queue (should be empty at end of main())
    this = input_queue_first;    while (this != NULL) {
        queue_member* next = this->after;
        free(this->pcb);
        free(this);
        this = next;
    }

    // completed queue
    this = completed_queue_first;
    while (this != NULL) {
        queue_member* next = this->after;
        free(this->pcb);
        free(this);
        this = next;
    }

}

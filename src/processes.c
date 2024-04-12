// processes.c
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2



// standard includes
#include <stdio.h>

// project includes
#include "headers/processes.h"
#include "headers/parameters.h"
#include "headers/pcb.h"





/**********************************************
 *                                            *
 *                 FUNCTIONS                  *
 *                                            *
***********************************************/


// switches between processes, performs context switch if necessary
// runs BEFORE current_time is incremented at end of each tick
void switch_process() {

    //////////////////////
    // end running item //
    //////////////////////

    // update metadata
    queue_first->last_burst_end = current_time + 1;
    queue_first->running_time += queue_first->last_burst_end - queue_first->last_burst_start;
    // save the next item in queue, , we will be modifying queue_first before we need it later
    queue_member* next_queue_item = queue_first->after;
    // boolean if the current item has finished running, we will be modifying queue_first before we need it later
    int running_item_finished = queue_first->remaining_time <= 0;

    // if the running item is now finished, finalize the stats and remove from queue
    if (running_item_finished) {

        // save some stats
        queue_first->completion_time = current_time + 1;
        queue_first->turn_around_time = queue_first->completion_time - queue_first->pcb->arrival;
        queue_first->response_time = queue_first->start_time - queue_first->pcb->arrival;

        // add into completed queue
        queue_first->before = completed_queue_last;
        queue_first->after = NULL;
        if (completed_queue_first == NULL) {
            completed_queue_first = completed_queue_last = queue_first;
            completed_queue_size = 1;
        } else {
            completed_queue_last->after = queue_first;
            completed_queue_last = queue_first;
            completed_queue_size++;
        }

        // pop from active queue
        queue_first = next_queue_item;
        queue_size--;

    }


    ///////////////////
    // overhead time //
    ///////////////////

    // is there still more than one item in the queue? move to the back of queue and switch to the next
    if (!running_item_finished && queue_size > 1) {
        queue_first = next_queue_item;
    // one item left or zero items in queue? no context switch (this case happens in round robin)
    } else if (queue_size == 0 || queue_size == 1) {
        // round robin quantum management
        if (algo == RR) {
            last_quantum_start = current_time;
        }
        return;
    // less than zero items in queue?  problem
    } else {
        printf("Unknown error: unknown state (negative queue size)");
        exit(1);
    }

    // add overhead time if it's not the very first tick and this isn't running because the last process finished
    if (current_time > 0 && running_item_finished) {
        current_time += CONTEXT_SWITCH_COST;
    }

    // round robin quantum management
    if (algo == RR) {
        last_quantum_start = current_time;
    }


    /////////////////////
    // start next item //
    /////////////////////
    // if it's the first time running, initialize some metadata
    if (queue_first->start_time == -1) {
        queue_first->start_time = current_time;
        queue_first->response_time = current_time - queue_first->pcb->arrival;
        queue_first->waiting_time = queue_first->response_time;  // same value for now
        queue_first->n_context = 1;
        queue_first->last_burst_start = current_time;
        queue_first->running_time = 0;
        queue_first->remaining_time = queue_first->pcb->remaining;
    // it's a process that has run before
    } else {
        queue_first->waiting_time += current_time - queue_first->last_burst_end;
        queue_first->n_context++;
    }
        

}


// performs one processor burst
void do_tick() {

    // sanity check, empty queue
    if (queue_first == NULL) {
        return;
    }

    // update current item
    queue_first->running_time++;
    queue_first->remaining_time--;

}

// processes.c
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2



// standard includes
#include <stdio.h>
#include <stdlib.h>

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
void end_current_process() {

    // debug print
    printf("Finished process PID %i\n", current_process->pcb->pid);

    //////////////////////
    // end running item //
    //////////////////////

    // we need to save this for later
    queue_member* next_queue_item = current_process->after;

    // save some stats
    current_process->last_burst_end = current_time;
    current_process->running_time += current_process->last_burst_end - current_process->last_burst_start;
    current_process->completion_time = current_time;
    current_process->turn_around_time = current_process->completion_time - current_process->pcb->arrival;
    current_process->response_time = current_process->start_time - current_process->pcb->arrival;

    // add into completed queue in order of pid

    // it is not the first one
    if (completed_queue_size) {

        queue_member* this = completed_queue_first;
        while (this != NULL) {

            // loop to find smaller PID
            if (current_process->pcb->pid < this->pcb->pid) {

                // debug print
                printf("  -> Sorting completed_queue by inserting PID %i before PID %i...\n", current_process->pcb->pid, this->pcb->pid);
                current_process->before = this->before;
                current_process->after = this;
                if (this->before) {
                        this->before->after = current_process;
                }
                this->before = current_process;

                // did we change first or last?
                // is it the first item?
                if (current_process->before == NULL) {
                    completed_queue_first = current_process;
                } 
                // is it the last item?
                if (current_process->after == NULL) {
                    completed_queue_last = current_process;
                }
                break;

            }

            // this shouldn't be happening
            if (this == this->after) {
                printf("ERROR switch_process(): Loop in completed_queue\n");
                    break;
            }
            // progress to next in search
            this = this->after;

        }

        // if 'this' is null, that means we didn't find a smaller PID. add to end of completed_queue
        if (this == NULL) {
            // debug print
            printf("  -> Adding PID %i to end of completed_queue...\n", current_process->pcb->pid);
            completed_queue_last->after = current_process;
            current_process->before = completed_queue_last;
            current_process->after = NULL;
            completed_queue_last = current_process;
        }

        completed_queue_size++;

    // it is the first one in the whole queue
    } else {

        // debug print
        printf("  -> Adding PID %i to completed_queue as the first in an empty queue...\n", current_process->pcb->pid);
        completed_queue_first = completed_queue_last = current_process;
        current_process->before = current_process->after = NULL;
        completed_queue_size = 1;

    }

    // debug print
    printf("  -> Number of completed processes: %i\n", completed_queue_size);

    // pop from active queue
    if (queue_size > 1) {
        current_process = next_queue_item;
        current_process->before = NULL;
        queue_size--;
    // the last running process
    } else if (queue_size == 1) {
        current_process = NULL;
        queue_size--;
    } else if (queue_size <= 0) {
        printf("ERROR switch_process(): decrease queue size when less than one.  Size: %i\n", queue_size);
    }

}


/////////////////////////
// switch current item //
/////////////////////////
// switches between processes, performs context switch if necessary
void rr_switch_process() {

    // sanity check
    if (!queue_size) {
        printf("No more queue items to switch to.\n");
        return;
    }


    // round robin: is there still more than one item in the queue? move to the back of queue and switch to the next
    if (queue_size > 1) {

        // debug print
        printf("Switching process (round robin)\n");
        printf("  -> Moving current process to the back of the ready queue (round robin)\n");

        // we need to save this for later
        queue_member* next_queue_item = current_process->after;
        // swap current process to the back
        current_process->before = queue_last;
        current_process->after = NULL;
        queue_last->after = current_process;
        queue_last = current_process;
        current_process = next_queue_item;
        current_process->before = NULL;

    // we're switching because of a new arrival and ready queue has only the current process
    } else if (queue_size == 1) {

        // debug print
        printf("  -> Process switch requested, but the ready queue is empty (%i)\n", queue_size);

        // round robin quantum management
        if (algo == RR) {
            last_quantum_start = current_time;
        }

        // do nothing

    // queue is empty and current process is finished.
    } else if (queue_size == 0) {

        printf("No more processes!\n");
        return;

    // less than zero items in queue?  problem
    } else if (queue_size < 0) {

        printf("  -> Unknown error: unknown state in 'switch_process()' (negative queue size)\n");
        exit(1);

    }

    // round robin quantum management
    last_quantum_start = current_time;

}




/////////////////////
// start next item //
/////////////////////
void start_current_process() {

    // if it's the first time running, initialize some metadata
    if (current_process->start_time == -1) {
        
        // debug print
        printf("Starting PID %i for the first time.  Time remaining: %i\n", current_process->pcb->pid, current_process->pcb->remaining);

        // initialize variables
        current_process->start_time = current_time;
        current_process->response_time = current_time - current_process->pcb->arrival;
        current_process->waiting_time = current_process->response_time;  // same value for now
        current_process->n_context = 0;
        current_process->last_burst_start = current_time;
        current_process->last_burst_end = -1;
        current_process->running_time = 0;
        current_process->remaining_time = current_process->pcb->remaining;

    // it's a process that has run before
    } else {

        // debug print
        printf("  -> Starting PID %i.  Time remaining: %.1f\n", current_process->pcb->pid, current_process->remaining_time);

        // initialize burst variables
        current_process->waiting_time += current_time - current_process->last_burst_end;
        current_process->last_burst_start = current_time;
        current_process->last_burst_end = -1;
    }      

}




// performs one processor burst
void do_tick() {

    // sanity check, empty queue
    if (!queue_size) {
        return;
    }

    // debug print
    printf("  -> burst\n");

    // update current item
    if (!current_process) {
        printf("ERROR do_tick(): current_process is NULL\n");
        return;
    }
    current_process->running_time++;
    current_process->remaining_time--;

}



// perform the output
void do_output() {

    // get the algorithm string
    char* algo_s;
    switch (algo) {
        case FCFS:
            algo_s = "FCFS";
            break;
        case SRTF:
            algo_s = "SRTF";
            break;
        case RR:
            algo_s = "RR  ";
            break;
        
        default:
            break;
    }

    // vars
    float total_run_time = 0;
    float total_wait_time = 0;
    float total_turn_around_time = 0;
    float total_response_time = 0;
    int total_n_context = 0;

    // print the title
    printf("********************************************************************************\n");
    printf("********************  Algorithm: %s                      *********************\n", algo_s);
    if (algo == RR) {
        printf("********************  Tasks: %i Quantum: %.1f                *********************\n", completed_queue_size, quantum_size);
    }
    printf("********************************************************************************\n");

    // print the table

    // print table header
    printf("pid,arrival,run_time,finish,wait_time,turn_around_time,response_time,n_context\n");

    // loop through complete items
    queue_member* this = completed_queue_first;
    while (this != NULL) {

        // print this row
        printf("%i,%i,%.1f,%.1f,%.1f,%.1f,%.1f,%i\n", this->pcb->pid,
                                          this->pcb->arrival,
                                          this->running_time,
                                          this->completion_time,
                                          this->waiting_time,
                                          this->turn_around_time,
                                          this->response_time,
                                          this->n_context);

        // add to totals
        total_run_time += this->running_time;
        total_wait_time += this->waiting_time;
        total_turn_around_time += this->turn_around_time;
        total_response_time += this->response_time;

        // progress to next item
        if (this == this->after) {
            printf("ERROR switch_process(): Loop in completed_queue\n");
                break;
        }
        this = this->after;

    }
    
    // print averages
    printf("Average run_time: %.1f\n", total_run_time / completed_queue_size);
    printf("Average wait_time: %.1f\n", total_wait_time / completed_queue_size);
    printf("Average turn_around: %.1f\n", total_turn_around_time / completed_queue_size);
    printf("Average total_response_time: %.1f\n", total_response_time / completed_queue_size);
    printf("Total n_context: %i\n", (total_n_context));

}

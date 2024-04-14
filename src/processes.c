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
// runs BEFORE current_time is incremented at end of each tick
void switch_process() {

    // debug print
    printf("  -> switch_process() requested...\n");

    //////////////////////
    // end running item //
    //////////////////////

    // update metadata
    current_process->last_burst_end = current_time + 1;
    current_process->running_time += current_process->last_burst_end - current_process->last_burst_start;
    // we need to save this for later
    queue_member* next_queue_item = current_process->after;
    // boolean if the current item has finished running, we will be modifying current_process before we need it later
    int running_item_finished = current_process->remaining_time <= 0 && current_process->start_time >= 0;

    // if the running item is now finished, finalize the stats and remove from queue
    if (running_item_finished) {

        // debug print
        printf("  -> Finished PID: %i\n", current_process->pcb->pid);

        // save some stats
        current_process->completion_time = current_time + 1;
        current_process->turn_around_time = current_process->completion_time - current_process->pcb->arrival;
        current_process->response_time = current_process->start_time - current_process->pcb->arrival;

        // add into completed queue in order of pid
        // debug print
        printf("  -> Adding to list of completed processes...\n");

        // it is not the first one
        if (completed_queue_size) {

            queue_member* this = completed_queue_first;
            while (this != NULL) {
                if (current_process->pcb->pid < this->pcb->pid) {
                    // debug print
                    printf("  -> Sorting completed queue and inserting...\n");
                    current_process->before = this->before;
                    current_process->after = this;
                    this->before = current_process;
                    // is it the first item?
                    if (current_process->before == NULL) {
                        completed_queue_first = current_process;
                    }
                    // is it the last item?
                    if (current_process->before == NULL) {
                        completed_queue_last = current_process;
                    }
                    break;
                }
            }

            // add to end of completed_queue
            if (this == NULL) {
                // debug print
                printf("  -> Adding to end of completed queue...\n");
                completed_queue_last->after = current_process;
                current_process->before = completed_queue_last;
                current_process->after = NULL;
                completed_queue_last = current_process;
            }

            completed_queue_size++;

        // it is the first one
        } else {

            current_process->before = current_process->after = NULL;
            completed_queue_first = completed_queue_last = current_process;
            completed_queue_size = 1;

        }

        // debug print
        printf("  -> Number of completed processes: %i\n", completed_queue_size);

        // pop from active queue
        if (queue_size > 1) {
            current_process = next_queue_item;
            current_process->before = NULL;
        }
        if (queue_size > 0) {
            queue_size--;
        } else {
            printf("  -> ERROR: decrease queue size when less than one.  Size: %i\n", queue_size);
        }

    }


    /////////////////////////
    // switch current item //
    /////////////////////////

    // debug print
    printf("Switching...\n");

    // round robin: is there still more than one item in the queue? move to the back of queue and switch to the next
    if (!running_item_finished && queue_size > 1 && current_process->start_time >= 0) {

        if (algo == RR) {
            
            // debug print
            printf("  -> Moving current process to the back of the ready queue (round robin)\n");

            current_process->before = queue_last;
            current_process->after = NULL;
            queue_last->after = current_process;
            queue_last = current_process;
            current_process = next_queue_item;
            current_process->before = NULL;

        }

        // if (algo != RR) {
            // we don't need to do anything besides record a context switch if it's not RR, this was done in insert_pcb_into_queue()
        // }

        // this entails a context switch
        if (current_time > 0) {
            // debug print
            printf("Context switch!  Adding overhead time...\n");
            current_process->n_context++;
            current_time += CONTEXT_SWITCH_COST;
        }

    // one item left or zero items in queue? no context switch
    } else if ((queue_size == 0 || queue_size == 1) && current_process->start_time >= 0) {

        // debug print
        printf("  -> Process switch requested, but the ready queue is empty (%i)\n", queue_size);

        // round robin quantum management
        if (algo == RR) {
            last_quantum_start = current_time;
        }
        return;

    // less than zero items in queue?  problem
    } else if (queue_size < 0) {

        printf("  -> Unknown error: unknown state in 'switch_process()' (negative queue size)\n");
        exit(1);

    }

    // round robin quantum management
    if (algo == RR) {
        last_quantum_start = current_time;
    }


    /////////////////////
    // start next item //
    /////////////////////

    // debug print
    printf("Starting...\n");

    // if it's the first time running, initialize some metadata
    if (current_process->start_time == -1) {
        // debug print
        printf("%.1f: Starting PID %i for the first time\n", current_time, current_process->pcb->pid);
        current_process->start_time = current_time;
        current_process->response_time = current_time - current_process->pcb->arrival;
        current_process->waiting_time = current_process->response_time;  // same value for now
        current_process->n_context = 0;
        current_process->last_burst_start = current_time;
        current_process->running_time = 0;
        current_process->remaining_time = current_process->pcb->remaining;
    // it's a process that has run before
    } else {
        current_process->waiting_time += current_time - current_process->last_burst_end;
        // debug print
        printf("  -> Starting PID %i.  new n_context = %i\n", current_process->pcb->pid, current_process->n_context);
    }      

}




// performs one processor burst
void do_tick() {

    // sanity check, empty queue
    if (!queue_size) {
        return;
    }

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
    float total_run_time;
    float total_wait_time;
    float total_turn_around_time;
    float total_response_time;
    int total_n_context;

    // print the title
    printf("********************************************************************************\n");
    printf("********************  Algorithm: %s                      *********************\n", algo_s);
    if (algo == RR) {
        printf("********************  Tasks: %i Quantum: %f                *********************\n", completed_queue_size, quantum_size);
    }
    printf("********************************************************************************\n");

    // print the table

    // print table header
    printf("pid,arrival,run_time,finish,wait_time,turn_around_time,response_time,n_context\n");

    // loop through complete items
    queue_member* this = completed_queue_first;
    while (this != NULL) {
        printf("%i,%i,%f,%f,%f,%f,%f,%i\n", this->pcb->pid,
                                          this->pcb->arrival,
                                          this->running_time,
                                          this->completion_time,
                                          this->waiting_time,
                                          this->turn_around_time,
                                          this->response_time,
                                          this->n_context);
        this = this->after;
    }
    
    // print averages
    printf("Average run_time: %i\n", (int)(total_run_time / completed_queue_size));
    printf("Average wait_time: %i\n", (int)(total_run_time / total_wait_time));
    printf("Average turn_around: %i\n", (int)(total_run_time / total_turn_around_time));
    printf("Average total_response_time: %i\n", (int)(total_run_time / completed_queue_size));
    printf("Total n_context: %i\n", (total_n_context));

}

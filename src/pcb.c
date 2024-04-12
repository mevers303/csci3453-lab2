// main.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "headers/pcb.h"
#include "headers/parameters.h"


queue_member* load_input_file(const char* filepath) {
    
    // open file
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("Could not open <%s>", filepath);
        exit(1);
    }


    int line = 0;
    int format_matches = 0;

    while (format_matches != EOF) {
        line += 1;

        // data from line
        int _pid = 0;
        int _arrival = 0;
        int _burst = 0;
        int _priority = 0;

        // read line
        int format_matches = fscanf(fp, "%i[ ]*%i[ ]*%i[ ]*%i[ ]*\n", &_pid, &_arrival, &_burst, &_priority);
        if (format_matches == EOF) {
            break;
        }
        if (format_matches != 4 || _pid < 0 || _arrival < 0 || _burst < 0 || _priority < 0) {
            printf("Improper formatting on line %i, skipping this line", line);
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
    return queue_first;

}


void add_pcb_to_input_queue(PCB* new_pcb) {

    // we need this PCB as a queue object
    queue_member* new_queued_pcb = malloc(sizeof(queue_member));
    memcpy(new_queued_pcb, (void*)&blank_queue_member, sizeof(queue_member));
    new_queued_pcb->pcb = new_pcb;

    // is it the very first queue item
    if (input_queue_first == NULL) {
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
        printf("Unhandled input error: arrival time is non-sequential.");
        exit(1);
    // this should never happen
    } else {
        printf("Unhandled input error: unknown input condition: add_pcb_to_input_queue()");
        exit(1);
    }

}




// simulates a new job arriving
// pops input_queue, inserts to active_queue
void receive_next_job() {

    // pull queue item off input queue
    queue_member* new_queued_pcb = input_queue_first;
    if (new_queued_pcb == NULL) {
        return;
    }
    input_queue_first = input_queue_first->after;
    input_queue_first->before = NULL;
    input_queue_size--;


    // if active queue is empty, just set it and be done
    if (queue_first == NULL) {
        queue_first = queue_last = new_queued_pcb;
        queue_size = 1;
        return;
    }


    // if FCFS just insert it at the end
    if (algo == FCFS) {
        insert_pcb_into_queue(new_queued_pcb, queue_last);
        return;
    }


    // now for SRTF
    if (algo == SRTF) {

        // start at front
        queue_member* current_queue_item = queue_first;
        
        // loop to compare run times and insert
        while (current_queue_item != NULL) {

            // we found a shorter remaining time, insert it into the linked list before current item
            if (new_queued_pcb->pcb->remaining < current_queue_item->pcb->remaining) {
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
        queue_member* current_queue_item = queue_first;
        
        // loop to compare priorities
        while (current_queue_item != NULL) {
            
            // we found a lower priority, insert it before the current list item
            if (new_queued_pcb->pcb->priority < current_queue_item->pcb->priority) {
                insert_pcb_into_queue(new_queued_pcb, current_queue_item->before);
                return;
            }

        }
    
        // if this line is executing that means we made it all the way the to the end of the queue without finding a smaller priority, insert it at the end
        insert_pcb_into_queue(new_queued_pcb, queue_last);
        return;

    }



    // this should never execute
    printf("Unknown error: unknown state or no algorithm in receive_next_job()");
    exit(1);
    
}




// inserts a PCB queue item after a specified item in the active queue
void insert_pcb_into_queue(queue_member* to_be_inserted, queue_member* insert_after) {

    // if its not the first in the queue
    if (insert_after != NULL) {

        // next let's swap them into the queue
        to_be_inserted->before = insert_after;
        to_be_inserted->after = insert_after->after;
        // Old PCB takes new PCB as predecesssor
        insert_after->after = to_be_inserted;
        // increase counter
        queue_size++;

    // we need to add to front of queue
    } else {

        // the queue is empty
        if (queue_first == NULL) {
            queue_first = queue_last = to_be_inserted;
            queue_size = 1;
        // the queue is not empty, we need to swap it in
        } else {
            queue_first->before = to_be_inserted;
            to_be_inserted->after = queue_first;
            queue_first = to_be_inserted;
            queue_size++;
        }

    }

}



void context_switch() {

    //////////////////////
    // end running item //
    //////////////////////
    queue_first->last_burst_end = current_time + 1;
    queue_first->running_burst_time += queue_first->last_burst_end - queue_first->last_burst_start;
    queue_member* next_queue_item = queue_first->after;
    int running_item_finished = queue_first->remaining_burst_time == 0;

    // if the running item is now finished
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

    // is there still more than one item in the queue?  switch to the next
    if (!running_item_finished && queue_size > 1) {
        queue_first = next_queue_item;
    // one item left or zero items in queue? no context switch
    } else if (queue_size == 0 || queue_size == 1) {
        return;
    // less than zero items in queue?  problem
    } else {
        printf("Unknown error: unknown state (negative queue size)");
        exit(1);
    }

    // add overhead time if it's not the very first tick
    if (current_time > 0) {
        current_time += CONTEXT_SWITCH_COST;
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
        queue_first->running_burst_time = 0;
        queue_first->remaining_burst_time = queue_first->pcb->remaining;
    // it's a process that has run before
    } else {
        queue_first->waiting_time += current_time - queue_first->last_burst_end;
        queue_first->n_context++;
    }
        
        
}



void do_tick() {

    // update current item
    queue_first->running_burst_time++;
    queue_first->remaining_burst_time--;

    // is it finiished?
    if (queue_first->remaining_burst_time <= 0) {
        context_switch();
    }

}

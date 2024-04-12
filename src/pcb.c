// pcb.c
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2

// standard includes
#include <stdio.h>

// project includes
#include "headers/pcb.h"
#include "headers/parameters.h"





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


// creates a pcb queue item and adds it to the queue
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

        // also this entails a context switch if it is already running
        if (current_time > 0) {
            switch_process();
        }

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

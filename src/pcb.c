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
        }

    }

}




void get_aggregate_stats(queue_member* to_be_inserted, queue_member* insert_before) {

    // calculate new PCB wait time
    float this_wait_time = current_time - to_be_inserted->last_burst_end;
    to_be_inserted->waiting_time += this_wait_time;
    
    // if it's not the very first in the queue
    if (insert_before != NULL) {
        // find the old PCB burst time
        insert_before->last_burst_end = current_time;
        float this_burst = insert_before->last_burst_end - insert_before->last_burst_start;
        insert_before->running_burst_time += this_burst;
        insert_before->pcb->remaining -= this_burst;
    }

}


void do_tick() {
    queue_member* one = queue_first;
    queue_member* two = one->after;

    // one stats
    one->last_burst_end = current_time;
    one->turn_around_time = one->completion_time - one->pcb->arrival;
    one->waiting_time = one->turn_around_time - one->running_burst_time;
    one->response_time = one->start_time - one->pcb->arrival;
    one->running_burst_time += one->last_burst_end - one->last_burst_start;
    one->remaining_burst_time = one->running_burst_time - one->pcb->burst;

    // is it finiished?
    if (one->remaining_burst_time <= 0) {
        // send off the first queue item to completed_queue
        one->completion_time = current_time;
        // add to the completed queue
        completed_queue_last->after = one;
        one->before = completed_queue_last;
        completed_queue_last = one;
        completed_queue_last->after = NULL;

        // set up the second queue item
        two->before = NULL;
        queue_first = two;
        // add context time switch
        current_time += .5;
        queue_size -= 1;
    }

    //two stats
    two->n_context += 1;
    if (!two->start_time) {
        two->start_time = current_time;
    }
    two->waiting_time += current_time - (two->last_burst_end || two->pcb->arrival);
    two->last_burst_start = current_time;

}

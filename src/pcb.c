// main.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "headers/pcb.h"
#include "headers/parameters.h"


queue_member* load_input_file(const char* filepath, enum algorithm algo) {
    
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
        add_pcb_to_queue(new_pcb, algo);

    }

    //
    fclose(fp);
    return queue_first;

}


void add_pcb_to_queue(PCB* new_pcb, enum algorithm algo) {

    // we need this PCB as a queue
    queue_member* new_queued_PCB = malloc(sizeof(queue_member));
    memcpy(new_queued_PCB, (void*)&blank_queue_member, sizeof(queue_member));
    new_queued_PCB->pcb = new_pcb;
    queue_size += 1;

    // is it the vert first queue item? easy
    if (queue_first == NULL) {
        queue_first = new_queued_PCB;
        return;
    }

    // easy one first, if FCFS just instert it at the end
    if (algo == FCFS) {
        // new queue item get new predecessor
        new_queued_PCB->before = queue_last;
        // last queue item gets new follower
        queue_last->after = new_queued_PCB;
        // new_queued_PCB->after remains NULL, signifying the end
        // save the last queue item for easy access next time
        queue_last = new_queued_PCB;
        return;
    }


    // now for SRTF
    if (algo == SRTF) {

        // start at front and compare remaining run times
        queue_member* old_queued_PCB = queue_first;
        
        while (old_queued_PCB->after != NULL) {
            // we found a shorter remaining time, insert it into the linked list
            if (new_queued_PCB->pcb->remaining < old_queued_PCB->pcb->remaining) {

                // calculate new PCCB wait time
                float this_wait_time = current_time - new_queued_PCB->last_burst_end;
                new_queued_PCB->waiting_time += this_wait_time;
                
                // calculate the burst effect
                old_queued_PCB->last_burst_end = current_time;
                float this_burst = old_queued_PCB->last_burst_end - old_queued_PCB->last_burst_start;
                old_queued_PCB->running_burst_time += this_burst;
                old_queued_PCB->pcb->remaining -= this_burst;

                // is it finished? remove it from the queue
                if (old_queued_PCB->pcb->remaining <= 0) {
                    if (old_queued_PCB->before != NULL) {
                        old_queued_PCB->before->after = old_queued_PCB->after;
                    }
                    if (old_queued_PCB->after != NULL) {
                        old_queued_PCB->after->before = old_queued_PCB->before;
                    }
                }

                // next let's swap them in the queue
                // new PCB takes old PCB's predecessor and makes it its follower
                new_queued_PCB->before = old_queued_PCB->before;
                new_queued_PCB->after = old_queued_PCB;
                // Old PCB takes new PCB as predecesssor
                old_queued_PCB->before = new_queued_PCB;
                // old PCB keeps the same follower, and we exit function

            }
        }
    
        // if this line is executing that means we made it all the way the to the end of the queue without finding a shorter job time, insert it at the end
        old_queued_PCB->after = new_queued_PCB;
        new_queued_PCB->before = old_queued_PCB;

        // done!
        return;

    }


    // now for RR
    if (algo == RR) {

        // start at front and compare remaining run times
        queue_member* current_queued_PCB = queue_first;
        
        while (current_queued_PCB->after != NULL) {
            
            // we found a shorter remaining time, insert it into the linked list
            if (new_queued_PCB->pcb->priority < current_queued_PCB->pcb->priority) {
                // new PCB takes old PCB's predecessor
                new_queued_PCB->before = current_queued_PCB->before;
                // Old PCB takes new PCB as predecesssor
                current_queued_PCB->before = new_queued_PCB;
                // new PCB takes old PCB as followor
                new_queued_PCB->after = current_queued_PCB;
                // old PCB keeps the same follower, and we exit function
                return;
            }
        }
    
        // if this line is executing that means we made it all the way the to the end of the queue without finding a shorter job time, insert it at the end
        current_queued_PCB->after = new_queued_PCB;
        new_queued_PCB->before = current_queued_PCB;
        queue_last = new_pcb;

        // done!
        return;

    }
    
}



void insert_pcb_into_queue(queue_member* to_be_inserted, queue_member* before, queue_member* after){

                // calculate new PCCB wait time
                float this_wait_time = current_time - to_be_inserted->last_burst_end;
                to_be_inserted->waiting_time += this_wait_time;

}

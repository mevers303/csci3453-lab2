// main.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "pcb.h"
#include "parameters.h"



queue_member* read_workload(const char filepath, enum algorithm algo) {
    
    // open file
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("Could not open <%s>", filepath);
        exit(1);
    }


    int line = 1;
    int format_matches = 0;

    while (format_matches != EOF) {

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
            printf("Improper formatting on line %i", line);
            fclose(fp);
            exit(1);
        }

        // we have a valid PCB, create struct instance
        PCB* new_pcb = malloc(sizeof(PCB));
        new_pcb->arrival = _arrival;
        new_pcb->burst = _burst;
        new_pcb->pid = _pid;
        new_pcb->priority = _priority;
        new_pcb->remaining = _burst;

        // first one in the list?
        if (queue_start == NULL) {
            queue_start = new_pcb;
        } else {
            insert_pcb_to_queue(new_pcb, algo);
        }

        line += 1;

    }

    fclose(fp);

}


void insert_pcb_to_queue(PCB* new_pcb, enum algorithm algo) {

    // we need this PCB as a queue
    queue_member* new_queued_PCB = malloc(sizeof(queue_member));
    new_queued_PCB->pcb = new_pcb;

    // is it the vert first queue item? easy
    if (queue_start == NULL) {
        queue_start = new_queued_PCB;
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
        queue_member* current_queued_PCB = queue_start;
        
        while (current_queued_PCB->after != NULL) {
            // we found a shorter remaining time, insert it into the linked list
            if (new_queued_PCB->pcb->remaining < current_queued_PCB->pcb->remaining) {
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

        // done!
        return;

    }


    // now for RR
    if (algo == RR) {

        // allocate new PCB
        queue_member* new_queued_PCB = malloc(sizeof(queue_member));
        new_queued_PCB->pcb = new_pcb;
        new_queued_PCB->before = NULL;
        new_queued_PCB->after = NULL;
        // start at front and compare remaining run times
        queue_member* current_queued_PCB = queue_start;
        
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

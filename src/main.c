// main.c
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2

// standard includes
#include <stdio.h>
#include <stdlib.h>

// project includes
#include "headers/parameters.h"
#include "headers/pcb.h"
#include "headers/processes.h"





int main(int argc, char* argv[]) {


    /////////////////////////////
    ///////   ARGUMENTS /////////
    /////////////////////////////
 
    // read the arguments
    if (argc != 3 && argc != 4) {
        printf("Improper arguments");
    }
    const char* input_file = argv[1];

    // sanity check algorithm enum
    int algo_i = atoi(argv[2]);
    if (algo_i < 0 ||  algo_i > 2) {
        printf("Algorithm selection must be between [0, 2] in the 2nd argument.");
        exit(1);
    }
    // we're good, save algorithm enum
    algo = algo_i;

    // is a quantum provided?
    if (argc == 4) {
        quantum_size = atof(argv[3]);
    }

    // sanity check for RR and quantum size
    if (algo == RR && quantum_size <= 0) {
        printf("Since the algorithm is Round Robin (2), you must provide a proper quantum size in the 3rd argument greater than 0.");
        exit(1);
    }


    /////////////////////////////
    ///////   MAIN LOOP /////////
    /////////////////////////////

    // load the input queue
    load_input_file(input_file);
    // initialize vars
    current_time = 0;
    if (algo = RR) {
        last_quantum_start = 0;
    }
    queue_member* this_queue_item = current_process;


    // loop while we still have queue items
    while (queue_size > 0 || input_queue_size > 0) {

        // first, manage the queues

        // check if a new process has arrived
        if (input_queue_size > 0 && input_queue_first->pcb->arrival <= current_time) {
            queue_member* old_current_process = current_process;
            // loop in case of multiple arrivals
            while (input_queue_first->pcb->arrival <= current_time) {
                receive_next_job();
            }
            // was something added to the front of the queue? do a context switch
            if (old_current_process != current_process && algo != RR) {
                switch_process();
            }
        }
        

        // second, check if context switch is needed for round robin
        if (algo == RR && current_time - last_quantum_start >= quantum_size) {
            switch_process();
        }
        

        // third, perform a burst cycle
        do_tick();

        
        // fourth, check if process just completed running, since switch_process() needs to be called before the time is incremented
        if (current_process->remaining_time <= 0) {
            // context switch checks for finished process and handles it
            switch_process();
        }


        // finally, increment the time for the next cycle
        current_time++;
    }


    // output
    do_output();


    // fin
    cleanup();
    return 0;

}

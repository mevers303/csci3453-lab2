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



/**********************************************
 *                                            *
 *               TYPES + VARS                 *
 *                                            *    
***********************************************/

// defined by assignment
const float CONTEXT_SWITCH_COST = 0.5;


// GLOBAL VARS
float current_time = -1;
float quantum_size = -1;
enum algorithm algo = FCFS;
float last_quantum_start = -1;






/**********************************************
 *                                            *
 *                 FUNCTIONS                  *
 *                                            *
***********************************************/


int main(int argc, char* argv[]) {


    /////////////////////////////
    ///////   ARGUMENTS /////////
    /////////////////////////////
 
    // read the arguments
    if (argc != 3 && argc != 4) {
        printf("  -> Improper arguments\n");
    }
    const char* input_file = argv[1];

    // sanity check algorithm enum
    int algo_i = atoi(argv[2]);
    if (algo_i < 0 ||  algo_i > 2) {
        printf("  -> Algorithm selection must be between [0, 2] in the 2nd argument.\n");
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
        printf("  -> Since the algorithm is Round Robin (2), you must provide a proper quantum size in the 3rd argument greater than 0.\n");
        exit(1);
    }

    // debug print
    printf("Input file: %s\nAlgorithm = %i\nQuantum size: %.1f\n\n", input_file, algo, quantum_size);


    /////////////////////////////
    ///////   MAIN LOOP /////////
    /////////////////////////////

    // load the input queue
    printf("Reading input file...\n");
    load_input_file(input_file);
    // initialize vars
    current_time = 0;
    if (algo == RR) {
        last_quantum_start = 0;
    }
    queue_member* this_queue_item = current_process;


    // loop while we still have queue items
    printf("Starting main loop...\n");
    while (queue_size > 0 || input_queue_size > 0) {

        // debug output
        printf("\n%.1f: queue_size=%i input_queue_size=%i\n", current_time, queue_size, input_queue_size);

        
        // first, check if process just completed running, since switch_process() needs to be called before the time is incremented
        int current_item_completed = current_process && current_process->remaining_time <= 0;
        queue_member* old_current_process = current_process;


        // check if a new process has arrived
        if (input_queue_size > 0 && input_queue_first->pcb->arrival <= current_time) {
            if (!input_queue_first) {
                printf("ERROR: main() input_queue_first is NULL");
            }
            // loop in case of multiple arrivals
            while (input_queue_first && input_queue_first->pcb->arrival <= current_time) {
                receive_next_job();
            }
        }


        //  perform a context switch if the last item was finished or something was added to the front of the queue
        if (current_item_completed || (old_current_process != current_process && algo != RR)) {
            printf("switch_process() because of finished process or newly arrived process\n");
            switch_process();
        } else if (algo == RR && current_time - last_quantum_start >= quantum_size) {
            printf("switch_process() because quantum time has ellapsed\n");
            switch_process();
        }
        

        // perform a burst cycle
        do_tick();


        // finally, increment the time for the next cycle
        current_time++;

    }


    // output
    do_output();


    // fin
    cleanup();
    return 0;

}

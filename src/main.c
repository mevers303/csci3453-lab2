#include "headers/parameters.h"
#include "headers/pcb.h"





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
    queue_member* this_queue_item = queue_first;


    // loop while we still have queue items
    while (! (queue_first == NULL && input_queue_first == NULL)) {

        // first, manage the queues

        // check if a new process has arrived
        if (input_queue_first != NULL && input_queue_first->pcb->arrival <= current_time) {
            queue_member* old_queue_first = queue_first;
            // loop in case of multiple arrivals
            while (input_queue_first->pcb->arrival <= current_time) {
                receive_next_job();
            }
            // was something added to the front of the queue? do a context switch
            if (old_queue_first != queue_first) {
                context_switch();
            }
        }
        

        // second, check if context switch is needed for round robin
        if (algo == RR && current_time - last_quantum_start >= quantum_size) {
            context_switch();
        }
        

        // third, perform a burst cycle
        do_tick();

        
        // fourth, check if process just completed running, since context_switch() needs to be called before the time is incremented
        if (queue_first->remaining_time <= 0) {
            // context switch checks for finished process and handles it
            context_switch();
        }


        // finally, increment the time for the next cycle
        current_time += 1;
    }

    return 0;

}

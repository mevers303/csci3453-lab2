#include "headers/parameters.h"
#include "headers/pcb.h"





int main(int argc, char* argv[]) {
 
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

    // read the first in queue
    current_time = 0;
    load_input_file(input_file);
    queue_member* this_queue_item = queue_first;


    // loop while we still have queue items
    while (!(queue_first == NULL && input_queue_first == NULL)) {

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

        do_tick();
        current_time += 1;
    }

    return 0;

}

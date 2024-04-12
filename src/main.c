#include "headers/parameters.h"
#include "headers/pcb.h"





int main(int argc, char* argv[]) {
 
    // read the arguments
    if (argc != 3 && argc != 4) {
        printf("Improper arguments");
    }
    const char* input_file = argv[1];
    // sanity check algorithm enum
    int alg_i = atoi(argv[2]);
    if (alg_i < 0 ||  alg_i > 2) {
        printf("Algorithm selection must be between [0, 2].");
        return 1;
    }
    const enum algorithm algo = alg_i;
    int quantum_size = -1;
    if (argc == 4) {
        quantum_size = atoi(argv[3]);
    }

    // sanity check for RR and quantum size
    if (algo == RR && quantum_size == -1) {
        printf("You must select a proper quantum size.");
        return 1;
    }


    /////////////////////////////
    ///////   MAIN LOOP /////////
    /////////////////////////////

    // read the first in queue
    current_time = 0;
    load_input_file(input_file);
    queue_member* this_queue_item = queue_first;


    // loop while we still have queue items
    while (queue_first != NULL && input_queue_first != NULL) {

        // check if a new process has arrived
        if (input_queue_first != NULL && input_queue_first->pcb->arrival <= current_time) {
            receive_next_job();
        }

        do_tick();
        current_time += 1;
    }

    return 0;

}

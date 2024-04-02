#include "parameters.h"
#include "pcb.h"





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

    // read the file
    queue_start = read_workload(input_file, algo);

    /**************************************************************************\
     *                                                                          * 
     *                                                                          *
     *                           MAIN  LOOP                                     *
     *                                                                          *  
     *                                                                          * 
    \***************************************************************************/

    while (queue_size) {

    
         return 0;

    }

    return 0;

}

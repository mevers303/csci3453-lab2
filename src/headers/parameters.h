#ifndef PARAMETERS_H
#define PARAMETERS_H

// defined by assignment
const float CONTEXT_SWITCH_COST = 0.5;
#define pid_array_chunk_size 1000

// enum for algorithm names
enum algorithm {
    FCFS = 0,
    SRTF = 1,
    RR = 2
};

// VERY IMPOLRTANT
float current_time = -1;
float quantum_size = -1;
enum algorithm algo = FCFS;
float last_quantum_start = -1;


#endif

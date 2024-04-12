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

// VERY IMPLOLRTANT
float current_time;
enum algorithm algo;


#endif

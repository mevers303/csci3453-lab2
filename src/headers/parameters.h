#ifndef PARAMETERS_H
#define PARAMETERS_H

const float CONTEXT_SWITCH_COST = 0.5;
#define pid_array_chunk_size 1000

enum algorithm {
    FCFS = 0,
    SRTF = 1,
    RR = 2
};

enum algorithm main_algo = FCFS;


#endif
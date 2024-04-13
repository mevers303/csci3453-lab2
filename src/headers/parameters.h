// paramaters.h
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2

#ifndef PARAMETERS_H
#define PARAMETERS_H

// defined by assignment
extern const float CONTEXT_SWITCH_COST;

// enum for algorithm names
enum algorithm {
    FCFS = 0,
    SRTF = 1,
    RR = 2
};

// GLOBAL VARS
extern float current_time;
extern float quantum_size;
extern enum algorithm algo;
extern float last_quantum_start;


#endif

// processes.h
// Project: CSCI-3453 Lab #2 
// Description: University of Colorado Operating System Concepts (CSCI-3453) Lab #2: Process scheduler Simulation (3 algorithms demo) 
// Author: Mark Evers <mark.evers@ucdenver.edu>, <mevers303@gmail.com>
// Repository: https://github.com/mevers303/csci3453-lab2


#ifndef PROCESSES_H
#define PROCESSES_H

/**********************************************
 *                                            *
 *                 FUNCTIONS                  *
 *                                            *
***********************************************/

// performs one processor burst
void do_tick();
// switches between processes, performs context switch if necessary
// runs BEFORE current_time is incremented at end of each tick
void switch_process();
// perform the output
void do_output();


#endif

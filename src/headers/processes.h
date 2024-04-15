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
// starts the current_process
void start_current_process();
// finalizes the current process and switches to the next
void end_current_process();
// round robin switcher
void rr_switch_process();
// perform the output
void do_output();




#endif

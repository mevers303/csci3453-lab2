#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "pcb.h";


queue_member* read_workload(const char* filepath);
void insert_pcb_to_queue(PCB* this_pcb, enum algorithm alg);

#endif

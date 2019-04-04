#ifndef MAIN_H
#define MAIN_H

#include "p1.5test.h"
#include "utils.h"

extern struct list_head ready_queue ;

void init_new_area(unsigned int new_address, unsigned int handler);
void init_process(unsigned int n, unsigned int addr_process, pcb_t *process);

#endif
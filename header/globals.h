#ifndef GLOBALS_H
#define GLOBALS_H

#include "types_rikaya.h"


extern struct list_head ready_queue ;
extern int ready_processes ;
extern pcb_t *running_process ;

void state_copy(state_t *s1, state_t *s2);

#endif 

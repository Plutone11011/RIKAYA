#ifndef GLOBALS_H
#define GLOBALS_H

#include "types_rikaya.h"
#include "const.h"

extern struct list_head ready_queue ;
extern int ready_processes ;
extern pcb_t *running_process ;

//valori dei semafori per i device I/O
extern int devs[DEV_PER_INT][NUM_LINES - 3];
//valori semafori per terminali (recv e transm)
extern int terms[DEV_PER_INT][2];


void state_copy(state_t *s1, state_t *s2);

#endif 

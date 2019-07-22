#ifndef GLOBALS_H
#define GLOBALS_H

#include "pcb.h"

//distinzione per il timer da settare
#define PSEUDOCLOCK 0
#define TIMESLICE 1

extern struct list_head ready_queue ;
extern int ready_processes, active_processes, blocked_processes ;
extern pcb_t *running_process ;


extern int devs[DEV_PER_INT][NUM_LINES - 3];
extern int terms[DEV_PER_INT][2];
extern int waitclockSem, timer_cause;

/*debug variables*/
extern int debug ;
void setDebug(int d);

void state_copy(state_t *s1, state_t *s2);

void init_Kernel_Vars();
void setHILOtime(cpu_t *time);

#endif 

#ifndef GLOBALS_H
#define GLOBALS_H

#include "pcb.h"

extern struct list_head ready_queue ;
extern int ready_processes, active_processes ;
extern pcb_t *running_process ;


extern int devs[DEV_PER_INT][NUM_LINES - 3];
extern int terms[DEV_PER_INT][2];
extern int blocked_processes ;

/*debug variables*/
extern int debug ;
extern int linenumber ;
extern int devnumber ;
extern struct list_head *sem_head ;
void setDebug(int d);

void state_copy(state_t *s1, state_t *s2);

void init_Kernel_Vars();

#endif 

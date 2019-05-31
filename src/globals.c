#include "../header/globals.h"

struct list_head ready_queue ;
int ready_processes ;
pcb_t *running_process = NULL ;

int devs[DEV_PER_INT][NUM_LINES - 3];
int terms[DEV_PER_INT][2];

void state_copy(state_t *s1, state_t *s2){

    int i = 0 ;
    s1->cause = s2->cause ;
    s1->entry_hi = s2->entry_hi ;
    s1->hi = s2->hi ;
    s1->lo = s2->lo ;
    s1->status = s2->status ;
    for (i = 0; i < STATE_GPR_LEN ; i++){
		s1->gpr[i] = s2->gpr[i];
	}
    s1->pc_epc = s2->pc_epc;
}

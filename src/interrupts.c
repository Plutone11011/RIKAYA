#include "../header/interrupts.h"



void interrupt_handler(){

    //int i ;
    state_t *old_process_state = (state_t*)INTERRUPT_OLDAREA ;

    unsigned int cause = old_process_state->cause ;
    
	state_copy(&(running_process->p_s),old_process_state);

    if (cause & CAUSE_IP(1)){
        if (!emptyProcQ(&ready_queue)){
            pcb_t *ready_pcb ;
            list_for_each_entry(ready_pcb,&ready_queue,p_next){
                ready_pcb->priority++ ;
            }
        }
        schedule();
    }
    //other interrupts, device I/O...
    else {
        PANIC();
    }
    
}
 
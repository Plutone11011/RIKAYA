#include "../header/interrupts.h"


void interrupt_handler(){

    int i ;
    state_t *old_process_state = (state_t*)INTERRUPT_OLDAREA ;

    unsigned int cause = old_process_state->cause ;
    
	running_process->p_s.cause = old_process_state->cause ;
    running_process->p_s.entry_hi = old_process_state->entry_hi ;
    running_process->p_s.hi = old_process_state->hi ;
    running_process->p_s.lo = old_process_state->lo ;
    running_process->p_s.status = old_process_state->status ;
    for (i = 0; i < STATE_GPR_LEN ; i++){
		running_process->p_s.gpr[i] = old_process_state->gpr[i];
	}
    running_process->p_s.pc_epc = old_process_state->pc_epc;
        
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
        
    }
    
}

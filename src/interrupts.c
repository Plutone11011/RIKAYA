#include "../header/interrupts.h"



void interrupt_handler(){

    //int i ;
    state_t *old_process_state = (state_t*)INTERRUPT_OLDAREA ;

    unsigned int cause = old_process_state->cause ;
    //copia lo stato dell'area old nel processo running
    //per poterlo ripristinare in futuro
	state_copy(&(running_process->p_s),old_process_state);

    //se il processor local timer ha generato l'interrupt
    //si chiama lo scheduler con le priorità
    //aggiornate
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
 
#include "../header/main.h"

void init_new_area(unsigned int new_address, unsigned int handler){
    state_t *new_state = (state_t *) new_address ;

    new_state->pc_epc = handler ;
    new_state->gpr[26] = RAMTOP ;

    
    new_state->status &= ~STATUS_IEc ;//interrupt
    new_state->status &= ~STATUS_KUc ;//kernel mode
    new_state->status &= ~STATUS_VMc ;//vm
    new_state->status |= STATUS_TE ;
}

void init_process(unsigned int n, unsigned int addr_process, pcb_t *process){

    //char buf[10];
    process->priority = n ;
    process->original_priority = n ;
    process->p_s.pc_epc = addr_process ;
    process->p_s.gpr[26] = RAMTOP - (FRAMESIZE*n) ;

    
    //isola l'interrupt mask e accende il bit dell'interval timer
    process->p_s.status |= (process->p_s.status & STATUS_IM_MASK) | STATUS_IM(1) ;
    process->p_s.status &= ~STATUS_KUc ;
    process->p_s.status &= ~STATUS_VMc ;
    process->p_s.status |= STATUS_IEc ;//abilita interrupt 
    process->p_s.status |= STATUS_TE ;

    insertProcQ(&ready_queue,process);
    ready_processes++ ;
}

int main(){

    pcb_t p1, p2, p3 ;
    init_new_area(INTERRUPT_NEWAREA,(unsigned int)interrupt_handler);
    init_new_area(PROGRAMTRAP_NEWAREA,(unsigned int)programtrap_handler);
    init_new_area(SYSCALL_NEWAREA,(unsigned int)syscall_handler);
    init_new_area(TLB_NEWAREA,(unsigned int)tlb_handler);


    initPcbs();
    mkEmptyProcQ(&ready_queue); 


    init_process(1,(unsigned int)test1,&p1);
    init_process(2,(unsigned int)test2,&p2);
    init_process(3,(unsigned int)test3,&p3);
    schedule();
    
    return 0 ;
}
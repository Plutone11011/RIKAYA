#include "../header/main.h"

struct list_head ready_queue ;

void init_new_area(unsigned int new_address, unsigned int handler){
    state_t *new_state = (state_t *) new_address ;

    new_state->pc_epc = handler ;
    new_state->gpr[26] = RAMTOP ;

    new_state->status &= ~STATUS_IEc ;//interrupt
    new_state->status &= ~STATUS_KUc ;//kernel mode
    new_state->status &= ~STATUS_VMc ;//vm

    //abilitare timer?
}

void init_process(unsigned int n, unsigned int addr_process, pcb_t *process){

    process->priority = n ;
    process->p_s.pc_epc = addr_process ;
    process->p_s.gpr[26] = RAMTOP - (FRAMESIZE*n) ;
    process->p_s.status &= ~STATUS_KUc ;//kernel mode
    process->p_s.status &= ~STATUS_VMc ;//vm
    process->p_s.status |= STATUS_IEc ;//abilita interrupt
    process->p_s.status |= STATUS_TE ; //abilita timer
}

int main(){

    pcb_t p1, p2, p3 ;
    init_new_area(INTERRUPT_NEWAREA,(unsigned int)interrupt_handler);
    init_new_area(PROGRAMTRAP_NEWAREA,(unsigned int)programtrap_handler);
    init_new_area(SYSCALL_NEWAREA,(unsigned int)syscall_handler);
    init_new_area(TLB_NEWAREA,(unsigned int)tlb_handler);

    mkEmptyProcQ(&ready_queue);

    initPcbs();

    init_process(1,test1,&p1);
    //init_process(2,test2,&p2);
    //init_process(3,test3,&p3);
    insertProcQ(&ready_queue,&p1);
    LDST(&(p1.p_s));
    
    return 0 ;
}
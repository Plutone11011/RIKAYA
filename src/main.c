#include "../header/main.h"

void init_new_area(unsigned int new_address, unsigned int handler){
    state_t *new_state = (state_t *) new_address ;

    new_state->pc_epc = handler ;
    new_state->gpr[26] = RAMTOP ;

    //questi valori sono già così di default
    //new_state->status &= ~STATUS_IEc ;//interrupt
    //new_state->status &= ~STATUS_KUc ;//kernel mode
    //new_state->status &= ~STATUS_VMc ;//vm
    new_state->status |= STATUS_TE ;
}

void init_first_process(){
    
    state_t statep ;
    
    statep.pc_epc = (memaddr)test ;
    statep.reg_sp = RAMTOP - FRAME_SIZE ;

    
    //isola l'interrupt mask e accende il bit dell'interval timer
    
    statep.status |= STATUS_IM(1) ;
    //process->p_s.status &= (~STATUS_KUc) ;
    //process->p_s.status &= (~STATUS_VMc) ; 
    statep.status |= STATUS_TE ;
    //IEp perché la ROM fa la pop dello stack 
    //degli interrupt quando processa LDST
    //quindi il bit previous diventa current
    statep.status |= STATUS_IEp ;

    //cpid è NULL perché viene creato il primo processo
    //non è figlio di nessuno
    createProcess(&statep,0,NULL);
}

int main(){
    
    //inizializza le new area per ogni eccezione
    init_new_area(INTERRUPT_NEWAREA,(unsigned int)interrupt_handler);
    init_new_area(PGMTRAP_NEWAREA,(unsigned int)programtrap_handler);
    init_new_area(SYSBK_NEWAREA,(unsigned int)SYS_handler);
    init_new_area(TLB_NEWAREA,(unsigned int)tlb_handler);


    initPcbs();
    initASL();
    mkEmptyProcQ(&ready_queue); 

    init_process();
    
    schedule();
    
    return 0 ;
}
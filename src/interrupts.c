#include "../header/interrupts.h"


void interrupt_handler(){
    
    unsigned int cause = ((state_t*)INTERRUPT_OLDAREA)->cause ;
    termprint("Interrupt caught",0);
    //interrupt timer
    //set timer probabilmente setta il timer locale
    //del processore (ma se è così settarlo nel main equivale a settarlo per il processo?)
    if (cause & CAUSE_IP(2)){
        termprint("Timer",0);
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

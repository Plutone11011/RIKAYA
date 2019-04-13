#include "../header/handler.h"
#include "../header/globals.h"

void TerminateProcess (pcb_t *p) {
    //Nel caso in cui si cerchi di terminare il processo in esecuzione
    if (p == running_process) running_process = NULL;
    //Se p è nella ready_queue diminuiamo il numero di processi ready
    if (outProcQ(&ready_queue, p) != NULL) ready_processes--;
    freePcb(p);
}


void SYS_handler () {
    /*
    switch (TYPE) {
        case TERMINATE_PROCESS: {
            TerminateProcess(pid); 
        }
            break;
    
        default:
            break;
    }
    */
}

void programtrap_handler(){
    termprint("pg trap",0);
    //PANIC();
}

void tlb_handler(){
    termprint("tlb trap",0);
    //PANIC();
}

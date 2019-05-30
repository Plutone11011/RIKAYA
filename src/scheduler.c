#include "../header/scheduler.h"



void schedule(){
    pcb_t *pcb_to_run ;
    //controlla se ci sono processi ready
    //se sì, manda il primo processo della coda
    if (ready_processes > 0){

        pcb_to_run = removeProcQ(&ready_queue);

        ready_processes-- ;
        

        if (running_process != NULL){
            //il processo in esecuzione prima del context switch
            //deve essere rimesso in ready
            insertProcQ(&ready_queue,running_process);
            ready_processes++ ;
        }
        running_process = pcb_to_run ;
        running_process->priority = running_process->original_priority ;

        log_process_order(running_process->original_priority);
        setTIMER(SCHED_TIME_SLICE*TIME_SCALE);
        LDST(&running_process->p_s);
    }
    else {
        //se non ci sono processi ready
        //o sono in waiting (non in questa fase)
        //oppure sono terminati
        HALT();
    }

}
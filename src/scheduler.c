#include "../header/scheduler.h"



void schedule(){
    //char buf[10];
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
        //itoa(running_process->p_s.pc_epc,buf);
        //termprint(buf,0);
        log_process_order(running_process->original_priority);
        setTIMER(TIMESLICE*TIME_SCALE);
        LDST(&running_process->p_s);
        //HALT();
    }
    else {
        //se non ci sono processi ready
        //o sono in waiting (non in questa fase)
        //oppure sono terminati
        termprint("Schedule terminated",0);
        HALT();
    }

}


//copia src in dest
void state_copy(state_t *dest, state_t *src){
	
	int i;
	dest->cause = src->cause ;
	dest->entry_hi = src->entry_hi ;
	dest->hi = src->hi ;
	dest->lo = src->lo ;
	dest->pc_epc = src->pc_epc ;
	dest->pc_epc -= 4 ;
	dest->status = src->status ;
	for (i = 0; i < STATE_GPR_LEN ; i++){
		dest->gpr[i] = src->gpr[i] ;
	}
}
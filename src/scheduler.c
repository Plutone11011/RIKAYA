#include "../header/scheduler.h"


//schedula i processi, inserendo quello appena switchato
//nella coda ready, e settando il timeslice
//old viene usato per aggiornare lo stato precedente del pcb
//con quello al momento del context switch 
void schedule(state_t *old){
    int status ;
    //controlla se ci sono processi ready
    //se sì, manda il primo processo della coda
    if (ready_processes > 0){

        //spostato inserimento del running process
        //nell'interrupt handler, sotto timeslice
        //perché potrebbe avere priorità maggiore degli altri
        running_process = removeProcQ(&ready_queue);

        ready_processes-- ;

        //log_process_order(running_process->original_priority);
        setTIMER(SCHED_TIME_SLICE*TIME_SCALE);
        LDST(&running_process->p_s);
    }
    else {
        if (blocked_processes > 0){
            //lo scheduler non deve prendere nessuna
            //decisione, non ci sono processi ready,
            //ma deve aspettare che i processi terminino
            //le operazioni I/O
            status = getSTATUS();
            int i ;
            status |= STATUS_IEc ;
            //abilita interrupt device I/O
            for (i = INT_LOWEST; i < INT_LOWEST + DEV_USED_INTS; i++){
                status |= STATUS_IM(i) ;
            }
            //forse necessario disabilitare timer
            //visto che ha priorità maggiore degli 
            //interrupt dei device I/O
            status &= (~STATUS_TE);
            setSTATUS(status);
            WAIT();
        }
        else {
            HALT();
        }
    }

}

//inserisce un processo nella coda ready, che sia quello running
//o uno nuovo. Aggiorna lo stato del pcb e ne ripristina la priorità
//originale.
void insertProcqReady(state_t *old, pcb_t *proc){

    if (proc != NULL){

        
        //può essere chiamato da createprocess,P,V
        //ma anche dallo scheduler
        //proc può quindi essere un nuovo processo
        // o il processo appena switchato
        if (running_process == proc){

            if (old != NULL){
                state_copy(&proc->p_s,old);
            }
            proc->priority = proc->original_priority ;
            
        }//allora è stato invocato dallo scheduler

        insertProcQ(&ready_queue,proc);
        ready_processes++ ;
    }
}
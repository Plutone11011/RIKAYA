#include "../header/scheduler.h"


/*
    Schedula i processi, inserendo quello appena switchato
    nella coda ready, e settando il timeslice
    old viene usato per aggiornare lo stato precedente del pcb
    con quello al momento del context switch.
*/
void schedule(state_t *old){
    int status ;
    pcb_t *pcb_to_run ;
    /* Se ci sono processi ready manda il primo processo della coda */
    if (ready_processes > 0){
        
        pcb_to_run = removeProcQ(&ready_queue);

        ready_processes-- ;
        if (running_process != NULL){
            insertProcqReady(old,running_process);
        }
        running_process = pcb_to_run ;
        set_timer();
        setHILOtime(&running_process->last_scheduled);
        LDST(&running_process->p_s);
    }
    else {
        if (blocked_processes > 0){
        /*
            Lo scheduler non deve prendere nessuna
            decisione, non ci sono processi ready,
            ma deve aspettare che i processi terminino
            le operazioni I/O
        */
            status = getSTATUS();
            status |= STATUS_IEc ;

            setSTATUS(STATUS_ALL_INT_ENABLE(status));
            WAIT();
        }
        else {
            HALT();
        }
    }

}

void update_usertime(pcb_t *userproc){

    if (userproc){
        userproc->user_time += (userproc->start_kernel - userproc->last_scheduled);
    }
    
}

void update_kerneltime(pcb_t *kernelproc){

    cpu_t now ;

    setHILOtime(&now);
    if (kernelproc){
        kernelproc->kernel_time += (now - kernelproc->start_kernel);
    }
}

/*
    Inserisce un processo nella coda ready, che sia quello running
    o uno nuovo. Aggiorna lo stato del pcb e ne ripristina la priorità
    originale.
*/
void insertProcqReady(state_t *old, pcb_t *proc){

    if (proc != NULL){
    /*
        Può essere chiamato da createprocess,P,V ma anche dallo 
        scheduler. Proc può quindi essere un nuovo processo
        o il processo appena switchato.
    */
        if (running_process == proc){

            if (old != NULL){
                state_copy(&proc->p_s,old);
            }
            proc->priority = proc->original_priority ;
            
        }
        insertProcQ(&ready_queue,proc);
        ready_processes++ ;
    }
}
/*
    Decide se settare il timer per lo scheduler o settare il timer 
    per il clock della waitclock.
*/
void set_timer(){
/*
    Calcola quanto tempo rimane allo scadere del prossimo pseudoclock
    modulando il time of day corrente.
*/
    int pseudoclock_remainingTime = SCHED_PSEUDO_CLOCK - ((getTODLO()/TIME_SCALE) % SCHED_PSEUDO_CLOCK) ;
    
    if (pseudoclock_remainingTime <= SCHED_TIME_SLICE){

        timer_cause = PSEUDOCLOCK ;
        setTIMER(pseudoclock_remainingTime*TIME_SCALE);
    }   
    else {
        timer_cause = TIMESLICE ;
        setTIMER(SCHED_TIME_SLICE*TIME_SCALE);
    }
}

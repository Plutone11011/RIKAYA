#include "../header/handler.h"
#include "../header/globals.h"

//terminare progenie solo in fase 2
//quindi non bisogna toccare la coda ready
void TerminateProcess () {
    
    running_process = NULL ;
    freePcb(running_process);

    schedule();
}


void SYS_handler () {

    state_t *old_process_state = (state_t*)SYSCALL_OLDAREA ;
    //distinzione tra syscall e breakpoint
    if (CAUSE_GET_EXCCODE(old_process_state->cause) == EXC_SYS){
        switch(old_process_state->reg_a0){
            case SYS3:
                TerminateProcess();
                break ;
            default:
                break ;

        }
    }
    else {
        PANIC();
    }
}

void programtrap_handler(){
    termprint("pg trap",0);
    //PANIC();
}

void tlb_handler(){
    termprint("tlb trap",0);
    //PANIC();
}

#include "../header/globals.h"

struct list_head ready_queue ;
int ready_processes ;
pcb_t *running_process  ;

//valori dei semafori per i device I/O
//valori semafori per terminali (recv e transm)
//usati quando le syscall vengono invocate da altre funzioni
//del kernel e non dai processi
int devs[DEV_PER_INT][NUM_LINES - 3];
int terms[DEV_PER_INT][2];
//bisogna contare quanti processi sono bloccati
//perché il kernel potrebbe doversi fermare, temporaneamente
//nel caso in cui non ci siano processi ready, ma processi waiting
int blocked_processes ;

/*debug variables*/
int devnumber = -1 ;
int linenumber = -1 ;
int debug = -1;
struct list_head *sem_head = NULL ;

void state_copy(state_t *s1, state_t *s2){

    int i = 0 ;
    s1->cause = s2->cause ;
    s1->entry_hi = s2->entry_hi ;
    s1->hi = s2->hi ;
    s1->lo = s2->lo ;
    s1->status = s2->status ;
    for (i = 0; i < STATE_GPR_LEN ; i++){
		s1->gpr[i] = s2->gpr[i];
	}
    s1->pc_epc = s2->pc_epc;
}
//chiamata una volta nel main
void init_Kernel_Vars(){

    int i, j ;
    mkEmptyProcQ(&ready_queue);
    ready_processes = 0 ;
    running_process = NULL ;
    blocked_processes = 0 ;
    //inizialmente a 0 perché qualora avvenga subito
    //un'operazione di I/O (syscall 7 Do_IO)
    //il processo deve bloccarsi finché l'operazione 
    //non è completata, ovvero quando viene sollevato un'interrupt
    for (i = 0; i < DEV_PER_INT; i++){
        for (j = 0; j < NUM_LINES - 3; j++){
            devs[i][j] = 0 ;
            if (j == 1 || j == 0){
                terms[i][j] = 0 ;
            }
        }

    }

    
}
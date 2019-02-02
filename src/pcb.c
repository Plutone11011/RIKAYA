#include "../header/pcb.h"

void initPcbs(void){

    int i ;
    struct list_head *tmp ;

    pcbfree_h = &pcbFree_table[0] ;

    /*inizializza lista circolare con sentinella*/
    INIT_LIST_HEAD(&(pcbfree_h->p_next));

    for (i = 1; i < MAXPROC; i++){
        tmp = &(pcbFree_table[i].p_next) ;
        /*aggiunge l'i-esimo elemento della lista in coda*/
        list_add_tail(tmp,&(pcbfree_h->p_next));
    }      
    
}

static void setlist_head(struct list_head list){
    list.next = NULL ;
    list.prev = NULL ;
}

pcb_t *allocPcb(void){  

    int i ;
    pcb_t *tmp = container_of(pcbfree_h->p_next.prev,pcb_t,p_next) ;

    if (pcbfree_h != NULL){
        /*elimino elemento in coda*/

        tmp->p_s.entry_hi = 0;
        tmp->p_s.hi = 0;
        tmp->p_s.lo = 0;
        tmp->p_s.cause = 0;
        tmp->p_s.status = 0;
        tmp->p_s.pc_epc = 0;

        for (i = 0; i < STATE_GPR_LEN; i++){
            tmp->p_s.gpr[i] = 0 ;
        }
        tmp->p_parent = NULL ;
        tmp->p_semkey = NULL ;
        tmp->priority = 0 ;
        setlist_head(tmp->p_child) ;
        setlist_head(tmp->p_sib) ;
        /*forse anche nel tree andrà fatta stessa cosa*/
        if (pcbfree_h != tmp){
            list_del(&(tmp->p_next));
        }
        /* se è rimasto un solo elemento list_del non funziona perché assume sia una lista con sentinella */
        else {
            tmp->p_next.next = NULL ;
            tmp->p_next.prev = NULL ;
            pcbfree_h = NULL ;
        }
        return tmp ;
    }
    else {
        return NULL ;
    }

}

#include "../header/pcb.h"

//debugging variable, counts number of enqueued processes
unsigned int enqueuedProc = 0 ;

void initPcbs(void){

    int i ;
    struct list_head *iterator ;

    pcbfree_h = &pcbFree_table[0] ;

    /*inizializza lista circolare per list_add_tail*/
    INIT_LIST_HEAD(&(pcbfree_h->p_next));

    for (i = 1; i < MAXPROC; i++){
        iterator = &(pcbFree_table[i].p_next) ;
        /*aggiunge l'i-esimo elemento della lista in coda*/
        list_add_tail(iterator,&(pcbfree_h->p_next));
    }      
    
}

static void setlist_head(struct list_head list){
    list.next = NULL ;
    list.prev = NULL ;
}

pcb_t *allocPcb(void){  

    int i ;
    

    if (pcbfree_h != NULL){
        /*elimino elemento in coda*/
        pcb_t *lastPcb = container_of(pcbfree_h->p_next.prev,pcb_t,p_next) ;

        lastPcb->p_s.entry_hi = 0;
        lastPcb->p_s.hi = 0;
        lastPcb->p_s.lo = 0;
        lastPcb->p_s.cause = 0;
        lastPcb->p_s.status = 0;
        lastPcb->p_s.pc_epc = 0;

        for (i = 0; i < STATE_GPR_LEN; i++){
            lastPcb->p_s.gpr[i] = 0 ;
        }
        lastPcb->p_parent = NULL ;
        lastPcb->p_semkey = NULL ;
        lastPcb->priority = 0 ;
        setlist_head(lastPcb->p_child) ;
        setlist_head(lastPcb->p_sib) ;
        /*forse anche nel tree andrà fatta stessa cosa*/
        if (pcbfree_h != lastPcb){
            list_del(&(lastPcb->p_next));
        }
        
        else {
            /*se il pcb è l'ultimo list_del non funziona*/
            lastPcb->p_next.next = NULL ;
            lastPcb->p_next.prev = NULL ;
            pcbfree_h = NULL ;
        }
        return lastPcb ;
    }
    else {
        return NULL ;
    }

}


void freePcb(pcb_t *p){
    if (pcbfree_h == NULL){
        pcbfree_h = p ;
        INIT_LIST_HEAD(&(pcbfree_h->p_next));/*inizializza il primo elemento per list_add_tail*/
    }
    else {
        list_add_tail(&(p->p_next),&(pcbfree_h->p_next));
    }
}

void mkEmptyProcQ(struct list_head *head){

    INIT_LIST_HEAD(head);

}

int emptyProcQ(struct list_head *head){

    return list_empty(head) ;

}

void insertProcQ(struct list_head *head, pcb_t *p){
    
    struct list_head *iterator ;
    //se la lista è vuota
    //lo si aggiunge in coda
    if (emptyProcQ(head)){
        list_add_tail(&(p->p_next), head);
    }
    else {
        list_for_each(iterator,head){
            //se la priorità corrente è minore di quella di p
            if ((container_of(iterator,pcb_t,p_next))->priority <= p->priority){
                //allora lo si inserisce tra il corrente e il precendente
                __list_add(&(p->p_next),iterator->prev, iterator);
                break ;
            }
            //se invece p ha priorità minima
            else if (iterator->next == head){
                list_add_tail(&(p->p_next), head);
            }
        }
    }
}

pcb_t *headProcQ(struct list_head* head){

    return container_of(list_next(head),pcb_t,p_next);

}

pcb_t *removeProcQ(struct list_head *head){

    if (emptyProcQ(head)){
        return NULL ;
    }
    else {
        pcb_t *firstPcb = container_of(head->next,pcb_t,p_next) ;
        list_del(head->next);
        return firstPcb ;

    }
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p){

    struct list_head *iterator ;
    if (emptyProcQ(head)){
        return NULL ;
    }
    else {
        list_for_each(iterator, head){
            if (container_of(iterator,pcb_t,p_next) == p){
                list_del(iterator);
                return p ;
            }
        }
        return NULL ;
    }
}
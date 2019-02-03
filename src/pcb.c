#include "../header/pcb.h"

void initPcbs(void){

    int i ;
    struct list_head *tmp ;

    pcbfree_h = &pcbFree_table[0] ;

    /*inizializza lista circolare per list_add_tail*/
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
    

    if (pcbfree_h != NULL){
        /*elimino elemento in coda*/
        pcb_t *tmp = container_of(pcbfree_h->p_next.prev,pcb_t,p_next) ;

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


void freePcb(pcb_t *p){
    if (pcbfree_h == NULL){
        pcbfree_h = p ;
        INIT_LIST_HEAD(&(pcbfree_h->p_next));//inizializza il primo elemento per list_add_tail
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
    
    struct list_head *tmp ;
    //se la lista è vuota o p ha priorità minima
    //lo si aggiunge in coda
    if (emptyProcQ(head)){
        list_add_tail(&(p->p_next), head);
    }
    else {
        list_for_each(tmp,head){
            //se la priorità corrente è minore di quella di p
            if ((container_of(tmp,pcb_t,p_next))->priority <= p->priority){
                //allora lo si inserisce tra il corrente e il precendente
                __list_add(&(p->p_next),tmp->prev, tmp);
                break ;
            }
            //se invece p ha priorità minima
            else if (tmp->next == head){
                list_add_tail(&(p->p_next), head);
            }
        }
    }
}

pcb_t *headProcQ(struct list_head* head){

    return container_of(list_next(head),pcb_t,p_next);

}
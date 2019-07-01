#include "../header/pcb.h"

pcb_t *pcbfree_h ;

pcb_t pcbFree_table[MAXPROC];

struct list_head *prev, *next ;

void initPcbs(void){

    unsigned int i ;
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

    unsigned int i ;
    

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
        lastPcb->priority = lastPcb->original_priority = 0 ;
        lastPcb->tutor = FALSE;
        lastPcb->user_time = lastPcb->kernel_time = lastPcb->wallclock_time = 0;

        setlist_head(lastPcb->p_child) ;
        setlist_head(lastPcb->p_sib) ;
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
            if ((container_of(iterator,pcb_t,p_next))->priority < p->priority){
                //allora lo si inserisce tra il corrente e il precendente
                list_add_between(&(p->p_next),iterator->prev, iterator);
                break ;
            }
            else if ((container_of(iterator,pcb_t,p_next))->priority == p->priority){
                //lo inserisco in fondo a quelli con priorità uguale, 
                //in modo che tra tutti sia il meno recente
                //ad essere servito
                if ((container_of(iterator->next,pcb_t,p_next))->priority == p->priority){
                    continue ;
                }
                else {
                    list_add_between(&(p->p_next),iterator, iterator->next);
                    break ;
                }

            }
            //se invece p ha priorità minima
            //vuol dire che si è arrivati in fondo alla lista
            else if (iterator->next == head){
                list_add_tail(&(p->p_next), head);
                break ;
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
            //controlla che p sia presente nella coda
            if (container_of(iterator,pcb_t,p_next) == p){
                list_del(iterator);
                return p ;
            }
        }
        return NULL ;
    }
}

int emptyChild(pcb_t *this){

    if (this->p_child.next == NULL){
        //bisogna inizializzare la lista vuota dei figli
        //la sentinella è la radice del sottoalbero
        INIT_LIST_HEAD(&(this->p_child));
        return 1 ;
    }
    else {
        //non ha figli se è la sentinella ma anche
        //se il pcb puntato dal parent del next child 
        //è diverso da sé stesso   
        return list_empty(&(this->p_child)) || (container_of(this->p_child.next,pcb_t,p_child)->p_parent != this) ;
    }
}

void insertChild(pcb_t *prnt, pcb_t *p){

    p->p_parent = prnt ;
    //inizializzo la lista dei sibling per trattarli come le code
    INIT_LIST_HEAD(&(p->p_sib));
    //controllo se questo è il primo nodo
    //della lista dei figli
    if (emptyChild(prnt)){
        list_add_between(&(p->p_child),&(prnt->p_child),prnt->p_child.next);
        
    }
    else {
        
        //prnt ha figli e quindi si aggiunge p alla lista dei fratelli
        list_add_tail(&(p->p_sib),&(container_of(prnt->p_child.next,pcb_t,p_child)->p_sib)) ;
    

    }


}

pcb_t *removeChild(pcb_t *p){

    pcb_t *firstChild ; //mantiene riferimento a ex primogenito per toglierlo dai fratelli

    if (emptyChild(p)){
        return NULL ;
    }
    else {
        firstChild = container_of(p->p_child.next,pcb_t,p_child) ;
        //elimina riferimento al padre
        firstChild->p_parent = NULL ;
        //cancella figlio dalla lista dei figli del padre
        list_del(&(firstChild->p_child));
        //aggiunge fratello successivo a lista dei figli
        //solo se non è l'ultimo
        if (!list_empty(&(firstChild->p_sib))){
            list_add_between(&(container_of(firstChild->p_sib.next,pcb_t,p_sib)->p_child),&(p->p_child),p->p_child.next);
        }
        //elimina riferimento ai fratelli
        list_del(&(firstChild->p_sib));
      
        return firstChild ;
    }
}

pcb_t *outChild(pcb_t *p){

    if (p->p_parent == NULL){
        return NULL ;
    }
    else {
        //se non è il primogenito, non deve modificare p_child
        if (p->p_parent->p_child.next != &(p->p_child)){
            list_del(&(p->p_sib));
            p->p_parent = NULL ;
        }
        else{
            
            list_del(&(p->p_child));
            //aggiunge fratello successivo a lista dei figli
            //solo se non è l'ultimo
            if (!list_empty(&(p->p_sib))){
                list_add_between(&(container_of(p->p_sib.next,pcb_t,p_sib)->p_child),&(p->p_parent->p_child),p->p_parent->p_child.next);
            }
            list_del(&(p->p_sib));
            p->p_parent = NULL ;
        }
        return p ;
    }
}
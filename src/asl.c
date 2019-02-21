#include "../header/asl.h"

void initASL(){
    unsigned int i ;
    struct list_head *iterator ;

    semdFree_h = &semd_table[0] ;

    INIT_LIST_HEAD(&(semdFree_h->s_next));

    for (i = 1; i < MAXPROC; i++){
        iterator = &(semd_table[i].s_next) ;
        /*aggiunge l'i-esimo elemento della lista in coda*/
        list_add_tail(iterator,&(semdFree_h->s_next));
    }

    semd_h = NULL ; //all'inizio i semafori non sono attivi
}

//funzione ausiliaria che ricerca il semaforo con chiave key nella ASL
//Precondition: l'ASL non è vuota
static semd_t* searchSemaphoreByKey(int *key){

    struct list_head *iterator ;

    if (semd_h->s_key == key){
        return semd_h ;
    }
    else {
        //l'iterazione assume che la testa sia il primo semaforo perché è già stato controllato
        //perciò il primo iterator sarà il secondo semaforo nella ASL
        list_for_each(iterator,&(semd_h->s_next)){
            if (container_of(iterator,semd_t,s_next)->s_key == key){
                return container_of(iterator,semd_t,s_next) ;
            }
        }
        return NULL ;
    }
}


// Alloca un semaforo dalla semdFree e lo mette nella ASL
static semd_t* allocSemaphore(){
    if (semdFree_h != NULL){
        /*elimino elemento in coda*/
        semd_t *lastSem = container_of(semdFree_h->s_next.prev,semd_t,s_next) ;

        
        if ((lastSem->s_next.next == &(lastSem->s_next)) && (lastSem->s_next.prev == &(lastSem->s_next))){
            semdFree_h = NULL ;
        }
        
        else {
            //Se invece il sem non è l'ultimo libero, devo eliminarlo dalla lista
            list_del(&(lastSem->s_next));
        }
        lastSem->s_next.next = NULL ;
        lastSem->s_next.prev = NULL ;
        lastSem->s_key = NULL ;
        return lastSem ;
    }
    else {
        return NULL ;
    }
}



semd_t* getSemd(int *key){

    if (semd_h == NULL){
        //allora non ci sono semafori nella lista degli attivi, e non ci sono pcb bloccati
        return NULL ;
    }
    else {
        return searchSemaphoreByKey(key);
    }
}

int insertBlocked(int *key,pcb_t* p){

    semd_t *newSem ;

    if (semd_h == NULL){
        newSem = allocSemaphore() ;
        if (newSem == NULL){
            //non ci sono più semafori liberi
            return 1 ;
        }
        else {
            semd_h = newSem ;
            p->p_semkey = key ;
            semd_h->s_key = key ;
            //anche l'ASL viene trattata come una lista bidirezionale circolare
            INIT_LIST_HEAD(&(semd_h->s_next)) ;
            //altrimenti non si sa quando la lista dei pcb bloccati è vuota
            INIT_LIST_HEAD(&(semd_h->s_procQ)) ;
            insertProcQ(&(semd_h->s_procQ),p);
            
            return 0 ;
        }
    }
    else {
        //l'ASL non è vuota, quindi si cerca il semaforo con chiave key
        semd_t *sem_withKey = searchSemaphoreByKey(key) ;
        
        if (sem_withKey != NULL){
            p->p_semkey = key ;
            //se il semaforo già esisteva, non c'è bisogno di inizializzarlo
            insertProcQ(&(sem_withKey->s_procQ),p);
            return 0 ;
        }
        else { //l'asl non è vuota ma il semaforo con quella chiave non c'è
            newSem = allocSemaphore() ;
            if (newSem == NULL){
                //non ci sono più semafori liberi
                return 1 ;
            }
            else {
                p->p_semkey = key ;
                newSem->s_key = key ;
                list_add_tail(&(newSem->s_next),&(semd_h->s_next)) ;
                //il semaforo è stato appena aggiunto quindi bisogna inizializzare la lista dei pcb bloccati
                INIT_LIST_HEAD(&(newSem->s_procQ));
                insertProcQ(&(newSem->s_procQ),p);

                return 0 ;
            }   
        }
    }
}
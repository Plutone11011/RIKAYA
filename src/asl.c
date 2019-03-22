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

        
        if (list_empty(&(semdFree_h->s_next))){
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
//chiamato solo quando il semaforo non ha più processi bloccati
static void freeSemaphore(semd_t *sem){

        if (list_empty(&(semd_h->s_next))){
            //se il semaforo è l'ultimo in ASL
            semd_h = NULL ;
        }
        else {
            if (sem == semd_h){
                //il semaforo da liberare è il primo, quindi semd_h deve puntare a quello successivo
                semd_h = container_of(semd_h->s_next.next,semd_t,s_next) ;
            }
            list_del(&(sem->s_next)) ;
        }
        //in entrambi i casi vengono aggiunti alla lista dei liberi
        if (semdFree_h == NULL){
            //va inizializzata
            semdFree_h = sem ;
            INIT_LIST_HEAD(&(semdFree_h->s_next));
        }
        else {
            list_add(&(sem->s_next),&(semdFree_h->s_next));
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
//non usa getSemd a differenza delle altre funzioni perché ha un comportamento diverso
//a seconda che l'ASL sia vuota o che il descrittore non sia presente nella ASL
int insertBlocked(int *key,pcb_t* p){

    semd_t *newSem ;

    if (semd_h == NULL){
        newSem = allocSemaphore() ;
        if (newSem == NULL){
            //non ci sono più semafori liberi
            return TRUE ;
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
            
            return FALSE ;
        }
    }
    else {
        //l'ASL non è vuota, quindi si cerca il semaforo con chiave key
        semd_t *sem_withKey = searchSemaphoreByKey(key) ;
        
        if (sem_withKey != NULL){
            p->p_semkey = key ;
            //se il semaforo già esisteva, non c'è bisogno di inizializzarlo
            insertProcQ(&(sem_withKey->s_procQ),p);
            return FALSE ;
        }
        else { //l'asl non è vuota ma il semaforo con quella chiave non c'è
            newSem = allocSemaphore() ;
            if (newSem == NULL){
                //non ci sono più semafori liberi
                return TRUE ;
            }
            else {
                p->p_semkey = key ;
                newSem->s_key = key ;
                list_add_tail(&(newSem->s_next),&(semd_h->s_next)) ;
                //il semaforo è stato appena aggiunto quindi bisogna inizializzare la lista dei pcb bloccati
                INIT_LIST_HEAD(&(newSem->s_procQ));
                insertProcQ(&(newSem->s_procQ),p);

                return FALSE ;
            }   
        }
    }
}

pcb_t* removeBlocked(int *key){

    pcb_t *pcb_removed ;
    semd_t *removeSem ;

    if ((removeSem = getSemd(key)) == NULL){
        //ritorna null sia se la ASL è vuota sia se il descrittore non esiste
        return NULL ;
    }
    else {
        pcb_removed = removeProcQ(&(removeSem->s_procQ)) ;
        //se la coda dei processi bloccati è vuota va liberato il semaforo
        if (emptyProcQ(&(removeSem->s_procQ))){
            freeSemaphore(removeSem);
        }
        return pcb_removed ;
    }
}

pcb_t* headBlocked(int *key){

    semd_t *sem ;

    if ((sem = getSemd(key)) == NULL){
        return NULL ;
    }
    else {
        return container_of(sem->s_procQ.next,pcb_t,p_next) ;
    }
}

pcb_t* outBlocked(pcb_t *p){

    pcb_t *pcb_removed ;
    semd_t *removeSem ;

    if ((removeSem = getSemd(p->p_semkey)) == NULL){
        //ritorna null sia se la ASL è vuota sia se il descrittore non esiste
        return NULL ;
    }
    else {
        //analogo a removeBlocked, ma su un pcb qualunque
        pcb_removed = outProcQ(&(removeSem->s_procQ),p) ;
        if (emptyProcQ(&(removeSem->s_procQ))){
            freeSemaphore(removeSem);
        }
        return pcb_removed ;
    }
}

void outChildBlocked(pcb_t *p) {
    pcb_t *child;
    list_for_each_entry(child, &p->p_child, p_sib) {
        outChildBlocked(child); // Chiamata ricorsiva
    }
    outBlocked(p); // Azione da portare avanti su tutti i parenti
}

/*MM funzione errata e un po' confusa; basta molto meno
void outChildBlocked(pcb_t *p){

    semd_t *removeSem ;

    if ((removeSem = getSemd(p->p_semkey)) != NULL){
        if (!emptyChild(p)){
            //p ha figli quindi si ricorre su di essi
            outChildBlocked(container_of(p->p_child.next,pcb_t,p_child));
        }
        else {
            //quando il sottoalbero è stato attraversato in tutta la sua profondità
            //si ricorre sui fratelli, finché il processo corrente è diverso dall'ultimo figlio
            //cioè quando il next del pcb corrente è diverso dal primo figlio (condizione testata) 
            if (&(container_of(p->p_sib.next,pcb_t,p_sib)->p_child) != container_of(p->p_sib.next,pcb_t,p_sib)->p_parent->p_child.next){
                outChildBlocked(container_of(p->p_sib.next,pcb_t,p_sib)) ;
            }
        }
        //la ricorsione garantisce che per ogni processo si controlli se esiste il suo semaforo
        //e se ha figli
        outProcQ(&(removeSem->s_procQ),p);
        if (emptyProcQ(&(removeSem->s_procQ))){
            freeSemaphore(removeSem);
        }
        outChild(p);
    }

}*/

#include "../header/handler.h"

void SYS_handler () {
    /*controlla nel registro Cause se l'eccezione sollevata
    è di tipo breakpoint o syscall, nel caso sia syscall
    fa un check sul numero, e chiama la funzione corrispondente,
    poi chiama lo scheduler
    negli altri casi (breakpoint, sys > 10, user mode)
    bisogna fare il passup*/
    state_t *old_process_state = (state_t*)SYSBK_OLDAREA ;
    int retval ;//valore ritornato da certe syscall

    unsigned int cause = old_process_state->cause ;
    unsigned int A1 = old_process_state->reg_a1;
    unsigned int A2 = old_process_state->reg_a2;
    unsigned int A3 = old_process_state->reg_a3;

    //distinzione tra syscall e breakpoint
    if (CAUSE_EXCCODE_GET(cause) == EXC_BREAKPOINT){
        //gestione eccezione breakpoint
        //con passup

    }
    else if (CAUSE_EXCCODE_GET(cause) == EXC_SYSCALL){

        switch(old_process_state->reg_a0){
            case GETCPUTIME:
                Get_CPU_Time((cpu_t*)A1, (cpu_t*)A2, (cpu_t*)A3);
                break;
            case CREATEPROCESS:
                retval = CreateProcess((state_t*)A1, A2,(void **)A3);
                if (retval == -1){
                    PANIC();
                }
                break;
            case TERMINATEPROCESS:
                retval = TerminateProcess((void**)A1);
                if (retval == -1){
                    PANIC();
                }
                break ;
            case VERHOGEN:
                Verhogen((int*)A1);
                break;
            case PASSEREN:
                Passeren((int*)A1);
                break;
            case WAITCLOCK:
                Wait_Clock();
                break;
            case WAITIO:
                Do_IO(A1,(unsigned int*)A2);
                break;
            case SETTUTOR:  
                Set_Tutor();
                break;
            case SPECPASSUP:
                Spec_Passup(A1, (state_t*)A2, (state_t*)A3);
                break;
            case GETPID:
                Get_pid_ppid((void **)A1, (void **)A2);
                break;
            default:
                break ;

        }
    }
    else {
        //esecuzione in user mode, passup
        PANIC();
    }
    //scheduling, passup
}



void whichDevice (int *IntlineNo, int *DevNo, int *termIO, unsigned int *reg) {

    //reg punta al campo command del registro, ovvero la seconda
    //word del registro
    unsigned int devAddrBase = (memaddr)reg - WORD_SIZE;
    
    //per ottenere l'indirizzo di un registro data la linea di interrupt
    //e il numero del device DEV_REGS_START+((LINENO-3)*DEV_REGBLOCK_SIZE) + (DEVNO*DEV_REG_SIZE)
    //per trovare la linea interrupt non serve l'offset del device number
    *IntlineNo = ((devAddrBase - DEV_REGS_START) / DEV_REGBLOCK_SIZE) + 3;

    *DevNo = (devAddrBase - DEV_REGS_START - ((*IntlineNo - 3) * DEV_REGBLOCK_SIZE)) / DEV_REG_SIZE;

    //  Se è un Terminale
    if (*IntlineNo == 7) {
        // registro dei terminali:
        //base + 0x0 recv status
        //base + 0x4 recv command
        //base + 0x8 transm status
        //base + 0xc transm command
        if (reg == (unsigned int*)(DEV_ADDRESS(*IntlineNo,*DevNo) + WORD_SIZE)){
            *termIO = RX ;
        } 
        else {
            *termIO = TX;
        } //    transm_command

    }
}

/* 
    Funzione ausiliaria per verificare se p sia
    o meno discendente di ancestor.
*/

int Descendant (pcb_t *p, pcb_t *ancestor) {
    
    pcb_t *tmp;
    int found = FALSE;
    tmp = p->p_parent;
    
    while (tmp != NULL && !found) {
        if (tmp == ancestor) 
            found = TRUE;
        else tmp = tmp->p_parent;
    }

    return found;
}

/* 
    Funzione ausiliaria per verificare se esista un tutor.
    
    -p è il processo dal cui genitore si inizia la ricerca del tutor.
*/

pcb_t* Tutor (pcb_t *p) {

    pcb_t *tmp;
    int found = FALSE;

    if (p != NULL) tmp = p->p_parent;
    else tmp = NULL;

    while (tmp != NULL && !found) {
        if (tmp->tutor == TRUE) found = TRUE;
        else tmp = tmp->p_parent;
    }

    return tmp;
}

/* 
    Funzione ausiliaria per il top dell'albero.
*/

pcb_t* TreeTop (pcb_t *p) {

    pcb_t *tmp;

    tmp = p;

    while (tmp != NULL && tmp->p_parent != NULL) tmp = tmp->p_parent;

    return tmp;
}

/* 
    Quando invocata, la SYS3 termina il processo
    identificato da pid (il proc. Corrente se pid == 0 o
    NULL) ma non la sua progenie. I processi figli
    vengono adottati dal primo antenato che sia
    marcato come “tutore” (o, nel caso non ce ne
    siano, dal processo in cima all’albero
    genealogico). Il processo da terminare deve
    essere un discendente del processo Corrente.
    – Restituisce 0 se ha successo, -1 per errore 
*/

//##################### SYS1 Get_CPU_Time #####################
/*  
–   Quando invocata, la SYS1 restituisce il tempo di
    esecuzione del processo che l’ha chiamata fino a
    quel momento, separato in tre variabili:
–   Il tempo usato dal processo come utente (user)
–   Il tempo usato dal processo come kernel (tempi
    di system call e interrupt relativi al processo)
–   Tempo totale trascorso dalla prima attivazione
    del processo.

    from slides:

    Si aggiunge un tracciamento del tempo di
    esecuzione di ogni processo, che lo scheduler
    deve accumulare in un nuovo campo della
    struttura pcb_t
*/

void Get_CPU_Time (cpu_t *user, cpu_t *kernel, cpu_t *wallclock) {

    if (user) 
        *user = running_process->user_time;

    if (kernel)
        *kernel = running_process->kernel_time;

    if (wallclock) {
        
        cpu_t currentTOD;
       
        currentTOD = TOD_HI;
        //currentTOD = currentTOD << 32;
        currentTOD += TOD_LO;

        *wallclock = currentTOD - running_process->first_scheduled;
    }

}

//#############################################################
//##################### SYS2 Create_Process ###################
/*  
    Questa system call crea un nuovo processo
    come figlio del chiamante. Il program counter, lo
    stack pointer, e lo stato sono indicati nello stato
    iniziale. Se la system call ha successo il valore di
    ritorno è zero altrimenti è -1. Se cpid != NULL e la
    chiamata ha successo *cpid contiene
    l’identificatore del processo figlio (indirizzo del
    PCB).
*/

int CreateProcess (state_t *statep, int priority, void ** cpid) {

    pcb_t *new_child = allocPcb();
    int success = -1;

    if(new_child != NULL) {
        if (running_process != NULL){
            //non è quindi il primo processo
            //di rikaya
            insertChild(running_process, new_child);    
        }
        state_copy(&new_child->p_s,statep);
        //new_child->p_s = *statep; usare funzione copia strutture
        new_child->priority = new_child->original_priority = priority;

        /* 
            ? prima attivazione != creazione
            new_child->created_time = TOD_HI;
            new_child->created_time <<= 32;
            new_child->created_time += TOD_LO; 
        */

        success = 0;

        if (cpid != NULL)
            *cpid = &new_child;

        insertProcQ(&ready_queue,new_child);
        ready_processes++ ;
    }
    
    return success;
}
//#############################################################
//##################### SYS3 Terminate_Process ################
/*
–   Quando invocata, la SYS3 termina il processo
    identificato da pid (il proc. Corrente se pid == 0 o
    NULL) ma non la sua progenie. I processi figli
    vengono adottati dal primo antenato che sia
    marcato come “tutore” (o, nel caso non ce ne
    siano, dal processo in cima all’albero
    genealogico). Il processo da terminare deve
    essere un discendente del processo Corrente.
–   Restituisce 0 se ha successo, -1 per errore
*/
int TerminateProcess (void ** pid) {
    
    int success = -1; //    Flag per il successo dell'operazione
    pcb_t **real_pid = (pcb_t**)pid ;
//  Se non viene specificato un pid:
    if (real_pid == 0 || real_pid == NULL) 
        *real_pid = running_process;
    
//  Se il processo da terminare è discendente del processo corrente
//o coincidono
    if (Descendant(*real_pid, running_process) || (*real_pid == running_process)) {
        
        //  Se il processo da terminare ha dei figli:
        if (!list_empty(&(*real_pid)->p_child)) {
            
            struct list_head* iter;
            pcb_t *first_child = container_of((*real_pid)->p_child.next,pcb_t,p_child);
            pcb_t *tutor = Tutor(*real_pid);
/*  
    Se non è stato possibile trovare un tutore, i processi orfani 
    verranno adottati dal processo in cima all'albero. 
*/
            if (tutor == NULL){
                tutor = TreeTop(*pid);
            }
            outChild(first_child); 
            insertChild(tutor, first_child);
/*  
    Tutti i fratelli del primogenito di pid 
    divengono figli di tutor
    il resto del sottoalbero di pid rimane invariato
*/
            list_for_each(iter, &(container_of(&(first_child->p_child),pcb_t,p_child)->p_sib)) {
                outChild(container_of(iter,pcb_t,p_sib));
                insertChild(tutor, container_of(iter,pcb_t,p_sib));
            }
        }
        
//  Se il processo è bloccato su un semaforo:
        if ((*real_pid)->p_semkey != NULL){
            outBlocked(*pid);
        }

//  Eliminiamo i legami di parentela di pid
        outChild(*pid);

//  Eliminare il processo dalla RQ, diminuire n. processi ready
        outProcQ(&ready_queue, *pid);
        ready_processes--;
        freePcb(*pid);  

        success = 0;
    }

    schedule();

    return success;
}
//#############################################################
//##################### SYS4 Verhogen #########################

/* 
    Operazione di rilascio su un semaforo. Il valore
    del semaforo è memorizzato nella variabile di
    tipo intero passata per indirizzo. L’indirizzo
    della variabile agisce da identificatore per il
    semaforo. 
*/
void Verhogen (int *semaddr){
    
//  Si incrementa il valore del semaforo.
    (*semaddr++);

/* 
    Se il valore del semaforo non è positivo,
    vuol dire che qualche altro processo è in 
    attesa di essere eseguito. 
*/
   if(*semaddr <= 0) {
       
//   Rimuoviamo p dalla coda:
       pcb_t *p = removeBlocked(semaddr);
       
       if (p != NULL) insertProcQ(&ready_queue, p); 
   }
    
}
//#############################################################
//##################### SYS5 Passeren #########################
/*     
    Operazione di richiesta di un semaforo. Il valore
    del semaforo è memorizzato nella variabile di
    tipo intero passata per indirizzo. L’indirizzo
    della variabile agisce da identificatore per il
    semaforo. 
*/

void Passeren (int *semaddr) {
    
    (*semaddr--);

/* 
    Se il valore diventa negativo:
    inseriamo il processo corrente nella
    coda del semaforo.
*/
    if (*semaddr < 0) {
/* 
    TODO: va interrotto (pausato) il processo corrente 
    salvando lo stato attuale del processore 
*/
        insertBlocked(semaddr, running_process);
    }
}
//#############################################################
//##################### SYS6 Wait_Clock #######################
/*  
–   Semplicemente, questa system call sospende il
    processo che la invoca fino al prossimo tick del
    clock di sistema (dopo 100 ms).
–   NB: se più processi possono sono sospesi a causa
    di questa system call, devono essere tutti
    riattivati al prossimo tick.
*/
void Wait_Clock () {

}
//#############################################################
//##################### SYS7 Do_IO ############################
/*
–   Questa system call attiva una operazione di I/O
    copiando parametro command nel campo
    comando del registro del dispositivo indicato
    come puntatore nel secondo argomento.
–   L’operazione è bloccante, quindi il chiamante
    viene sospeso sino alla conclusione del comando.
    Il valore ritornato è il contenuto del registro di
    status del dispositivo.
    from P_of_Op:
        • After an operation has begun on a device, 
        its device register “freezes” – becomes read-only – 
        and will not accept any other commands until 
        the operation completes.
    from slides:
        Utilizzate un semaforo per ogni device per
        “risvegliare” il processo che ha richiesto
        l’operazione di I/O con la SYS7 (due semafori per i
        terminali che sono device “doppi”).
        Notate che le linee di interrupt per i dispositivi di I/
        O (dalla linea 3 in poi) possono essere relative a
        istanze multiple, per cui bisogna distinguere quale
        di esse abbia effettivamente lanciato l’eccezione.
*/
int Do_IO (unsigned int command, unsigned int *reg) {
    // ? Il parametro reg è ptr al registro device o al campo comando?
            //campo comando
    int IntlineNo, DevNo, RX_TX; //    numero linea, device, receiver/transmitter terminale
    int status = -1;//   Stato del device alla fine dell'operazione
    termreg_t *term;//può essere terminale o altro device
    dtpreg_t *dev ;
//  Info sul device
    whichDevice(&IntlineNo, &DevNo, &RX_TX, reg);
    //device = *((memaddr *)DEV_REG_ADDR(3,DevNo));

//  Scrittura comando nel registro del device 
    *reg = command;

    if (IntlineNo < 7) {
        Passeren(&(devs[DevNo][IntlineNo-3]));
        dev = (dtpreg_t*)DEV_ADDRESS(IntlineNo,DevNo) ;
        status = dev->status;
    }
    else if (IntlineNo == 7) {
        Passeren(&(terms[DevNo][RX_TX]));
        term = (termreg_t*)DEV_ADDRESS(IntlineNo,DevNo) ;
        if (RX_TX == 0)
            status = term->recv_status;
        else if (RX_TX == 1)
            status = term->transm_status;
    }
    
    return (status);
   
}
//#############################################################
//##################### SYS8 Set_Tutor ########################
/*
–   Indica al kernel che il processo che la invoca deve
    agire da tutor per i processi discendenti che
    dovessero trovarsi orfani, e che quindi
    diventeranno suoi figli.
–   Si può implementare in diversi modi; per
    esempio, aggiungendo un campo nel pcb che
    marchi i tutor.
*/
void Set_Tutor () {
    running_process->tutor = TRUE ;
}
//#############################################################
//##################### SYS9 Spec_Passup ######################
/*
–   Questa chiamata registra quale handler di livello superiore
    debba essere attivato in caso di trap di Syscall/breakpoint
    (type=0), TLB (type=1) o Program trap (type=2). Il significato
    dei parametri old e new è lo stesso delle aree old e new
    gestite dal codice della ROM: quando avviene una trap da
    passare al gestore lo stato del processo che ha causato la
    trap viene posto nell’area old e viene caricato lo stato
    presente nell’area new. La system call deve essere
    richiamata una sola volta per tipo. Se la system call ha
    successo restituisce 0, altrimenti -1.
*/
int Spec_Passup (int type, state_t *old, state_t *new) {
    
    int success = -1;

    switch (type) {
        case SYS5_SYSBK: {
            //usare copia strutture
            state_copy(old,&running_process->p_s);
            //*old = running_process->p_s;
            state_copy(&running_process->p_s,new);
            //running_process->p_s = *new;
            success = 0;
        }
        break;
        
        case SYS5_TLB: {

        }
        break;

        case SYS5_PGMTRAP:
        
        break;
        
        default:
            break;
    }
    return success ;
}
//#############################################################
//##################### SYS10 Get_pid_ppid ####################
/*
–   Questa system call assegna il l’identificativo del
    processo corrente a *pid (se pid != NULL) e
    l’identificativo del processo genitore a *ppid (se
    ppid != NULL)
*/
void Get_pid_ppid (void ** pid, void ** ppid) {

    if (pid != NULL) 
        *pid = running_process;

    if(*ppid != NULL) 
        *ppid = running_process->p_parent;

}


void programtrap_handler(){
    //termprint("pg trap",0);
    PANIC();
}

void tlb_handler(){
    //termprint("tlb trap",0);
    PANIC();
}

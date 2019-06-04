#include "../header/interrupts.h"

/*  
    Int. Line  Device Class
    0          Inter-processor interrupts
    1          Processor Local Timer
    2          Bus (Interval Timer)
    3          Disk Devices
    4          Tape Devices
    5          Network (Ethernet) Devices
    6          Printer Devices
    7          Terminal Devices

    Word    Physical Address    Field Name
    0       0x1000.003C         Interrupt Line 3 Interrupting Devices Bit Map
    1       0x1000.0040         Interrupt Line 4 Interrupting Devices Bit Map
    2       0x1000.0044         Interrupt Line 5 Interrupting Devices Bit Map
    3       0x1000.0048         Interrupt Line 6 Interrupting Devices Bit Map
    4       0x1000.004C         Interrupt Line 7 Interrupting Devices Bit Map

•   Il nucleo deve gestire le linee di interrupt da 1 a 7.
•   Azioni che il nucleo deve svolgere:
    1.  Identificare la sorgente dell’interrupt
    -   Linea: registro Cause.IP
    -   Device sulla linea (>3): Interrupting Device Bit Map
    2.  Acknowledgment dell’interrupt
    -   Scrivere un comando di ack (linea >3) o un nuovo comando
        nel registro del device.
•   Interrupt con numero di linea più bassa hanno priorità più alta, e
    dovrebbero essere gestiti per primi.
*/


void interrupt_handler(){

    state_t *old_process_state = (state_t*)INTERRUPT_OLDAREA ;
    unsigned int cause = old_process_state->cause ;

    //se il processor local timer ha generato l'interrupt
    //si chiama lo scheduler con le priorità
    //aggiornate
    if (CAUSE_IP_GET(cause,INT_T_SLICE)){
        if (!emptyProcQ(&ready_queue)){
            pcb_t *ready_pcb ;
            list_for_each_entry(ready_pcb,&ready_queue,p_next){
                ready_pcb->priority++ ;
            }
        }
        if (running_process != NULL){
            insertProcqReady((old_process_state),running_process);
        }
        schedule(old_process_state);
    }
    else {
        pcb_t *p;   //  Processo da risvegliare
        dtpreg_t *dev;  //  Registro device
        termreg_t *term ;
        
        int bitmap, IntlineNo, DevNo;   // Bitmap Interrupt, numero linea e device
        
        for (IntlineNo = INT_LOWEST; IntlineNo < INT_LOWEST + DEV_USED_INTS; IntlineNo++) {
            if (CAUSE_IP_GET(cause,IntlineNo))
                break;
        }
        //bitmap del device che ha causato interrupt
        bitmap = *((memaddr*)INTR_CURRENT_BITMAP(IntlineNo));  

    //  Cerchiamo il primo bit a 1 per identificare il numero di device
        for (DevNo = 0; DevNo < DEV_PER_INT; DevNo++)
            if (bitmap  & (1U << DevNo)) 
                break;
        
        switch (IntlineNo) {
            case INT_TERMINAL: 
                term = (termreg_t *)DEV_ADDRESS(IntlineNo,DevNo);
                int TX_RX = -1;

                //  Transm o recv?
                if ((term->recv_status & 0xFF) == DEV_TRCV_S_CHARRECV) 
                    TX_RX = RX;
                else if((term->transm_status & 0xFF) == DEV_TTRS_S_CHARTRSM)
                    TX_RX = TX;
                else 
                    PANIC();

                
                if ((p = headBlocked(&terms[DevNo][TX_RX])) == NULL)
                    PANIC();

                //  V
                Verhogen(&terms[DevNo][TX_RX]);

                //  ACK
                if (TX_RX == RX) {
                    //a cosa serve questo?
                    p->p_s.reg_a1 = term->recv_status;
                    term->recv_command = DEV_C_ACK;
                    }
                else if (TX_RX == TX){
                    p->p_s.reg_a1 = term->transm_status;    
                    term->transm_command = DEV_C_ACK;
                }
                else{
                    PANIC();
                }
                break;
            case INT_DISK:
                break;
            case INT_TAPE:
                break;
            case INT_UNUSED:
                break; 
            case INT_PRINTER: 
                
                dev = (dtpreg_t *)DEV_ADDRESS(IntlineNo,DevNo);
                //Primo processo in attesa sul semaforo
                if ((p = headBlocked(&devs[DevNo][IntlineNo-3])) == NULL)
                    PANIC();

                //Copiamo lo stato del device nel registro A1
                p->p_s.reg_a1 = dev->status;

                //Incrementiamo il valore del semaforo
                Verhogen(&devs[DevNo][IntlineNo-3]);
                
                //Invio acknowledgement interrupt        
                dev->command = DEV_C_ACK;
            
                break;
            
            
            default:
                PANIC();
        }

        blocked_processes-- ;
        if(running_process != NULL){
            //eventuali calcoli sui tempi
            LDST(old_process_state);
        }
        else {
            schedule(NULL);
        } 

   
    }
}
    

 
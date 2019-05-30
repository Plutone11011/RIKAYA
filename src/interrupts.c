#include "../header/interrupts.h"
#include "../header/arch.h"
#include "../header/globals.h"
#include "../header/handler.h"

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

    //copia lo stato dell'area old nel processo running
    //per poterlo ripristinare in futuro
	state_copy(&(running_process->p_s),old_process_state);

    //se il processor local timer ha generato l'interrupt
    //si chiama lo scheduler con le priorità
    //aggiornate
    if (cause & CAUSE_IP(INT_T_SLICE)){
        if (!emptyProcQ(&ready_queue)){
            pcb_t *ready_pcb ;
            list_for_each_entry(ready_pcb,&ready_queue,p_next){
                ready_pcb->priority++ ;
            }
        }
        schedule();
    }/*
    else if (cause & CAUSE_IP(IL_TIMER)){
        //code
    }
    else {
        pcb_t *p;   //  Processo da risvegliare
        int *key;   // Ptr a valore semaforo
        devreg_t *dev_reg;  //  Registro device
        int bitmap, IntlineNo, DevNo;   // Bitmap Interrupt, numero linea e device
        
        for (IntlineNo = 3; IntlineNo < N_INTERRUPT_LINES; IntlineNo++) {
            if (cause & CAUSE_IP(IntlineNo))
                break;
        }

        bitmap = *((memaddr*)CDEV_BITMAP_ADDR(IntlineNo));  

    //  Cerchiamo il primo bit a 1 per identificare il numero di device
        for (DevNo = 0; DevNo < DEV_PER_INT; DevNo++)
            if (bitmap  & (1U << DevNo)) 
                break;
        
        dev_reg = *((memaddr *)DEV_REG_ADDR(3,DevNo));
        
        switch (IntlineNo) {
            case IL_DISK:
            case IL_TAPE:
            case IL_ETHERNET:
            case IL_PRINTER: {
                *key = (normal_devs[IntlineNo-3][DevNo]);
            //  Primo processo in attesa sul semaforo
                if ((p = headBlocked(key)) == NULL)
                    PANIC();

            //  Copiamo lo stato del device nel registro A1
                p->p_s.reg_a1 = dev_reg->dtp.status;

            //  Incrementiamo il valore del semaforo
                Verhogen(key);
                
            //  Invio acknowledgement interrupt        
                dev_reg->dtp.command = ACK;
            } 
            break;

            case IL_TERMINAL: {
                int TX_RX = -1;

                //  Transm o recv?
                if ((dev_reg->term.recv_status & 0xFF) == CHAR_RCVD) 
                    TX_RX = RX;
                else if((dev_reg->term.transm_status & 0xFF) == CHAR_TRSD)
                    TX_RX = TX;
                else 
                    PANIC();

                *key = (terminals[DevNo][TX_RX]);
                
                if ((p = headBlocked(key)) == NULL)
                    PANIC();

                //  V
                Verhogen(key);

                //  ACK
                if (TX_RX == RX) {
                    p->p_s.reg_a1 = dev_reg->term.recv_status;
                    dev_reg->term.recv_command = ACK;
                    }
                else if (TX_RX == TX){
                    p->p_s.reg_a1 = dev_reg->term.transm_status;    
                    dev_reg->term.transm_command = ACK;
                }
                else PANIC();
            } 
            break;
            
            default:
                PANIC();
            
        }

   
    }*/
}
    

 
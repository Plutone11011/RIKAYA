#include "print.h"


void print(char *str){
    /*trova l'indirizzo del registro del terminale 0*/
    volatile unsigned int term0Addr = DEV_ADDR_BASE + 512 ;
    /*punta ai campi TRANSM_STATUS e TRANSM_COMMAND del registro*/ 
    volatile unsigned int *term0AddrStatus = (volatile unsigned int*)(term0Addr + 0x8) ;
    volatile unsigned int *term0AddrCommand = (volatile unsigned int*)(term0Addr + 0xC) ;

    unsigned int i, tmp ;
    for (i = 0; i < *(str + i); i++){
        /*seleziona solo lo status byte*/
        do{
            tmp = (*term0AddrStatus) & 0xFF ;
        } while ((tmp != READY_STAT) && (tmp != TRANSMITTED_STAT)) ;
        /*shifta l'i-esimo carattere di un byte a sinistra perché il carattere da trasmettere
        va nel secondo byte meno significativo mentre il comando va nel primo*/
        tmp = ((unsigned int)(*(str + i)) << 8) | TRANSMIT_CHAR ;
        /*copia tmp nel registro per lanciare comando*/
        *term0AddrCommand = tmp ;
        
    }
}
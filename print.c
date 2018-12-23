#include "print.h"


int strlen(char *str){
    int len = 0 ;
    while (*str != 0){
        len++ ;
        *str++ ;
    }
    return len ;
}

 
void print(char *str){
    //trova l'indirizzo del registro del terminale 0
    unsigned int term0Addr = devAddrBase + 512 ;
    //punta ai campi TRANSM_STATUS e TRANSM_COMMAND del registro 
    unsigned int *term0AddrStatus = (int*)(term0Addr + 0x8) ;
    unsigned int *term0AddrCommand = (int*)(term0Addr + 0xC) ;

    unsigned int i, tmp ;
    for (i = 0; i < strlen(str); i++){
        //seleziona solo lo status byte
        do{
            tmp = (*term0AddrStatus) & 0xFF ;
        } while ((tmp != READY_STAT) && (tmp != TRANSMITTED_STAT)) ;
        //shifta l'i-esimo carattere di un byte a sinistra perché il carattere da trasmettere
        //va nel secondo byte meno significativo mentre il comando va nel primo
        tmp = ((unsigned int)(*(str + i)) << 8) | TRANSMIT_CHAR ;
        //copia tmp nel registro per lanciare comando
        *term0AddrCommand = tmp ;
        
    }
}
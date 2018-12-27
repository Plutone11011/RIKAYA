#define DEV_ADDR_BASE 0x10000050
#define READY_STAT 1 /*codici dello status byte*/
#define TRANSMITTED_STAT 5
#define BUSY_STAT 3
#define TRANSMIT_CHAR 2 /*codice di invio carattere per il campo trasm command*/


/*stampa str sul terminale 0*/
void print(char *str);
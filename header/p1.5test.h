#ifndef P1_5TEST_H
#define P1_5TEST_H

#include "interrupts.h"
#include "handler.h"
#include "scheduler.h"

#define STEPS 6
#define GANTT_SIZE 20

/*variabili globali*/
extern int test1_baton[STEPS + 1] ;
extern int test2_baton[STEPS + 1] ; 
extern int test3_baton[STEPS + 1] ;

extern char gantt_diagram[GANTT_SIZE] ;

extern char *toprint[] ;

/*I/O routine*/
typedef unsigned int devreg;

devreg termstat(memaddr *stataddr);

unsigned int termprint(char *str, unsigned int term);

void addokbuf(char *strp);
void adderrbuf(char *strp);

/*log del diagramma di gantt*/
void log_process_order(int process);

/*processi di rikaya fase 1.5*/
void test1();
void test2();
void test3();

#endif
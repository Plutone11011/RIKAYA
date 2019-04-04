#ifndef P1_5TEST_H
#define P1_5TEST_H

#include "const.h"
#include "listx.h"
#include "libumps.h"
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "cp0.h"
#include "interrupts.h"
#include "handler.h"

#define TRANSMITTED 5
#define TRANSTATUS 2
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16
#define READY 1
#define DEVREGLEN 4
#define TRANCOMMAND 3
#define BUSY 3

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)
#define FRAMESIZE 4096
/*old e new area*/
#define INTERRUPT_OLDAREA RAMBASE
#define INTERRUPT_NEWAREA 0x2000008C
#define TLB_OLDAREA 0x20000118
#define TLB_NEWAREA 0x200001A4
#define PROGRAMTRAP_OLDAREA 0x20000230
#define PROGRAMTRAP_NEWAREA 0x200002BC
#define SYSCALL_OLDAREA 0x20000348
#define SYSCALL_NEWAREA 0x200003D4

#define SYS3 3
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
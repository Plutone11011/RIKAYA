#ifndef GLOBALS_H
#define GLOBALS_H

#include "types_rikaya.h"

#define TRANSMITTED 5
#define RECEIVED 5
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
#define TIMESLICE 500000U

extern struct list_head ready_queue ;
extern int ready_processes ;
extern pcb_t *running_process ;

#endif 

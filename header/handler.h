#ifndef HANDLER_H
#define HANDLER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "cp0.h"
#include "p1.5test.h" 

#define SYS1 1
#define SYS2 2
#define SYS3 3
#define SYS4 4
#define SYS5 5 
#define SYS6 6 
#define SYS7 7 
#define SYS8 8
#define SYS9 9
#define SYS10 10

#define SYS_BP 0
#define TLB 1
#define PGTRP 2


void programtrap_handler();
void tlb_handler();
void SYS_handler();

void Verhogen(int *semaddr);
void Passeren(int *semaddr);

#endif
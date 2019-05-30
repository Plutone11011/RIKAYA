#ifndef HANDLER_H
#define HANDLER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "pcb.h"
#include "asl.h"


void programtrap_handler();
void tlb_handler();
void SYS_handler();

void Verhogen(int *semaddr);
void Passeren(int *semaddr);
int createProcess(state_t *statep, int priority, void **cpid);

#endif
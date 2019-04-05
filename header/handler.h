#ifndef HANDLER_H
#define HANDLER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "cp0.h" 

void programtrap_handler();
void tlb_handler();
void syscall_handler();

#endif
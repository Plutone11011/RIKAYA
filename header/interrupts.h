#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "cp0.h"
#include "p1.5test.h"
#include "scheduler.h"

void interrupt_handler();

#endif
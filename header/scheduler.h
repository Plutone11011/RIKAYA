#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "arch.h"
#include "pcb.h"
#include "asl.h"
#include "cp0.h"
#include "p1.5test.h"
#include "utils.h"


void schedule();
void state_copy(state_t *dest, state_t *src);

#endif
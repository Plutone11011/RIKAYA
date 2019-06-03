#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"


void schedule(state_t *old);
void insertProcqReady(state_t *old, pcb_t *proc);

#endif
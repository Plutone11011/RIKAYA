#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"


void schedule(state_t *old);
//memorizza il tempo di inizio esecuzione del processo
//per poter calcolare user time in futuro
void set_lastScheduled(pcb_t *scheduled);
void insertProcqReady(state_t *old, pcb_t *proc);
void update_usertime(pcb_t *userproc);
void update_kerneltime(pcb_t *kernelproc);
void set_timer();

#endif
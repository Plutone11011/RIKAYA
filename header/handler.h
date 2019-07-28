#ifndef HANDLER_H
#define HANDLER_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "pcb.h"
#include "asl.h"
#include "scheduler.h"


void programtrap_handler();
void tlb_handler();
void SYS_handler();

void Get_CPU_Time (cpu_t *user, cpu_t *kernel, cpu_t *wallclock);
void Verhogen(int *semaddr);
void Passeren(int *semaddr);
int CreateProcess(state_t *statep, int priority, void **cpid);
int TerminateProcess (void ** pid);
void Wait_Clock();
int Do_IO (unsigned int command, unsigned int *reg, unsigned int tx_rx);
void Set_Tutor ();
int Spec_Passup (int type, state_t *old, state_t *new);
void Get_pid_ppid (void ** pid, void ** ppid);
static int specifiedHandler (int type, state_t* s);

#endif
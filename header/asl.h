#ifndef ASL_H
#define ASL_H

#include "const.h"
#include "globals.h"
#include "types_rikaya.h"
#include "pcb.h"

extern semd_t semd_table[NUM_SEMAPHORES] ;

extern semd_t *semdFree_h ; //sentinella lista semafori liberi
extern semd_t *semd_h ; //sentinella lista semafori attivi

/* ASL handling functions */
semd_t* getSemd(int *key);
void initASL();

int insertBlocked(int *key,pcb_t* p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *key);
void outChildBlocked(pcb_t *p);

#endif

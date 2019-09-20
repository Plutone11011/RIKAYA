#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "const.h"
#include "globals.h"
#include "libumps.h"
#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include "handler.h"

/*
#define RX 0
#define TX 1
#define CHAR_RCVD 5
#define CHAR_TRSD 5

//  48 Semafori
//  Linee 3-4-5-6
int normal_devs[N_IL - 4][DEV_PER_INT];
//  Linea 7 (2 per device)
int terminals[DEV_PER_INT][2];
*/
void interrupt_handler();

#endif
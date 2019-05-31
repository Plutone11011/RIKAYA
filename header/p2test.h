#ifndef P2TEST_H
#define P2TEST_H

#include "scheduler.h"
#include "handler.h"
#include "interrupts.h"

typedef unsigned int devregtr;
typedef unsigned int pid_t;

void print(char *msg);

void test();
void p2();
void p3();
void p4();
void p5prog();
void p5mm();
void p5();
void p5a();
void p5b();
void p5sys();
void p6();
void p7();
void p7a();
void p8root();
void child1();
void intermediate();
void child2();
void p8leaf();
void curiousleaf();

#endif
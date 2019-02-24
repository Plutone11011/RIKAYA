# RIKAYA
Operating Systems project based on uMPS2 machine emulator

## GETTING STARTED

Read these instructions to use RIKAYA

### Prerequisites
Install the uMPS2 emulator:
* for Arch Linux users, follow the instructions here http://so.v2.cs.unibo.it/wiki/index.php?title=Installare_%CE%BCMPS_su_Arch_Linux
* for Debian based o.s. users, follow the instructions here https://github.com/Zimm1/umps-apt-installer

or just compile from source https://github.com/tjonjic/umps

### Compiling
Run make to compile, link and get a .core file named kernel.core.umps

### Layout
/src :
* p1test_rikaya_v0.c: testing functions
* asl.c: implementation of an active semaphore list to handle blocked processes.
* pcb.c: implementation of processes' trees and queues, and a list of free processes.

/header:
* listx.h: an API for circular, bidirectional lists. Used to implement the ASL, the queue of blocked processes on a semaphore
and the list of siblings in a tree.
* types.h: utility types definitions, for device registers and processor state.
* types_rikaya.h: process control block and semaphore descriptor data structures.
* const.h: global constant and macro definitions.
* libumps.h: external declarations for uMPS library module. 

### License
This project is licensed under the GPLv2 License - see the LICENSE.md file for details


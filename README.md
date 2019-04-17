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
* p1.5test.c: funzioni di testing
* asl.c: implementazione di una active semaphore list
* pcb.c: implementazione delle code e alberi di processo
* globals.c: dichiara e inizializza le variabili globali del kernel
* scheduler.c: sceglie il processo della coda ready da mandare in esecuzione, inserendo il precedente e aggiornando le priorità
* interrupts.c: gestione degli interrupt sulle linee 1-7. Aging dei processi e copia dello stato del processo
* handler.c: gestione delle trap tlb, programtrap e system call
* main.c: inizializza le aree NEW e i processi di RIKAYA, con le strutture dati appropriate

/header:
* listx.h: an API for circular, bidirectional lists. Used to implement the ASL, the queue of blocked processes on a semaphore
and the list of siblings in a tree.
* types.h: utility types definitions, for device registers and processor state.
* types_rikaya.h: process control block and semaphore descriptor data structures.
* const.h: global constant and macro definitions.
* libumps.h: external declarations for uMPS library module. 

### License
This project is licensed under the GPLv2 License - see the LICENSE.md file for details


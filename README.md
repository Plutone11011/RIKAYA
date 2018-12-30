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

### Authors

* Lorenzo Borelli 0000789622

### Layout
The RIKAYA project contains a print source file, and its header, which defines a print of a null-terminated string on terminal 0. The main tests the print. The print_config file is a suitable machine configuration.

### License
This project is licensed under the GPLv2 License - see the LICENSE.md file for details


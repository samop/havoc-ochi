/* EDIT: Explain what this source file does
   Copyright (C) 2011 Samo Penic, Miha Fosnaric, Ales Berkopec.

   This file is part of Ochi, an optical recognition backend of HAvOc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#ifndef _H_DAEMON
#define _H_DAEMON
#include<stdarg.h>
#include<stdlib.h>
#include<stdio.h>
extern int daemon_flag;
int dfprintf(FILE *fd, char *fmt, ...);
void signal_handler(int sig);
int daemonize();
int lock_file();
int process_scans();
void print_version (void);
void print_usage (void);



#endif

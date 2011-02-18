#ifndef _H_DAEMON
#define _H_DAEMON
#include<stdarg.h>
#include<stdlib.h>
#include<stdio.h>
int quiet;
int dfprintf(FILE *fd, char *fmt, ...);
#endif

#include "daemon.h"

/* outputs string depending on quiet mode */
int dfprintf(FILE *fd, char *fmt, ...){
if(quiet) return 0;
    va_list ap;
    va_start(ap,fmt);
vfprintf(fd, fmt, ap); /* Call vfprintf */
va_end(ap); /* Cleanup the va_list */
return 0;
}


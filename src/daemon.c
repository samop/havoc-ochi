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


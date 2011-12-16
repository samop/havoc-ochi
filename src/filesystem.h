/* filesystem.c: prototypes for filesystem.c.

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


#ifndef _H_FILESYSTEM
#define _H_FILESYSTEM
#include <glob.h>
#include <leptonica/allheaders.h>
/* global variables */
char *scan_dir;
char *debug_dir;
char *output_dir;
char *processed_dir;
char *database_name;
char *database_user;
int debug;
int use_database;
int daemon_flag;
int extract_flag;
PIX *PIXmark1;

glob_t *list_files(void);
void readconfig();

#endif

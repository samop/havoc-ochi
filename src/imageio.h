/* imageio.h: prototypes for functions in imageio.c.

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


#ifndef __H_IMAGE_IO
#define __H_IMAGE_IO
#include "bar_process.h"
#include "sid_process.h"
#include "ans_process.h"
#include "filesystem.h"

int saveimage(PIX *pixd, char *fileout);
PIX *loadimage(char *filename);
int fileformat(char *filename);
int writerezfile(PIX *pixs,char *source_fname,ANS *ans, BAR *bar, SID *sid);
#endif

/* sid_process.h: prototypes and definition of SID data structure.

   Copyright (C) 2008-2011 Samo Penic, Miha Fosnaric, Ales Berkopec.

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


#ifndef _SID_PROCESS_H
#define _SID_PROCESS_H

#define NETWORK_FILE "network.net"
#define SID_LENGTH 8
#define SID_SPACE 20
#define SID_INTERSPACE 5
#define SID_CORNER_X_START 4/6
#define SID_CORNER_X_END 2/6
#define SID_CORNER_Y_END 1/11

typedef struct {
    /*corrected image of Student ID*/
    PIX *img;
    /*location in original image*/
    int x,y,w,h;
    /*character value for recognised SID*/
    char *sid;
    float *certainty;
} SID;

SID *getSID(PIX *pixs);
char *chopSID(SID *sid, int n, char *num, int xoff, int yoff);
void decodeSID(SID *sid);
void decodeMissSID(SID *sid);
char get_sid_from_character_array(char cdata[32][32], float *certain);
SID *locateSID(PIX *pixs);
void sidOldEnhance(SID *sid);
void sidEnhance(SID *sid);
void pixRemoveStub(PIX **pixt0b); /* TODO */
void sidDestroy(SID **sid);
#endif

/* bar_process.h: defines prototypes and BAR data structure.

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


#ifndef _BAR_PROCESS_H
#define _BAR_PROCESS_H

/* With these numbers we clip the part of the image where the barcode risides */

#define BAR_CORNER_X_START   0
#define BAR_CORNER_X_END 0.3333
#define BAR_CORNER_Y_START 0
#define BAR_CORNER_Y_END 0.1


typedef struct {
    /* Image of Barcode */
    PIX *img;
    /* location in original image */
    int x,y,w,h;
    /* character value for recognised Barcode */
    char *barcode;
} BAR;

BAR *getCode (PIX *pixs);
BAR *locateCode(PIX *pixs);
void decodeCode(BAR *code);
void img2yuv800(PIX *pixs, void **yuv);
PIX *yuv8002img(uint8_t *yuv800, int w, int h);
void barDestroy(BAR **barcode);
#endif

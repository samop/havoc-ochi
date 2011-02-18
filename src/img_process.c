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


#include <leptonica/allheaders.h>
#include "img_process.h"

PIX *lineremoval(PIX *pixs){
    PIX *pixd = pixMorphCompSequence(pixs, "o2.2", 0); /* Odstranimo pikice o2.2*/
    return pixd;
}

int deskew(PIX **pixd, PIX **pixs){
    l_float32 angle, deg2rad = 3.1415926535 / 180.;
    PIX *pixd1, *pixs1;
    pixd1=pixFindSkewAndDeskew(*pixd, DESKEW_REDUCTION, &angle, NULL);
    pixs1=pixRotateShear(*pixs, 0, 0, deg2rad*angle, L_BRING_IN_WHITE);
/*this is required to avoid memory leaks */
    pixDestroy(pixd);
    pixDestroy(pixs);
    *pixs=pixs1;
    *pixd=pixd1;
}


PIX *repair_scanned_image(PIX **pixs){
    PIX *pixd= lineremoval(*pixs);
    deskew(&pixd, pixs);
    return pixd;
}

PIX *make1bpp(PIX **pixs){

return NULL;
}


/* returns clipped image from original */

PIX *clip_image(PIX *pixs, int Xs, int Ys, int W, int H){
    BOX *clip=boxCreate(Xs,Ys, W, H);
    PIX *retval = pixClipRectangle(pixs, clip, NULL);
    boxDestroy(&clip);
    return retval;
}



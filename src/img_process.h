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


#ifndef _IMG_PROCESS_H
#define _IMG_PROCESS_H


#define   DESKEW_REDUCTION      4      /* 1, 2 or 4 */


PIX *lineremoval(PIX *pixs);
int deskew(PIX **pixd, PIX **pixs);
PIX *repair_scanned_image(PIX **pixs);
PIX *make1bpp(PIX **pixs);
PIX *clip_image(PIX *pixs, int Xs, int Ys, int W, int H);
#endif

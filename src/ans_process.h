/* ans_process.h: header file for ans_process.c.

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

#ifndef _H_ANS_PROCESS
#define _H_ANS_PROCESS


/* to be removed */
//these numbers are used to enlarge area around answers.... should be given
//relative or... completely removed from the procedure.
#define XOFF -30
#define YOFF -15
#define WIDTH 40
#define HEIGHT 40



typedef struct {
    int mark_type;
    float x;
    float y;
    float w;
    float h;
} MARK;

typedef struct {
    int n;
    MARK *mark;
} ANS_MARK;

typedef struct {
    ANS_MARK *up;
    ANS_MARK *right;
    char *ans_string;
    int *mtx;
} ANS;

ANS *getanswer(PIX *pixs);
void debugsquare(PIX *pixs); /* TODO */
int findanswer(PIX *pixd, ANS *ans);
PIX* answerenhance(PIX *pixs); /* TODO */
int generateobsoletestring(ANS *ans);
ANS_MARK *locateupmarker(PIX *pixs);
ANS_MARK *locaterightmarker(PIX *pixs);
int locate(PIX *pixwhole, int sx, int sy, int ex, int ey, PIX *pixm, ANS_MARK *mark);
int odstranimark(int i, int *n, int *x, int *y, int *w, int *h); /* TODO */
int sortmark(ANS_MARK *mark, int sorttype);
int ans_markAlloc(ANS_MARK *mark, int n_new);
void ans_markDestroy(ANS_MARK **mark);
void ansDestroy(ANS **ans);

#endif

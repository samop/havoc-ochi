/* ans_process.c: functions that locates markers and finds answers in the
   scanned page. Returns matrix of answered questions and string of answers.

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
#include<stdlib.h>
#include<string.h>
#include "sid_process.h"
#include "img_process.h"
#include "ans_process.h"
#include "filesystem.h"
#include "localization.h"
#include "daemon.h"

/* high level function to be called when you don't want to mess with internals*/
ANS *getanswer(PIX *pixs){
    ANS *ans=(ANS *)calloc(1,sizeof(ANS));
    ans->up=locateupmarker(pixs);
    ans->right=locaterightmarker(pixs);
    sortmark(ans->up,1);
    sortmark(ans->right,2);
    findanswer(pixs,ans);
    generateobsoletestring(ans);
    printf("%s\n",ans->ans_string);
    return ans;
}

/* TODO: not finished. To be used for debugging */
void debugsquare(PIX *pixs){
    ANS_MARK *mark=calloc(1,sizeof(ANS_MARK));
    //locate(pixs, pixs, PIXmark1, mark);
    
 //     pixRenderBox(pixs,boxa,3,L_SET_PIXELS);
    saveimage(pixs,"/tmp/sq_debug.png");
    ans_markDestroy(&mark);
}

int findanswer(PIX *pixd, ANS *ans){
/* odgovori se nahajajo na lokacijah, ki so presecisca x+w/2 in y+h/2 */
    ANS_MARK *up=ans->up;
    ANS_MARK *right=ans->right;
    int  w = pixGetWidth(pixd);
    int h = pixGetHeight(pixd);



//BOX *boxa;
PIX *pixclip;
int i,j, numpix, numpixall;
float ratio;

    ans->mtx=(int *)malloc(up->n*right->n*sizeof(int));

for(i=0;i<right->n;i++){
    for(j=0;j<up->n;j++){
        pixclip=clip_image(pixd,up->mark[j].x*w+XOFF,right->mark[i].y*h+YOFF,up->mark[j].w*w+WIDTH,right->mark[i].h*h+HEIGHT );
        pixCountPixels(pixclip, &numpix, NULL);
        numpixall=pixGetWidth(pixclip)*pixGetHeight(pixclip);
        ratio=(float)numpix/(float)numpixall*100;
        if(ratio>35){
            ans->mtx[i*up->n+j]=1;
        }
        else {
            ans->mtx[i*up->n+j]=0;
        }
//      printf("(D) Naloga %i odgovor %i je pobarvan na %3.2f\n",i,j,ratio);
/*      pixRenderBox(pixd,boxa,3,L_SET_PIXELS); */
       
       pixDestroy(&pixclip);
    }
}

return 0;
}



/* preliminary answer enhance function */
PIX* answerenhance(PIX *pixs){
  PIX *pixt1,*pixt0, *pixt0a, *pixt0b;
    
  pixt0b=pixMorphSequence(pixs, "o2.2", 0); 

    pixt0b = pixMorphCompSequence(pixt0b, "c5.5", 0); /* Tukaj nastavimo 
kdaj so vertikalni deli nepovezani */
 //       pixDestroy(&pixt0b);
    pixt0b=pixMorphSequence(pixt0b, "e10.10", 0); 
 //   pixt0b = pixMorphCompSequence(pixt0b, "c2.10", 0);
        
    saveimage(pixt0b, "odgovori.png");

   //     pixDestroy(&pixt0a);
        pixDestroy(&pixt0b);
        return NULL;



}


int generateobsoletestring(ANS *ans){
    int i;
    int j;
    char tmp[25];
    ans->ans_string=(char *)calloc(255,sizeof(char));
    for(i=0;i<ans->right->n;i++){
        for(j=0;j<ans->up->n;j++){
            if(ans->mtx[i*ans->up->n+j]){
                sprintf(tmp,",%i%c",i+1,j+'a');
                ans->ans_string=strcat(ans->ans_string,tmp);
            }
        }
    }

return 0;
}

/*locate upper markers for answers*/
ANS_MARK *locateupmarker(PIX *pixs){
    int i;
    ANS_MARK *mark_up=calloc(1,sizeof(ANS_MARK));
    /* use just a part of image, where markers are expected */
    locate(pixs,0, 0,pixGetWidth(pixs), pixGetHeight(pixs)/15,PIXmark1,mark_up);
    dfprintf(stdout,_("UP: Found %i markers:\n"),mark_up->n);    
    for(i=0;i<mark_up->n;i++){
        dfprintf(stdout,_("location %f, %f, %f, %f\n"), mark_up->mark[i].x,
                mark_up->mark[i].y, mark_up->mark[i].w, mark_up->mark[i].h);
    }
    return mark_up;
}

/*locate right markers for answers*/
ANS_MARK *locaterightmarker(PIX *pixs){

    int i;
    ANS_MARK *mark_right=calloc(1,sizeof(ANS_MARK));
    /* use just a part of image, where markers are expected */
    locate(pixs,pixGetWidth(pixs)*9/10, 0,pixGetWidth(pixs)/10, pixGetHeight(pixs),PIXmark1,mark_right);
    dfprintf(stdout,_("RIGHT: Found %i markers:\n"),mark_right->n);    
    for(i=0;i<mark_right->n;i++){
        dfprintf(stdout,_("location %f, %f, %f, %f\n"), mark_right->mark[i].x,
                mark_right->mark[i].y, mark_right->mark[i].w, mark_right->mark[i].h);
 
    }
    return mark_right;
}


/* Function that locates markers (whether vertical or horizontal) in the given
 * image excerpt */
int locate(PIX *pixwhole, int sx,int sy, int ex, int ey, PIX *pixm, ANS_MARK *mark){
    PIX *pixs=clip_image(pixwhole,sx,sy,ex,ey);
    /* settings for pixGenerateSelWithRuns */
    l_int32  NumHorLines = 10;
    l_int32  NumVertLines = 8;
    l_int32  MinRunlength = 1;
    l_uint32  HitColor = 0xff880000;
    l_uint32  MissColor = 0x00ff8800;
    int      w, h;
    int i, n, stMark;
    BOX         *box, *boxe;
    BOXA        *boxa1;
    SEL         *sel, *sel_2h;
    PIX *pixt2, *pixpe, *pixhmt;
    PIX *pixt3;
    int oldn;
    w = pixGetWidth(pixwhole);
    h = pixGetHeight(pixwhole);

/* TODO: Part of creating sel_2h could be a separate function. It will speed up
 * the process a bit and simplify function a lot. */

/*pixpe je vzorec nasega elementa, ki ga iscemo v image fajlu &pixs */
    sel = pixGenerateSelWithRuns(pixm, NumHorLines, NumVertLines, 0,
                                MinRunlength, 7, 7, 0, 0, &pixpe);


    pixhmt = pixHMT(NULL, pixs, sel);
    /* small erosion to remove noise; typically not necessary if
     * there are enough elements in the Sel */
    sel_2h = selCreateBrick(1, 2, 0, 0, SEL_HIT);
    pixt2 = pixErode(NULL, pixhmt, sel_2h);

    pixt3=pixMorphSequence(pixt2, "d5.5", 0); 
    /* TODO: test if you need to remove some noise??? */
    saveimage(pixt3,"/tmp/dbg-square.png");
    boxa1 = pixConnCompBB(pixt3, 8);
    n = boxaGetCount(boxa1);
    if(n>0){
    /* allocate space for marker data */
        oldn=ans_markAlloc(mark, n);
        for(i=oldn;i<mark->n;i++){
            box=boxaGetBox(boxa1,i-oldn,L_COPY);
                mark->mark[i].x=(float)(box->x+sx)/w;
                mark->mark[i].y=(float)(box->y+sy)/h;
                mark->mark[i].w=(float)box->w/w;
                mark->mark[i].h=(float)box->h/h;
                mark->mark[i].mark_type=0;
            boxDestroy(&box);
        }
    }
    pixDestroy(&pixt2);
    pixDestroy(&pixt3);
    pixDestroy(&pixpe);
    pixDestroy(&pixhmt);
    selDestroy(&sel);
    selDestroy(&sel_2h);
    boxaDestroy(&boxa1);
    pixDestroy(&pixs);
    return n;
}

int odstranimark(int i, int *n, int *x, int *y, int *w, int *h){

    int j;
    for(j=i;j<(*n)-1;j++){
            x[j]=x[j+1];
            y[j]=y[j+1];
            w[j]=w[j+1];
            h[j]=h[j+1];
    }
    *n=(*n)-1;

    return 0;

}

/* TODO: Napisi boljsi sort algoritem! */
int sortmark(ANS_MARK *mark, int sorttype){

    /* sorttype 1, po x
     * sorttype 2, po y
     */

    int i,j,s=0;
    MARK tmark;
    for(j=0; j<mark->n  ; j++){
    for(i=0; i<mark->n-1; i++){
        if((mark->mark[i].x > mark->mark[i+1].x && sorttype==1) || ((mark->mark[i].y>mark->mark[i+1].y) && sorttype==2)){
            memcpy(&tmark,&mark->mark[i],sizeof(MARK));
            memcpy(&mark->mark[i],&mark->mark[i+1],sizeof(MARK));
            memcpy(&mark->mark[i+1],&tmark,sizeof(MARK));
            s=1;
        }
    }
    if(s==0) break;
    s=0;
    }

    return 0;
}

/* function that allocates memory for n_new new markers into current ANS_MARK
 * datastructure. Data structure must be initialized with calloc or pointers
 * must be set to null if initialized with malloc */
int ans_markAlloc(ANS_MARK *mark, int n_new){
    int oldmark=mark->n;
    mark->n+=n_new;
    mark->mark=(MARK *)realloc(mark->mark, mark->n*sizeof(MARK));
    return oldmark;
}

/* cleans memory of the datastructure usage */
void ans_markDestroy(ANS_MARK **mark){
    if(*mark!=NULL){
        if((*mark)->mark!=NULL){
            free((*mark)->mark);
        }
    free(*mark);
    }
}

void ansDestroy(ANS **ans){
    if(*ans!=NULL){
        ans_markDestroy(&(*ans)->up);
        ans_markDestroy(&(*ans)->right);
        if((*ans)->mtx!=NULL){
            free((*ans)->mtx);
        }
        if((*ans)->ans_string!=NULL){
            free((*ans)->ans_string);
        }
        free(*ans);
    }
}

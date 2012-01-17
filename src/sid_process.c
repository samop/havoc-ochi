/* sid_process.c: finds and decodes student id (sid).

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

/* TODO: Hey.. an idea. Try to fnad unconested stubs in skeletonized image to
 * remove stubs in final image that makes problems in reconition e.g. 9 or 4, 6
 * or 5 etc.... */
#include <leptonica/allheaders.h>
#include<stdlib.h>
#include "ocrlib/lwneuralnet.h"
#include "ocrlib/ocr.h"
#include "sid_process.h"
#include "img_process.h"
#include "filesystem.h"
#include "daemon.h"
#include "localization.h"

/* a wrapper script to collect all functions in this file to get student id
 * number in a string and returned in a data structure */
SID *getSID(PIX *pixs){
    SID *sid=locateSID(pixs);
    PIX *pixt=pixCopy(NULL,sid->img);
    PIX *pixt2;
//TEST  sidEnhance(sid); /* use Ochi v4 enhancement procedure */
    decodeSID(sid);
    /* try another way, only if there is x in SID */
    if(index(sid->sid,'x')!=NULL){
        dfprintf(stdout,_("SID not recognised (%s), doing different processing and trying again!\n"),sid->sid);
        pixt2=sid->img;
        sid->img=pixt;
        sidOldEnhance(sid);
        decodeMissSID(sid);
        sid->img=pixt2;
    }
    pixDestroy(&pixt);
    return sid;
}

/*get a character array of 32x32 of a numeral of a SID*/
char *chopSID(SID *sid, int n, char *num, int xoff, int yoff){
    PIX *pix1,*pix2;
    int j,k, pixl,cw=(int)pixGetWidth(sid->img)/SID_LENGTH, ch=pixGetHeight(sid->img);
    pix1=clip_image(sid->img,n*cw+SID_INTERSPACE/2+xoff,0+yoff,cw,ch); /* get real... can I make it more obscure? */
    pix2=pixScale(pix1,(float) 32/pixGetWidth(pix1),(float) 32/pixGetHeight(pix1));
    for(j=0;j<pixGetHeight(pix2);j++){
        for(k=0;k<pixGetWidth(pix2);k++){
            pixGetPixel(pix2,k,j,&pixl);
            num[j+k*32]=pixl; /* funny transformation of rows/columns. TODO: works with intels, but others??? */
        }
    }
    char fname[1024];
    sprintf(fname,"%s/vpisna%d.png",debug_dir,n);
    saveimage(pix2,fname);
    pixDestroy(&pix1);
    pixDestroy(&pix2);
    return num;
}

/* chops and decodes digit by digit */
void decodeSID(SID *sid){
    int i,rv,j,k;
    char *num=(char *)malloc(32*32*sizeof(char));
    float *cert=(float *)malloc(SID_LENGTH*sizeof(float));
    char *sid1=(char *)malloc((SID_LENGTH+1)*sizeof(char));
    for(i=0;i<SID_LENGTH;i++){
        num=chopSID(sid,i,num,0,0);
        rv=get_sid_from_character_array(num, &cert[i]);
        if(rv<0) {
            dfprintf(stdout,_("Going int XY correction mode!\n"));
            sid1[i]='x';
            for(j=-6;j<=6;j+=2){
                for(k=-2;k<=2;k+=1){
                    num=chopSID(sid,i,num,j,k);
                    rv=get_sid_from_character_array(num, &cert[i]);
                    if(rv>=0){ 
                        sid1[i]='0'+rv;
                        break;
                    }
                    if(rv>=0){
                        break;
                    }
                }       
            }
        }
        else sid1[i]='0'+rv;
    }
    sid1[SID_LENGTH]=0;
    sid->sid=sid1;
    sid->certainty=cert;
//if we requested that numerals must me extracted
	char fname[255];
	char target_name[255];
    if(extract_flag==1){
	for(i=0;i<SID_LENGTH;i++){
		if(sid->certainty[i] > 0.8){
			if(i==0 || i==1 || i==4) continue;
			printf("Saving...\n");
			//cp /tmp/vpisnaI.png v numerals/I/rand.png
    			sprintf(fname,"%s/vpisna%d.png",debug_dir,i);
    			sprintf(target_name,"%s/numerals/%d/vpisna%d.png",debug_dir,sid->sid[i]-'0',(int)(rand()*1000000));
			printf("Saving (%s -> %s)...\n",fname, target_name);
			rename(fname, target_name);
		}
	}
    }
    free(num);

}

/* chops and decodes digit by digit of missing sid characters */
void decodeMissSID(SID *sid){
    int i,rv;
    char *num=(char *)malloc(32*32*sizeof(char));
    for(i=0;i<SID_LENGTH;i++){
        if(sid->sid[i]=='x'){
            num=chopSID(sid,i,num,0,0);
            rv=get_sid_from_character_array(num, &sid->certainty[i]);
            if(rv<0) sid->sid[i]='x';
            else sid->sid[i]='0'+rv;
        }
    }
    free(num);
}

/* Runs lwneuralnet recognition. Functions that are used are actually from that
 * project. */
char get_sid_from_character_array(char cdata[32][32], float *certain){
    network_t *net;
    char filename[1024];
    float output[10], max;
    int i, j, maxj;
    char sid;
    training_data_t data;
    
    preprocess_char32x32(cdata,0,data.intensity);

/* load neural network from file */
    sprintf(filename,"%s/%s",DATADIR,NETWORK_FILE);
    net = net_load(filename);
    if (net == NULL) {
        return(-1);
    }

    net_compute (net, (float *) data.intensity, output);

/* recognized digit is one with highest output */
    maxj = 0;
    max = -1.0;
    for (j = 0; j < 10; j++) {
        if (max < output[j]) {
            max = output[j];
            maxj = j;
        }
    }
    if(max>=0.85) sid=maxj; //0.78
    else sid=-1;
    if(certain!=NULL) *certain=max; 
    net_free(net);
    return sid;
}




/* Function that locates student id number on a scan. Image and location is
 * stored in SID datastructure */
/* does not work with 150dpi scans... Must be at least 200dpi (300dpi) preffered*/
SID *locateSID(PIX *pixs){

/* some of the code was taken from flip*.c in leptonica's examples! Great source
 * of information. */
    SID *sid=(SID *)malloc(sizeof(SID));
    PIX *pixclip=NULL, *pixt0=NULL, *pixt1=NULL, *pixm=NULL;
    BOX *box;
    BOXA *boxa;
    int mArea, cArea, boxid;
    int nbox,x,y,w,h,i;
/* extract the right top corner for further inspection (we don't analyze the whole image). */
    pixclip = clip_image(pixs,pixGetWidth(pixs)*SID_CORNER_X_START, 0, pixGetWidth(pixs)*SID_CORNER_X_END, pixGetHeight(pixs)*SID_CORNER_Y_END);
/* BEGIN TEST: to improve SID location */
	sid->img=pixclip;
	sidEnhance(sid);
	pixclip=sid->img;
/* END TEST */
/* Some basic image morphology to make all id numbers connected */
    pixt0 = pixMorphCompSequence(pixclip, "c1.30 + c30.1", 0); /* changed code 22.12.2011 */
    pixt1 = pixMorphSequence(pixt0, "o10.1", 0); /* obsolete 22.12.2011 */
/* list all connected structures in this corner */
    boxa = pixConnComp(pixt0, NULL, 8); 

    nbox = boxaGetCount(boxa);
    mArea=0;
    if(nbox==0){
        pixDestroy(&pixclip);
        pixDestroy(&pixt0);
        pixDestroy(&pixt1);
        boxaDestroy(&boxa);
        sid->img=NULL;
        return sid;
    }

    for (i = 0; i < nbox; i++) {
        box = boxaGetBox(boxa, i, L_CLONE);
        /*Find highest box. This is supposed to be SID */
        cArea=box->h;
        
        if(cArea>mArea){
            mArea=cArea;
            boxid=i;
        }

            boxDestroy(&box);
    }

/* we get the biggest box */

    box=boxaGetBox(boxa,boxid,L_CLONE);

	unsigned int leftx, rightx, most_right_id=boxid,temp;
	BOX *tempbox;
	PIX *dbg;
	leftx = box->x;
	rightx = box->x;
	for(temp = 0; temp < nbox; temp++) {
		if (temp == boxid) continue;
//		printf("************ Vstop v if\n");	
		tempbox = boxaGetBox(boxa,temp,L_CLONE);
		if ((tempbox->h > box->h*0.70) && (tempbox->h < box->h*1.30)){
//			fprintf(stderr,"Bil sem tu" );
			if (tempbox->x <= leftx) {
//				fprintf(stderr,"Leftmost changed... x=%d\n", tempbox->x);
				leftx = tempbox->x;
			}
			else if(tempbox->x >=rightx) {
//				fprintf(stderr,"Rightmost changed... x=%d\n", tempbox->x);
				rightx = tempbox->x;
				most_right_id = temp;
			}
		}
		boxDestroy(&tempbox);
	}
//	fprintf(stderr,"Index temp=%d",most_right_id);
	tempbox = boxaGetBox(boxa,most_right_id,L_CLONE);
	rightx += tempbox->w;
	
	dbg=clip_image(pixclip,0,tempbox->y,tempbox->x+tempbox->w,tempbox->h);
	saveimage(dbg,"/tmp/rightmost.png");
	saveimage(pixt0,"/tmp/clip.png");
	pixDestroy(&dbg);

	boxDestroy(&tempbox);
	
	box->x = leftx;
	box->w = rightx - leftx;
		

    sid->img=clip_image(pixclip,box->x-SID_INTERSPACE,box->y,box->w+2*SID_INTERSPACE,box->h);
    saveimage(sid->img, "/tmp/tempimg.png");
     
/* Calculate position on image */
/* We just add some padding in front and in back in sid->x and sid->w*/
            sid->x=x+box->x- SID_INTERSPACE;
            sid->y=y+box->y;
            sid->w=box->w + 2*SID_INTERSPACE;
            sid->h=box->h;
            boxDestroy(&box);
    char fname[1024];
    sprintf(fname,"%s/vpisnabox.png",debug_dir);
    saveimage(sid->img, fname);

    pixDestroy(&pixt0);
    pixDestroy(&pixt1);
    pixDestroy(&pixm);
    boxaDestroy(&boxa);
    pixDestroy(&pixclip);
    return(sid);
}


void sidOldEnhance(SID *sid){
 //       saveimage(sid->img,"/tmp/old_vpisna_before.png");
        PIX *pixt1 = pixMorphCompSequence(sid->img, "o3.3", 0); /* Odstranimo pikice o2.2*/
        pixDestroy(&sid->img);
        PIX *pixt2 = pixMorphCompSequence(pixt1, "c9.8 + c9.8", 0); /* naredi bloke */ /*original je bilo 3.8, 3.8*/
        pixDestroy(&pixt1);
        pixDilateBrick(pixt2,pixt2,2,2);
        //pixDestroy(&pixt2);
        sid->img=pixt2;
        saveimage(sid->img,"/tmp/old_vpisna.png");
}

/* Enhances image for OCR. Basically removes noise, skeletonizes and thickens
 * image, so all lines have same width no matter how they are drawn */
void sidEnhance(SID *sid){
    
    PIX *pixt0a, *pixt0b;
//odstrani sum!
    pixt0a=pixMorphSequence(sid->img, "o2.2", 0); 
//closing. Povezi nepovezane dele
    pixt0b = pixMorphCompSequence(pixt0a, "c5.5", 0);
    pixDestroy(&pixt0a);
//opening. Se enkrat odstrani sum, ki se je morebiti prikradel
    pixt0a=pixMorphSequence(pixt0b, "o3.3", 0); 
    pixDestroy(&pixt0b);
// Skeletoniziraj
    pixt0b=pixThin(pixt0a,L_THIN_FG,4,40);
    pixDestroy(&pixt0a);
/* test: remove stubs */
    pixRemoveStub(&pixt0b);
// Odebeli linije
    pixt0a=pixMorphSequence(pixt0b, "d5.5", 0); 
    pixDestroy(&pixt0b);
//closing again...
    pixt0b = pixMorphCompSequence(pixt0a, "c10.10", 0);
    pixDestroy(&pixt0a);
    pixt0a=pixThin(pixt0b,L_THIN_FG,4,40);
    pixDestroy(&pixt0b);
    pixt0b=pixMorphSequence(pixt0a, "d10.10", 0); 
    pixDestroy(&pixt0a);
    pixDestroy(&sid->img);
    sid->img=pixt0b;
//    saveimage(sid->img,"/tmp/vpisnastevilka.png");
}


void pixRemoveStub(PIX **pixt0b){
    PIX *pixs=*pixt0b;
    const char *textsel1 = " o "
                           "oOx"
                           " xx";
    SEL *sel = selCreateFromString(textsel1, 3, 3, "textsel1");
    PIX *pixhmt = pixErode(NULL, pixs, sel);
    saveimage(pixhmt,"/tmp/seltest.png");
    selDestroy(&sel);
    pixDestroy(&pixhmt);
    
}

void sidDestroy(SID **sid){
    if(*sid!=NULL){
    if((*sid)->img!=NULL)
        pixDestroy(&((*sid)->img));
    if((*sid)->sid!=NULL)
        free((*sid)->sid);
    if((*sid)->certainty!=NULL)
        free((*sid)->certainty);
    free(*sid);
    }
}


/* main.c: a main (entry file) for the project Ochi -- a part of HAvOc
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <leptonica/allheaders.h>
#include "imageio.h"
#include "img_process.h"
#include "sid_process.h"
#include "bar_process.h"
#include "ans_process.h"
#include "filesystem.h"
#include "localization.h"
#include "daemon.h"
#include "../config.h"
#include "database.h"

PIX* sidbox(PIX *pixs);

PIX* locatesid2(PIX *pixs);
int process_scans();
static void print_version (void);

void main(int    argc, char **argv) {
    print_version();
/* Read config file */
    readconfig();
/* Localization settings */
    set_localization("");

    test_db_connection();
    process_scans();
}
/* Print version and copyright information.  */

static void print_version (void) {
  fprintf (stdout,_("%s version %s\n\n"), PACKAGE, VERSION);
  
  fprintf (stdout,_("\
Copyright (C) %s Samo Penic, Miha Fosnaric, Ales Berkopec\n\
Published under GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n"),
              "2008--2011");
}



int process_scans(){
  PIX *pixs=NULL, *pixd=NULL;
    int i,j;
    BAR *barkoda=NULL;
    SID *vpisna=NULL;


    glob_t *flist=list_files();

    i=0;
    if(flist->gl_pathc==0) {
        fprintf(stderr,_("No files in current directory... Exiting...\n"));
        exit(1);
    }
    for(i=0;i<flist->gl_pathc;i++){
	    fprintf(stdout,_("file: %s\n"),flist->gl_pathv[i]);
        pixs=loadimage(flist->gl_pathv[i]);
        if(pixs==NULL){
            fprintf(stderr,_("Pix error... Exiting...\n"));
            exit(1);
        }
        pixd=repair_scanned_image(&pixs);
        barkoda=getCode (pixd);
/* If there is no barcode in the expected location that means the scan is
 * something we expected */
        if(barkoda==NULL){
            dfprintf(stdout,_("no barcode\n"));
            /*try to rotate the image 180deg, to see if it is just incorrectly
            * scanned */
            pixDestroy(&pixs);
            pixDestroy(&pixd);
            continue;
        }
/* This is a hack to correct a glitch in barcodes */
        if(barkoda->barcode==NULL){
            barDestroy(&barkoda);
            dfprintf(stdout,_("Code was not decoded... trying again\n"));
            barkoda=getCode(pixs);
        }
/* SID is only on the first page. Inhibit SID recognition on subsequent pages */
        if(barkoda->barcode[7]=='0'){
        vpisna=getSID(pixs);
        }
        else
        {
        vpisna->sid=malloc((SID_LENGTH+1)*sizeof(char));
         vpisna->sid="xxxxxxxx";
        }
        dfprintf(stdout,_("Barcode number of file is %s.\n"),barkoda->barcode);
        dfprintf(stdout,_("Student id number of file is %s.\n"),vpisna->sid);
        dfprintf(stdout,_("Certainty:"));
        for(j=0;j<SID_LENGTH;j++){
            printf(" %d",(int)(vpisna->certainty[j]*100));
        } 
        dfprintf(stdout,".\n");
        ANS *answer=getanswer(pixs);
        
/* save result file */
        writerezfile(pixd, flist->gl_pathv[i], answer, barkoda, vpisna);
/* destroy all allocated space (hopefully) */
        ansDestroy(&answer);
        sidDestroy(&vpisna);
        barDestroy(&barkoda);
        pixDestroy(&pixs);
        pixDestroy(&pixd);
    }
    globfree(flist);
    free(flist);
}





PIX* sidbox(PIX *pixs){

    BOX *clip, *box;
    PIX *pixt1,*pixt0, *pixt0a, *pixt0b;
    
    pixt0a = pixMorphCompSequence(pixs, "c1.8 + c30.1", 0); /* Tukaj nastavimo kdaj so vertikalni deli nepovezani */
 //   pixt0b=pixMorphSequence(pixt0a, "o10.1", 0); 
  //  pixClipToForeground(pixt0b, NULL, &box);

       pixRasterop(pixt0a, 1 + 1,  2, 600 - 2 * 1, 1200 + 13,
                            PIX_SET, NULL, 0, 0);


        saveimage(pixt0a, "vpisnaarea.png");
        return NULL;
}







PIX* locatesid2(PIX *pixs){

    BOX *clip, *box;
    PIX *pixt1,*pixt0, *pixt0a, *pixt0b;
    
  //  clip=boxCreate(VPISNA_X, VPISNA_Y, VPISNA_W, VPISNA_H);
  //  pixt0=pixClipRectangle(pixs,clip,NULL);


    pixt0a = pixMorphCompSequence(pixs, "c8.8", 0); 
    pixt0a = pixMorphCompSequence(pixt0a, "e8.8", 0); 

        saveimage(pixt0a, "vpisnabox.png");

    pixt0b= pixMorphSequence(pixt0a,"d10.10",0);
    //pixt0b=pixThin(pixt0a,L_THIN_BG,4,5);

        saveimage(pixt0b, "vpisnawholebox.png");

        pixDestroy(&pixt0a);
  //      pixDestroy(&pixt0b);
        return NULL;
}



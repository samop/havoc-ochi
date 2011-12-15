/* imageio.c: image input/ouput functions for reading/writting images and
   results file corresponding to the recognised images.

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
#include "../config.h"
#include "imageio.h"
#include "bar_process.h"
#include "sid_process.h"
#include "ans_process.h"
#include "filesystem.h"
#include <string.h>
#include <stdlib.h>
/* save PNG image */
int saveimage(PIX *pixd, char *fileout){
    pixWrite(fileout, pixd, IFF_PNG);
    return 0;
}

/* load PNG, TIFF, JPG, GIF or BMP to PIX datastructure. The actual supported
 * formats depends on how the leptonica was compiled */
PIX *loadimage(char *filename){
    PIX *pix;
    int format, bpp;
    format=fileformat(filename);
        // In later versions of leptonica you will have to do this 
        // pixReadHeader(filename, format,NULL,NULL,NULL,bpp,NULL);
    if(format!=IFF_PNG && format!=IFF_JFIF_JPEG && format!=IFF_TIFF && format!=
IFF_GIF && format!=7 && format!=8){
        dfprintf(stderr,"Not recognised file format %i", format);
        return NULL;
    }
    if ((pix = pixRead(filename)) == NULL) return NULL;

/* TODO: convert image to 1-bpp 300dpi regardless of scan */
   return pix; 
}

/* leptonica 1.62 doesn't have this function yet, so here it is. */
int fileformat(char *filename){
    FILE *fp;
    int format;
     if ((fp = fopenReadStream(filename)) == NULL)
        return ERROR_INT("image file not found", "fileformat", 1);
#ifdef NEWLEPT
	findFileFormat(filename,&format);
#else
    format=findFileFormat(fp);
#endif
    fclose(fp);
    return format;
}

/* write result file */
int writerezfile(PIX *pixs,char *source_fname,ANS *ans, BAR *bar, SID *sid){
    int i;
    float mincert;
    char fname[1024];
    char picfname[1024];
    char *base=calloc(1024,sizeof(char));
    char *ridx=rindex(source_fname,'.');
    char *ansarray;
 
    base=strncpy(base, source_fname,ridx-source_fname);
    char *picname=rindex(base,'/');
    if(picname==NULL) picname=base;
    mincert=100.0;
    for(i=0;i<SID_LENGTH;i++){
        if(sid->certainty[i]<mincert){
            mincert=sid->certainty[i];
        }
    }

    if(bar->barcode==NULL)
        sprintf(fname,"%s/%s.err",output_dir,picname);
    else if(mincert<0.85)
        sprintf(fname,"%s/%s.war",output_dir,picname);
    else
        sprintf(fname,"%s/%s.rez",output_dir,picname);

    sprintf(picfname,"%s/%s.png",output_dir,picname);

    FILE *fh=fopen(fname,"w");
    fprintf(fh,"%s\n",bar->barcode+1);
    fprintf(fh,"%s\n",sid->sid);
    fprintf(fh,"%s\n",ans->ans_string);
    fprintf(fh,"%d\n",(int)(mincert*100));
    fprintf(fh,"A\n");
    
/*    fprintf(fh,"x=array(");
    for(i=0;i<ans->up->n-1;i++){
        fprintf(fh,"%f,",ans->up->mark[i].x);
    }
    fprintf(fh,"%f);",ans->up->mark[i].x);
    fprintf(fh,"y=array(");
    for(i=0;i<ans->right->n-1;i++){
        fprintf(fh,"%f,",ans->right->mark[i].y);
    }
    fprintf(fh,"%f);\n",ans->right->mark[i].y); */
    
    ansarray=ans_array(ans);
    fprintf(fh,"%s\n",ansarray);
    free(ansarray);
    fclose(fh);
//rescale image
    PIX *pixd=pixScale(pixs, (float)1280/pixGetWidth(pixs),(float)1280/pixGetWidth(pixs));
    saveimage(pixd,picfname);
    pixDestroy(&pixd);

    sprintf(fname,"%s/%s.tif",processed_dir,picname);
    int retval=rename(source_fname, fname);
    if(retval<0){
    dfprintf(stderr,"rename not successful!\n");
    }

    free(base);

    return 0;
}


char *ans_array(ANS *ans){
    int i;
    char *str=malloc(10000*sizeof(str));
    sprintf(str,"x=array(");
    for(i=0;i<ans->up->n-1;i++){
        sprintf(str,"%s%f,",str,ans->up->mark[i].x);
    }
    sprintf(str,"%s%f);",str,ans->up->mark[i].x);
    sprintf(str,"%s y=array(",str);
    for(i=0;i<ans->right->n-1;i++){
        sprintf(str,"%s%f,",str,ans->right->mark[i].y);
    }
    sprintf(str,"%s%f);",str,ans->right->mark[i].y);
    return str;
}

char *picfname(char *source_fname){
    char *picfname=malloc(1024*sizeof(char));
    char *base=calloc(1024,sizeof(char));
    char *ridx=rindex(source_fname,'.');

    base=strncpy(base, source_fname,ridx-source_fname);
    char *picname=rindex(base,'/');

    sprintf(picfname,"%s/%s.png",output_dir,picname);
    free(base);
    return picfname;
}

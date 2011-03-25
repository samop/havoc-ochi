/* bar_process.c: finds and decodes barcode that corresponds to id of
   examination sheet (pid).

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


#include <leptonica/allheaders.h>
#include<stdlib.h>
#include<stdint.h>
#include "sid_process.h"
#include "img_process.h"
#include "bar_process.h"
#include "daemon.h"
#include "localization.h"
#include <string.h>
#include <zbar.h>


/* Function that effectively locates and decodes barcode */
BAR *getCode(PIX *pixs){

    BAR *code=locateCode(pixs);
    if(code==NULL){
        return NULL;
    }
    
    decodeCode(code);
    if(code->barcode==NULL){
        PIX *temp;
        temp=code->img;
        dfprintf(stdout,_("tryig another way\n"));
        //pixRotate180(code->img,code->img);
        code->img=pixMorphSequence(temp, "d1.1", 0); 
        pixDestroy(&temp);
        decodeCode(code);         
    }
    return code; 
}

/* Function uses leptonica library to find location of barcode (basically it
 * finds set of vertical bars). It returns a data structure with image of the
 * barcode and it's location in the main image. */
BAR *locateCode(PIX *pixs){
    BAR *code; 
    PIX *image;
    BOXA *loc_barcode;  /* BOXA is an array of boxes */
    int i, bx,by, bw, bh;
/*let's clip a part of image, where the barcode resides */
    image = clip_image(pixs,BAR_CORNER_X_START*pixGetWidth(pixs), 
            BAR_CORNER_Y_START*pixGetHeight(pixs),
            BAR_CORNER_X_END*pixGetWidth(pixs), 
            BAR_CORNER_Y_END*pixGetHeight(pixs));
/* Lets determine positions of Barcodes */
    loc_barcode= pixLocateBarcodes(image,20,NULL,NULL);
//    printf("(I) Nasel sem %i potencialnih barkod!\n", boxaGetCount(loc_barcode));
    if(boxaGetCount(loc_barcode)>2 || boxaGetCount(loc_barcode)==0){
/* No barcode found */
        pixDestroy(&image);
        boxaDestroy(&loc_barcode);
        return NULL;
    } 
    code=(BAR *)malloc(sizeof(BAR));
    boxaGetBoxGeometry(loc_barcode,0, &code->x, &code->y, &code->w, &code->h);
// TODO: make this clip relative to the image size and not absolute!
    code->img=clip_image(pixs,code->x+=-100,code->y+=-30,code->w+=200,code->h+=100);
//    printf("(I) Poz. pot. barkode %i je : x=%i y=%i w=%i h=%i\n",i,code->x,code->y,code->w,code->h);
    pixDestroy(&image);
    boxaDestroy(&loc_barcode);
    return code;
}


/* Function gets the barcode from *pixs. Function is derived from example in
 * zbarlib with minor changes in image acquisition routine, since we already
 * have an image in memory we don't need to read it from disk as it was in
 * original example. Also, ZBAR_CFG_ADD_CHECK has been added and ZBAR_CFG_ENABLE
 * disabled, to disable recognition of symbology and to receive all 13 digits of
 * barcode. First digit is not in use in HAvOc as of beginning of 2011! Witho
 * minor modifications this function may work from live video stream.
 * Recognition is fast and if leptonica does some preprocessing it should be
 * lightning fast.*/
void decodeCode (BAR *code){
/* create a reader */
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();
/* return value */
    char *retval;
    char *data;
    if(code->img!=NULL)
        saveimage(code->img,"/tmp/barkoda.png");
/* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ADD_CHECK, 1);
/* obtain image data */
    int width = pixGetWidth(code->img), height = pixGetHeight(code->img);
    void *raw =NULL;
     img2yuv800(code->img,&raw);
/* wrap image data into zbarlib data structure. Use YUV800 encoding. */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, (void *)raw, width * height, zbar_image_free_data);

/* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
        if (n>1 || n==0) {
            /* multiple barcode or no barcode found!*/
            zbar_image_destroy(image);
            zbar_image_scanner_destroy(scanner);
            code->barcode=NULL;
            return; /* RETURNS IF SOMETHING WRONG! */
    }
/* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        data = zbar_symbol_get_data(symbol);
    /* clean up */
    char *outdata=malloc(14*sizeof(char));
    outdata=strcpy(outdata,data);
    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);
    code->barcode=outdata;
    //free(symbol);
}

/* Function converts leptonica image (1-bpp bitmap) to YUV800 encoding.
 * Seriously, it can be grayscale, but it is disabled by default. all pixels
 * with value greater than 0 are considered black. Function returns nothing, but
 * the image is stored in pointer to pointer to void raw data. Function is
 * needed for converting leptonica image of barcode to zbarlib processor. */
void img2yuv800(PIX *pixs,void **yuv){
    int width=pixGetWidth(pixs);
    int height= pixGetHeight(pixs);
    int pixval,i,j;
/* allocate memory for raw data (is it deallocated when destroying image data
 * structure? */
    uint8_t *data = (uint8_t *)malloc(width*height*sizeof(uint8_t));
/* copy raw pixel data into raw data matrix */
    for(j = 0;j < height;j++){
        for(i = 0; i < width; i++){
            pixGetPixel(pixs,i,j, &pixval);
            data[j*width+i]=pixval?0:255;
        }
    }
    *yuv=data; //allocate void pointer yuv to data structure
}

/* Reads YUV800 ecoded image of width w and height h and returns PIX to the
 * caller, which can be used to manipulate in leptonica. As for now this
 * function exists, it works but is not used anywhere. It just seemed right to
 * coexists with img2yuv800 as a function with reverse operation. */
PIX *yuv8002img(uint8_t *yuv800, int w, int h){
    PIX *pix=pixCreate(w,h,1);
    int i,j;
    for(j = 0;j < h;j++){
        for(i = 0; i < w; i++){
            pixSetPixel(pix,i,j, yuv800[j*w+i]);
        }
    }
    return pix;
}

void barDestroy(BAR **barcode){
    if((*barcode)->img!=NULL)
        pixDestroy(&((*barcode)->img));
    if((*barcode)->barcode!=NULL)
        free((*barcode)->barcode);
    
    
    free(*barcode);


}

int bar_get_page_no(char *barstring){ 
    return barstring[7]-'0'+1;
}


char *bar_get_first_code(char *barstring){
    char *firstcode=malloc(14*sizeof(char));
    firstcode=strcpy(firstcode, barstring);
    firstcode[7]='0';
    firstcode[12]='0'+ean13_check_digit(firstcode);
    return firstcode;
}


int ean13_check_digit(char *bs){
    int even;
    int odd;
    int total;
    even=bs[1]+bs[3]+bs[5]+bs[7]+bs[9]+bs[11]-6*'0';
    odd=bs[0]+bs[2]+bs[4]+bs[6]+bs[8]+bs[10]-6*'0';
    total=even*3+odd; 
    return total%10 ? 10-total%10 : 0; /* this fixes bug when : is returned instead of 0 ! */
}

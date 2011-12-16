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


#include <stdlib.h>
#include <leptonica/allheaders.h>
#include <localization.h>
#include "daemon.h"
#include "filesystem.h"

PIX* sidbox(PIX *pixs);
PIX* locatesid2(PIX *pixs);
//int process_scans();
//static void print_version (void);

/* Entry to the program */
int main(int    argc, char **argv) {
    int c;
/* Read config file */
    readconfig();
    while( (c = getopt(argc, argv, "dnhe|help")) != -1) {
        switch(c){
            case 'h':
                print_usage();
                exit(0);
                break;
            case 'd':
                daemon_flag = 1;
                break;
            case 'n':
                daemon_flag = 0;
                break;
	    case 'e':
		extract_flag = 1;
		break;
            default:
                print_usage();
                exit(0);
                break;
        }
    }
/* Localization settings */
    set_localization("");
    if (use_database) test_db_connection();
    if(daemon_flag)
        daemonize();
    else {
        if(lock_file()) {
            fprintf(stderr,"Error: cannot get lock file /var/lock/ochi. Maybe another process is running?");
            exit(1);
        } 
        process_scans();
    }

    exit(0);
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



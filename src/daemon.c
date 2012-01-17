/* daemon.c: Daemonize ochi process
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


#include "daemon.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>


#include <leptonica/allheaders.h>
#include "imageio.h"
#include "img_process.h"
#include "sid_process.h"
#include "bar_process.h"
#include "ans_process.h"
#include <filesystem.h>
#include <localization.h>
#include "daemon.h"
#include "../config.h"
#include "database.h"

//int daemon_flag=0;


/* outputs string depending on quiet mode */
int dfprintf(FILE *fd, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
if(daemon_flag) {
    syslog(LOG_INFO,fmt,ap);
    va_end(ap); /* Cleanup the va_list */
    return 0;
}
    vfprintf(fd, fmt, ap); /* Call vfprintf */
    va_end(ap); /* Cleanup the va_list */
    return 0;
}



void signal_handler(int sig) {
 
    switch(sig) {
        case SIGHUP:
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            break;
        case SIGTERM:
            syslog(LOG_WARNING, "Received SIGTERM signal.");
            exit(1);
            break;
        case SIGKILL:
            syslog(LOG_WARNING, "Received SIGKILL signal.");
            exit(1);
            break;
        default:
            syslog(LOG_WARNING, "Unhandled signal (%d) %s",sig, strsignal(sig));
            break;
    }
}




int daemonize(){
    signal(SIGHUP, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

  
    // Our process ID and Session ID
    pid_t pid, sid;
    /* Fork the parent process */
    pid = fork();
    if (pid < 0) {
            exit(EXIT_FAILURE);
    }
    // If we got a good PID, then we can exit the parent process.
    if (pid > 0) {
            exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    openlog("havoc-ochi", LOG_PID|LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Starting daemon ... ");

            
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
            /* Log the failure */
            exit(EXIT_FAILURE);
    }
    

    
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
            /* Log the failure */
            exit(EXIT_FAILURE);
    }

    /* Ensure only one copy */
    if(lock_file()) {
        fprintf(stderr,"Fatal: Cannot get a lock file /var/lock/ochi. Maybe another process is running"); 
        exit(EXIT_FAILURE);
    } 
    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
   


    /* Daemon-specific initialization goes here */
 
    /* The Big Loop */
    while (1) {
       /* Do some task here ... */
       process_scans();
       sleep(10); /* wait 10 seconds */
    }
exit(EXIT_SUCCESS);

}



int lock_file(){
    int pidFilehandle;
    char str[10]; 
    char pidfile[]="/var/lock/ochi";

   pidFilehandle = open(pidfile, O_RDWR|O_CREAT, 0600);

    if (pidFilehandle == -1 ){
        /* Couldn't open lock file */
        syslog(LOG_INFO, "Could not open PID lock file %s, exiting", pidfile);
        return 1;
    }
    /* Try to lock file */
    if (lockf(pidFilehandle,F_TLOCK,0) == -1)
    {
        /* Couldn't get lock on lock file */
        syslog(LOG_INFO, "Could not lock PID lock file %s, exiting", pidfile);
        return 1;
    }

    /* Get and format PID */
    sprintf(str,"%d\n",getpid());
    /* write pid to lockfile */
    write(pidFilehandle, str, strlen(str));
    return 0;
}

int process_scans(){
    PIX *pixs=NULL, *pixd=NULL;
    int i,j;
    BAR *barkoda=NULL;
    SID *vpisna=NULL;


    glob_t *flist=list_files();

    i=0;
    if(flist->gl_pathc==0) {
//        dfprintf(stderr,_("No files in current directory... Exiting...\n"));
//        exit(1);
        return(EXIT_FAILURE);
    }

/* connect to database */
	PGconn     *conn;
    if(use_database){
    conn = connect_db();
	if (PQstatus(conn) != CONNECTION_OK) {
        globfree(flist);
        free(flist);
        return(EXIT_FAILURE); // if cannot connect 
    }
    }
    for(i=0;i<flist->gl_pathc;i++){
	    dfprintf(stdout,_("file: %s\n"),flist->gl_pathv[i]);
        pixs=loadimage(flist->gl_pathv[i]);
        if(pixs==NULL){
            dfprintf(stderr,_("Pix error... Exiting...\n"));
            if(use_database) close_db(conn); /* close connection to database */
            globfree(flist);
            free(flist);
            return(EXIT_FAILURE);
        }
        pixd=repair_scanned_image(&pixs);
        barkoda=getCode (pixd);

//try rotation for 180 degrees after first unsuccessful getCode()
	if(barkoda==NULL) {
		fprintf(stdout,"Unable to find barcode, trying rotation\n");
		pixRotate180(pixd,pixd); 
		pixRotate180(pixs,pixs); 
		barkoda=getCode(pixd);} 

/* If there is no barcode in the expected location that means the scan is
 * something we expected */
        if(barkoda==NULL){
            dfprintf(stdout,_("no barcode\n"));
	    pixDestroy(&pixd);
	    pixDestroy(&pixs);
            continue;
        }
/* This is a hack to correct a glitch in barcodes */
        if(barkoda->barcode==NULL){
            barDestroy(&barkoda);
            dfprintf(stdout,_("Code was not decoded... trying again\n"));
            barkoda=getCode(pixs);
        }
	pixDestroy(&pixs);
/* SID is only on the first page. Inhibit SID recognition on subsequent pages */
        if(barkoda->barcode[7]=='0'){
	//fprintf(stderr," prva stran bul sem tu %s\n", barkoda->barcode);
        vpisna=getSID(pixd);

        }
        else
        {
            /*Ugly, dirty and fix for multiple scan pages */
            //Not so ugly and dirty anymore! */
	      	//vpisna = getSID(pixd);
            vpisna=(SID *)malloc(sizeof(SID));
            vpisna->sid=(char *)malloc(2*sizeof(char));
            vpisna->sid[0]='x';
            vpisna->sid[1]='\0';
            vpisna->img=NULL;
            vpisna->certainty=(float *)malloc(SID_LENGTH*sizeof(float));
        	for(j=0;j<SID_LENGTH;j++){
            		vpisna->certainty[j]=100;
        	} 
        }
        dfprintf(stdout,_("Barcode number of file is %s.\n"),barkoda->barcode);
        dfprintf(stdout,_("Student id number of file is %s.\n"),vpisna->sid);
//        dfprintf(stdout,_("Certainty:"));
/*        for(j=0;j<SID_LENGTH;j++){
            dfprintf(stdout," %d",(int)(vpisna->certainty[j]*100));
        } 
        dfprintf(stdout,".\n"); */
        ANS *answer=getanswer(pixd);
        
/* save result file for legacy database insert */
        writerezfile(pixd, flist->gl_pathv[i], answer, barkoda, vpisna);
/* insert into database */
        if(use_database) db_insert_wrapper(conn,flist->gl_pathv[i], answer, barkoda, vpisna);

/* destroy all allocated space (hopefully) */
        ansDestroy(&answer);
        sidDestroy(&vpisna);
        barDestroy(&barkoda);
        pixDestroy(&pixd);
    }

    if(use_database) close_db(conn); /* close connection to database */
    globfree(flist);
    free(flist);
    return(EXIT_SUCCESS);
}


/* Print version and copyright information.  */
void print_version (void) {
  fprintf (stdout,_("%s version %s\n\n"), PACKAGE, VERSION);
  
  fprintf (stdout,_("\
Copyright (C) %s Samo Penic, Miha Fosnaric, Ales Berkopec\n\
Code contributors: Miha Halas, Dejan Hrovatin\n\
Published under GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n\n"),
              "2008--2012");
}



void print_usage (void){

    print_version();

    fprintf(stdout,_("\
Usage instructions:\n\n\
-h|-help : printout this text\n\
-d : daemonize process\n\
-e : extract numerals -- you have to create directories /tmp/numerals/X, where X is 0 to 9.\n"));
}



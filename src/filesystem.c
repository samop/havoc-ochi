/* filesystem.c: Functions that has to do with files, directories and configs.

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
#include <confuse.h>
#include "filesystem.h"
#include "daemon.h"
#include "localization.h"

glob_t *list_files(void)
{
    int retval;
    char path[1024];
    sprintf(path,"%s/*.tif", scan_dir);
    glob_t *data=(glob_t *)malloc(sizeof(glob_t));
    retval= glob(path, 0, NULL, data );
/*    switch( retval )
    {
        case 0:
            break;
        case GLOB_NOSPACE:
            printf( "Out of memory\n" );
            break;
        case GLOB_ABORTED:
            printf( "Reading error\n" );
            break;
        case GLOB_NOMATCH:
            printf( "No files found\n" );
            break;
        default:
            break;
    }
*/
/*    int i;
    for(i=0; i<data->gl_pathc; i++)
    {
        printf( "%s\n", data->gl_pathv[i] );
    } */
 //   globfree( &data );
    return data;
}


/*set global variable*/

void readconfig(){
    char conffile[255];
    sprintf(conffile,"%s/ochi.conf",CONFDIR);
    dfprintf(stdout,_("Config dir in: %s\n"),conffile);
//    debug_dir=(char *)malloc(1024*sizeof(char));
//    debug_dir=(char *)malloc(1024*sizeof(char));


    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("scan_directory", &scan_dir),
        CFG_SIMPLE_STR("processed_directory", &processed_dir),
        CFG_SIMPLE_STR("debug_output", &debug_dir),
        CFG_SIMPLE_STR("result_output",&output_dir),
        CFG_SIMPLE_STR("database_name",&database_name),
        CFG_SIMPLE_STR("database_user",&database_user),
        CFG_SIMPLE_BOOL("debug",&debug),
        CFG_SIMPLE_BOOL("use_database",&use_database),
        CFG_SIMPLE_BOOL("daemon",&daemon_flag),
        CFG_END()
    };
    cfg_t *cfg;    
    int retval;
    cfg = cfg_init(opts, 0);
    retval=cfg_parse(cfg, conffile);
    if(retval==CFG_FILE_ERROR){
        dfprintf(stderr, "No config file.\n");
        exit(1);
        }
    else if(retval==CFG_PARSE_ERROR){
        dfprintf(stderr,"Invalid config file!\n");
        exit(1);
        }
    cfg_free(cfg);
/* define global variables for two commonly used variables */
    char mark1filename[1024];
    sprintf(mark1filename,"%s/square.png",DATADIR);
    PIXmark1=loadimage(mark1filename);
    if(PIXmark1==NULL){
        dfprintf(stderr,"Error, cannot find marker file!\n");
        exit(1);
        }

    //sprintf(networkfile,"%s/%s",DATADIR,NETWORK_FILE);


}

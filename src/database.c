/* database.c: functions that inserts the scans into database
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


#include<stdlib.h>
#include<stdio.h>
#include<leptonica/allheaders.h>
#include<fcntl.h>
#include "bar_process.h"
#include "ans_process.h"
#include "sid_process.h"
#include "imageio.h"
#include "database.h"

void close_db(PGconn *conn){
     PQfinish(conn);
}

PGconn *connect_db(){
	PGconn     *conn;
    const char *conninfo = "dbname = havoc";
	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK)
        {
                printf("Connection to database failed: %s\n",
                        PQerrorMessage(conn));
        }
    return conn;
}




void test_db_connection(){
	const char *conninfo;
	PGconn     *conn;
        PGresult   *res;
        int	nFields;

	conn = connect_db();
	if (PQstatus(conn) != CONNECTION_OK)
        {
                printf("Connection to database failed: %s\n",
                        PQerrorMessage(conn));
                close_db(conn);
                exit(1);
        }
    else {
        printf("Connection to database successful!\n");
    }

    close_db(conn);
}


int db_insert_wrapper(char *filename, ANS *ans, BAR *barkoda, SID *vpisna){
	PGconn     *conn;
    PGresult   *res;
    conn = connect_db();
    int retval;
	if (PQstatus(conn) != CONNECTION_OK) return 1; /* if cannot connect */
    
    if (db_check_serial_validity(conn,barkoda->barcode)==0) { close_db(conn); return 1;}
    if (db_check_scan_presence(conn,barkoda->barcode)==1) { close_db(conn); return 1;} 
    char *ansarray=ans_array(ans);
    char *picname=picfname(filename);
    retval= db_insert_scan(conn, barkoda->barcode, ans->ans_string, "REZ",ansarray,vpisna->sid, picname);
    res= PQexec(conn,"update pola set first_id=T2.pola_id from  pola T2 where pola.first_ser_st=T2.ser_st and pola.first_id is NULL;");
    PQclear(res);
    free(ansarray);
    free(picname);
    close_db(conn);
    return retval;
}

/* checks if the serial number of scan is valid in database. Returns 1 on
 * success, 0 on fail */
int db_check_serial_validity(PGconn *conn, char *bar){
    PGresult   *res;
    char *query=malloc(1024*sizeof(char));

    sprintf(query,"SELECT ser_st FROM resitve where ser_st='%s';",bar);
    res = PQexec(conn, query);
    free(query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("Checking for serial number in database failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }
    if(PQntuples(res)==1){
        PQclear(res);
        return 1;
    }
    else {
        PQclear(res);
        return 0;
    }
}


int db_check_scan_presence(PGconn *conn, char *bar){
    PGresult   *res;
    char *query=malloc(1024*sizeof(char));

    sprintf(query,"SELECT ser_st FROM pola where ser_st='%s';",bar);
    res = PQexec(conn, query);
    free(query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("Checking for already present scan in database failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }
    if(PQntuples(res)>0){
        PQclear(res);
        return 1;
    }
    else {
        PQclear(res);
        return 0;
    }
}

int db_insert_scan(PGconn *conn, char *barcode, char *answerstring, char *debug, char *coords, char *sid, char *filename){
    PGresult   *res;
    char *query=malloc(10000*sizeof(char));
    char *first_ser=bar_get_first_code(barcode);
    Oid blobId=db_insert_blob(conn,filename);
    if(blobId==0){
        printf("db_insert_scan: error in calling db_insert_blob. Scan will not be inserted!\n");
        return 1;
    }
    sprintf(query,"INSERT INTO pola (student_id, sken_filename, odgovori, debug, ser_st, image, first_ser_st, page_no, coords, ocena, procenti) VALUES ('%s', '%s', '%s','%s','%s','%d','%s',%d,'%s',%d,%d);",sid, filename, answerstring, debug ,barcode, blobId, first_ser, bar_get_page_no(barcode), coords, -1, -1);
    free(first_ser);
    res=PQexec(conn,query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("db_insert_scan error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }
    PQclear(res);
    return 0;  
}

Oid db_insert_blob(PGconn *conn, char *filename){
    PGresult *res;
    Oid blobId;
    int fd, lobj_fd;
    char buf[1024];
    int nbytes, tmp;
    fd = open(filename, O_RDONLY, 0666);
    if (fd < 0){                           /* error */
        printf("db_insert_blob: can't open file %s\n", filename);
        return 0;
    }
    res = PQexec(conn, "begin"); /* blob operations must be in begin-end block */
    PQclear(res);

    blobId = lo_creat(conn, INV_READ | INV_WRITE);
    if (blobId == 0){
        fprintf(stderr, "db_insert_blob: can't create large object\n");
        return 0;
    }


    lobj_fd = lo_open(conn, blobId, INV_WRITE);


    while ((nbytes = read(fd, buf, 1024)) > 0){
        tmp = lo_write(conn, lobj_fd, buf, nbytes);
        if (tmp < nbytes)
            fprintf(stderr, "db_insert_blob: error while reading large object %d != %d\n", tmp,nbytes);
    }


    close(fd);
    lo_close(conn, lobj_fd);
    res = PQexec(conn, "end");
    PQclear(res); 
    return blobId;
}


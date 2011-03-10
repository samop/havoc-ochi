/* database.h: header file for database.c
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


#ifndef _H_DATABASE
#define _H_DATABASE

#include<libpq-fe.h>
#include <libpq/libpq-fs.h>
PGconn *connect_db();
void close_db(PGconn *conn);
void test_db_connection();
int db_insert_wrapper(PGconn *conn, char *filename, ANS *ans, BAR *barkoda, SID *vpisna);
int db_check_serial_validity(PGconn *conn, char *bar);
int db_check_scan_presence(PGconn *conn, char *bar);
int db_insert_scan(PGconn *conn, char *barcode, char *answerstring, char *debug, char *coords, char *sid, char *filename);
Oid db_insert_blob(PGconn *conn, char *filename);

#endif

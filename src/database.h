#ifndef _H_DATABASE
#define _H_DATABASE

#include<libpq-fe.h>
#include <libpq/libpq-fs.h>
PGconn *connect_db();
void close_db(PGconn *conn);
void test_db_connection();
int db_insert_wrapper(char *filename, ANS *ans, BAR *barkoda, SID *vpisna);
int db_check_serial_validity(PGconn *conn, char *bar);
int db_check_scan_presence(PGconn *conn, char *bar);
int db_insert_scan(PGconn *conn, char *barcode, char *answerstring, char *debug, char *coords, char *sid, char *filename);
Oid db_insert_blob(PGconn *conn, char *filename);

#endif

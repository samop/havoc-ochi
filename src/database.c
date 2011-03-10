#include<stdlib.h>
#include<stdio.h>
#include<libpq-fe.h>

static void
exit_nicely(PGconn *conn)
{
        PQfinish(conn);
        exit(1);
}


void test_db_connection(){
	const char *conninfo;
	PGconn     *conn;
        PGresult   *res;
        int	nFields;

	conninfo = "dbname = havoc";
	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK)
        {
                fprintf(stderr, "Connection to database failed: %s",
                        PQerrorMessage(conn));
                exit_nicely(conn);
        }

}


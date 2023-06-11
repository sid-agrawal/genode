/*
 * \brief  Trivial program to exercise filesystem and sqlite
 * \author Sid Agrawal
 * \date   2023-06-10
 */

/*
 * Copyright (C) 2023 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "./sqlite3.h"




static int callback(__attribute__((unused))void *NotUsed,
                int argc, char **argv, char **azColName){
        int i;
        for(i=0; i<argc; i++){
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
}

int main(int, char **, char **, LibC::Env &env) {
        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        rc = sqlite3_open("/my_notes/sql.db", &db);
        if( rc ){
                printf("Can't open database: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return(1);
        }

        /* Create Table */
        static char const *cmd1 = "CREATE TABLE KVStore (key int, val int)";
        rc = sqlite3_exec(db, cmd1, callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                return 1;
        }

        /* Insert value */
        char buf[200];
        for(int i= 1; i <= 10; i++){
                sprintf(buf,"INSERT INTO KVStore (key, val) VALUES (%d,%d)", i, i*i);
                rc = sqlite3_exec(db, buf, callback, 0, &zErrMsg);
                if( rc!=SQLITE_OK ){
                        printf("SQL error: %s\n", zErrMsg);
                        sqlite3_free(zErrMsg);
                        return 1;
                }
        }


        sqlite3_stmt *res;
        rc = sqlite3_prepare_v2(db, "SELECT * FROM KVStore", -1, &res, 0);
        if (rc != SQLITE_OK) {
                printf("Failed to fetch data: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return 1;
        }
        for(int i= 1; i <= 10; i++){
                rc = sqlite3_step(res);
                if (rc == SQLITE_ROW) {
                        printf("--%s | %s\n", sqlite3_column_text(res, 0), sqlite3_column_text(res, 1));
                } else {
                        break;
                }
        }
        /*(XXX) Add error check*/
        sqlite3_finalize(res);

        /* Drop Table */
        static char const *cmd3 = "DROP TABLE KVStore";
        rc = sqlite3_exec(db, cmd3, callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
        }
        printf("Finishing test");

        sqlite3_close(db);
        return 0;
}

/* Genode includes */
#include <libc/component.h>

/* libc includes */
#include <libc/args.h>
#include <stdlib.h> /* 'exit'   */

/* initial environment for the FreeBSD libc implementation */
extern char **environ;

/* provided by the application */
// extern "C" int main(int argc, char **argv, char **envp);

static void construct_component(Libc::Env &env)
{
	int argc    = 0;
	char **argv = nullptr;
	char **envp = nullptr;

	populate_args_and_env(env, argc, argv, envp);

	environ = envp;

	exit(main(argc, argv, envp, env));
}


void Libc::Component::construct(Libc::Env &env)
{
	Libc::with_libc([&] () { construct_component(env); });
}

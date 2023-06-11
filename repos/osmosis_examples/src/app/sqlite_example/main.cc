/*
 * \brief  Writing files in a VFS <ram> FS
 * \author Martin Stein
 * \date   2021-06-21
 */

/*
 * Copyright (C) 2021 Genode Labs GmbH
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

int main(int, char **) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    printf("Starting example\n");
    rc = sqlite3_open("/my_notes/sql.db", &db);
    if( rc ){
      printf("Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }

    /* Create Table */

    static char const *cmd1 = "CREATE TABLE KVStore ( key int, val int)";
    rc = sqlite3_exec(db, cmd1, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }

    /* Insert value */
    static char const *cmd2 = "INSERT INTO KVStore (key, val) VALUES (1, 4)";
    rc = sqlite3_exec(db, cmd2, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }


    /* Drop Table */
    static char const *cmd3 = "DROP TABLE KVStore";
    rc = sqlite3_exec(db, cmd3, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    return 0;
  }

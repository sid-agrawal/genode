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

#include <root/component.h>
#include <hello_session/hello_session.h>
#include <base/rpc_server.h>


/* Genode includes */
#include <libc/component.h>

#include <stdlib.h> /* 'exit'   */

static int do_sql();
/*
 * \brief  Main program of the Hello server
 * \author Björn Döbel
 * \author Norman Feske
 * \date   2008-03-20
 */

/*
 * Copyright (C) 2008-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/component.h>
#include <base/log.h>
#include <base/heap.h>
#include <root/component.h>
#include <hello_session/hello_session.h>
#include <base/rpc_server.h>

namespace Hello {
	struct Session_component;
	struct Root_component;
	struct Main;
}


struct Hello::Session_component : Genode::Rpc_object<Session>
{
        void say_hello() override
        {
                Genode::log("I am here... Hello.");
		Libc::with_libc([] () { do_sql(); });

        }

        int add(int a, int b) override {
		return a + b; }
};


class Hello::Root_component
:
	public Genode::Root_component<Session_component>
{
	protected:

		Session_component *_create_session(const char *) override
		{
			Genode::log("creating hello session");
			return new (md_alloc()) Session_component();
		}

	public:

		Root_component(Genode::Entrypoint &ep,
		               Genode::Allocator &alloc)
		:
			Genode::Root_component<Session_component>(ep, alloc)
		{
			Genode::log("creating root component");
		}
};


// {
// 	Genode::Env &env;

// 	/*
// 	 * A sliced heap is used for allocating session objects - thereby we
// 	 * can release objects separately.
// 	 */
// 	Genode::Sliced_heap sliced_heap { env.ram(), env.rm() };

// 	Hello::Root_component root { env.ep(), sliced_heap };

// 	Main(Genode::Env &env) : env(env)
// 	{
// 		/*
// 		 * Create a RPC object capability for the root interface and
// 		 * announce the service to our parent.
// 		 */
// 		env.parent().announce(env.ep().manage(root));
// 	}
// };


struct Hello::Main
{
	Libc::Env               &env;
	Genode::Sliced_heap sliced_heap { env.ram(), env.rm() };
	Hello::Root_component root { env.ep(), sliced_heap };

	Main(Libc::Env &env) : env(env)
	{
		env.parent().announce(env.ep().manage(root));
	//	Libc::with_libc([] () { do_sql(); });
	}
};


#if 1
static int callback(__attribute__((unused))void *NotUsed,
                int argc, char **argv, char **azColName){
        int i;
        for(i=0; i<argc; i++){
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
}
#endif

__attribute__((unused))
static int do_sql()
{
        Genode::log("-----");
        sqlite3 *db;
        int rc;

        /* This needs to be available*/
	// static Hello::Main main(x);

        rc = sqlite3_open("/my_notes/sql.db", &db);
        if( rc ){
                printf("Can't open database: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return(1);
        }
                Genode::log("opened DB");

        char *zErrMsg = 0;
        /* Create Table */
        static char const *cmd1 = "CREATE TABLE KVStore (key int, val int)";
        rc = sqlite3_exec(db, cmd1, callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                return 1;
        }
                Genode::log("Table made DB");

        /* Insert value */
        char buf[200];
        for(int i= 1; i <= 5; i++){
                sprintf(buf,"INSERT INTO KVStore (key, val) VALUES (%d,%d)", i, i*i);
                rc = sqlite3_exec(db, buf, callback, 0, &zErrMsg);
                if( rc!=SQLITE_OK ){
                        printf("SQL error: %s\n", zErrMsg);
                        sqlite3_free(zErrMsg);
                        return 1;
                }
        }
                Genode::log("Values Inserted made DB");


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
                        const unsigned char * x = sqlite3_column_text(res, 0);
                        const unsigned char * y = sqlite3_column_text(res, 1);
                        Genode::log("--", x, " | ", y);
                } else {
                        break;
                }
        }
                Genode::log("Values Read from DB");

        /*(XXX) Add error check*/
        rc = sqlite3_finalize(res);
        if (rc != SQLITE_OK) {
                Genode::log("Failed to fetch data:" , sqlite3_errmsg(db));
                sqlite3_close(db);
                return 1;
        }

                Genode::log("Statement closed");

/*Enabling this somehow hangs the test.*/
#if 0
        /* Drop Table */
        static char const *cmd3 = "DROP TABLE KVStore";
        rc = sqlite3_exec(db, cmd3, callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
                printf("SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
        }
#endif
        Genode::log("Finishing test");

        sqlite3_close(db);
        return 0;
}





void Libc::Component::construct(Libc::Env &env) { static Hello::Main inst(env); }

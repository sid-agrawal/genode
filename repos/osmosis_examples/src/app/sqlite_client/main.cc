
/*
 * \brief  Test client for the Hello RPC interface
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
#include <hello_session/connection.h>
#include <pd_session/connection.h>


void Component::construct(Genode::Env &env)
{

	Genode::log("Client started");
	Hello::Connection hello(env);
	Genode::log("2-Client started");

	hello.say_hello();

	Genode::log("3-Client started");
	int const sum = hello.add(2, 5);
	Genode::log("added 2 + 5 = ", sum);

	Genode::log("hello test completed");

	Genode::Pd_connection pd(env);
	pd.model_state();
}

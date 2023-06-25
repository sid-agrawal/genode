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




void Component::construct(Genode::Env &env)
{
	Genode::error(">>>>>>>>Obtaining Local connection");
	Hello::Connection hello(env);
	Genode::error("<<<<<<<<Obtaining Local connection");

	hello.say_hello();

	int const sum = hello.add(2, 5);
	Genode::log("added 2 + 5 = ", sum);

	Genode::log("hello test completed");
}

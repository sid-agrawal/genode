
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


void Component::construct(Genode::Env &env)
{

        void * ptr = &env;
	Genode::log("added 2 + 5 = ", ptr);

	Genode::log("hello test completed");
}

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
#include <base/child.h>
#include <root/component.h>
#include <hello_session/hello_session.h>
#include <base/rpc_server.h>
#include <base/attached_ram_dataspace.h>
#include <base/attached_rom_dataspace.h>

namespace Hello {
	struct Session_component;
	struct Root_component;
	struct Main;
}


struct Hello::Session_component : Genode::Rpc_object<Session>
{
	void say_hello() override {
		Genode::log("I am here... Hello."); }

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


struct Hello::Main
{
	Genode::Env &env;

	/*
	 * A sliced heap is used for allocating session objects - thereby we
	 * can release objects separately.
	 */
	Genode::Sliced_heap sliced_heap { env.ram(), env.rm() };

	Hello::Root_component root { env.ep(), sliced_heap };

	Main(Genode::Env &env) : env(env)
	{
		/*
		 * Create a RPC object capability for the root interface and
		 * announce the service to our parent.
		 */
		env.parent().announce(env.ep().manage(root));

		Genode::Attached_ram_dataspace _ds{env.ram(), env.rm(), 4096};

		// Genode::Capability_space::capid(_ds);

		Genode::Pd_connection pd(env);
		pd.model_state();

#if 0
		Genode::Region_map_client _address_space{pd.address_space()};
		Genode::Region_map_client _stack_area{pd.stack_area()};
		Genode::Region_map_client _linker_area{pd.linker_area()};
		/* dataspace used for creating shared memory between parent and child */
		Genode::Attached_ram_dataspace _ds{env.ram(), env.rm(), 4096};
		_address_space.attach(_ds.cap());

		Genode::log("In the Server");
		Genode::log("Address Space");
		_address_space.list();
		// Genode::log("Stack area");
		// _stack_area.list();
		// Genode::log("LInker area");
		// _linker_area.list();

		// Genode::log("here is the model state");
		// pd.model_state();
#endif
	}
};


void Component::construct(Genode::Env &env)
{
	static Hello::Main main(env);
}

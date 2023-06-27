/*
 * \brief  Test program for raising and handling region-manager faults
 * \author Norman Feske
 * \date   2008-09-24
 *
 * This program starts itself as child. When started, it first determines
 * wheather it is parent or child by requesting a RM session. Because the
 * program blocks all session-creation calls for the RM service, each program
 * instance can determine its parent or child role by the checking the result
 * of the session creation.
 */

/*
 * Copyright (C) 2008-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/component.h>
#include <base/log.h>
#include <base/child.h>
#include <rm_session/connection.h>
#include <root/component.h>
#include <base/attached_ram_dataspace.h>
#include <base/attached_rom_dataspace.h>

#include <hello_session/connection.h>
#include <pd_session/connection.h>

using namespace Genode;


namespace Hello {
	struct Session_component;
	struct Root_component;
}
/***********
 ** Child **
 ***********/
int encrypt(){
	Genode::log("foo, excrypted");
	return 0;
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

void main_child(Env &env)
{
	Genode::log("Starting child, calling foo: ", encrypt());
	Genode::Sliced_heap sliced_heap { env.ram(), env.rm() };

	Hello::Root_component root { env.ep(), sliced_heap };
		env.parent().announce(env.ep().manage(root));
}


/************
 ** Parent **
 ************/

class Test_child_policy : public Child_policy
{
	public:

		typedef Registered<Genode::Parent_service> Parent_service;
		typedef Registry<Parent_service>           Parent_services;

	private:

		Env                            &_env;
		Parent_services                &_parent_services;

		Service &_matching_service(Service::Name const &name)
		{
			Service *service = nullptr;
			_parent_services.for_each([&] (Service &s) {
				if (!service && name == s.name())
					service = &s; });

			if (!service)
				throw Service_denied();

			return *service;
		}

	public:

		/**
		 * Constructor
		 */
		Test_child_policy(Env &env, Parent_services &parent_services)
		:
			_env(env),
			_parent_services(parent_services)
		{ }


		/****************************
		 ** Child-policy interface **
		 ****************************/

		Name name() const override { return "lwc1"; }

		Binary_name binary_name() const override { return "lwc_sqlite_example"; }

		Pd_session           &ref_pd()           override { return _env.pd(); }
		Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

		void init(Pd_session &session, Pd_session_capability cap) override
		{
			session.ref_account(_env.pd_session_cap());

			_env.pd().transfer_quota(cap, Ram_quota{1*1024*1024});
			_env.pd().transfer_quota(cap, Cap_quota{20});

		}

		Route resolve_session_request(Service::Name const &name,
		                              Session_label const &label,
		                              Session::Diag const  diag) override
		{
			return Route { .service = _matching_service(name),
			               .label   = label,
			               .diag    = diag };
		}
};


struct Main_parent
{
	Env &_env;


	Heap _heap { _env.ram(), _env.rm() };

	Attached_rom_dataspace _config { _env, "config" };
	Rom_connection _binary { _env, "ld.lib.so" };

	/* parent services */
	struct Parent_services : Test_child_policy::Parent_services
	{
		Allocator &alloc;

		Parent_services(Env &env, Allocator &alloc) : alloc(alloc)
		{
			static const char *names[] = {
				"PD", "CPU", "ROM", "LOG", 0 };
			for (unsigned i = 0; names[i]; i++)
				new (alloc) Test_child_policy::Parent_service(*this, env, names[i]);
		}

		~Parent_services()
		{
			for_each([&] (Test_child_policy::Parent_service &s) { destroy(alloc, &s); });
		}
	} _parent_services { _env, _heap };

	/* create child */
	Test_child_policy _child_policy { _env, _parent_services };

	Child _child { _env.rm(), _env.ep().rpc_ep(), _child_policy };

	Region_map_client _address_space { _child.pd().address_space() };

	/* dataspace used for creating shared memory between parent and child */
	Attached_ram_dataspace _ds { _env.ram(), _env.rm(), 4096 };

	Main_parent(Env &env) : _env(env) {

		Genode::log("Starting parent");
		void main_parent(Env &env);
	 }
};


void main_parent(Env &env)
{
	Genode::log("-- parent role started --");

	Genode::log("Client started");
	Hello::Connection hello(env);
	Genode::log("2-Client started");

	hello.say_hello();
}

void Component::construct(Env &env)
{
	log("--- region-manager fault test ---");

	try {
		/*
		 * Distinguish parent from child by requesting an service that is only
		 * available to the parent.
		 */
		Rm_connection rm(env);
		static Main_parent parent(env);
		Genode::log("-- parent role started --");
	}
	catch (Service_denied) {
		main_child(env);
	}
}

/*

RPC_Cap LWC_Create(func to involve in the child, what to share ) {

	1. Create a new Child
    2. The new child will have a new service
	3. This new service will show up in the Test Child policy somehow.
}

LWC_SWITCH(RPC_Cap) {
	1. Switch to the child
	2. Call the function
	3. Switch back to the parent
}



*/
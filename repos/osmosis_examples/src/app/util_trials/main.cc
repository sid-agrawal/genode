/*
 * \brief  Trivial program to test util libs
 * \author Sid Agrawal
 * \date   2023-06-10
 */

/*
 * Copyright (C) 2023 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/component.h>
#include <base/log.h>
#include <base/heap.h>
#include <base/allocator_avl.h>
#include <root/component.h>
#include <base/rpc_server.h>
#include <base/rpc_server.h>



namespace UtilTrials {
	struct BlockType;
	struct Main;
}

struct UtilTrials::BlockType{
	private:

			Genode::addr_t _base;
			Genode::size_t _size;
			bool   _write;

	public:
		BlockType(Genode::addr_t base, Genode::size_t size, bool write)
		: _base(base), _size(size), _write(write) {}

		Genode::addr_t                    base() const { return _base;  }
		Genode::size_t                    size() const { return _size;  }
		bool                     write() const { return _write; }

		void print(Genode::Output &output) const {
			Genode::print(output, "BlockType: base:", _base,
						  " size:", _size, " write:", _write);
		}
};


#define PAGE_SIZE 4096

void cap_tree_test(Genode::Env &/*env*/)
{

		// Kernel::Object_identity_reference_tree cap_tree;

		// cap_tree.for_each([&](Kernel::Object_identity_reference const &oir)
		// 				  { Genode::log("cap_tree: ", (long)oir.capid()); });
}

void avl_test(Genode::Env &env)
{


	Genode::Sliced_heap sliced_heap { env.ram(), env.rm() };
	Genode::Allocator_avl_tpl<UtilTrials::BlockType>  _map(&sliced_heap);


	Genode::addr_t addr1 = 0;
	Genode::addr_t addr2 = 16*PAGE_SIZE;
	Genode::addr_t addr3 = PAGE_SIZE;

	_map.add_range(addr1, 10*PAGE_SIZE);
	_map.add_range(addr2, 3*PAGE_SIZE);

	Genode::log("Before of map:");
	Genode::log(_map);

	// _map.alloc_addr(0x1000, 17*PAGE_SIZE);

	using Alloc_error = Genode::Range_allocator::Alloc_error;
	_map.alloc_addr(PAGE_SIZE, addr3).with_result(

		[&](void *)
		{
			/* store attachment info in meta data */
			try
			{
				_map.construct_metadata(
					(void *)addr3,
					addr3,
					(Genode::size_t)PAGE_SIZE,
					true);
			}
			catch (Genode::Allocator_avl_tpl<UtilTrials::BlockType>::Assign_metadata_failed)
			{
				Genode::log("failed to store attachment info, exiting");
				return;
			}

			UtilTrials::BlockType &b = *_map.metadata((void *)addr3);
			Genode::log("Constructed block details:", b);
		},

		[&](Alloc_error error)
		{
			switch (error)
			{

			case Alloc_error::OUT_OF_RAM:
			{
				Genode::log("OUT_OF_RAM");
				return;
			}
			case Alloc_error::OUT_OF_CAPS:
			{
				Genode::log("OUT_OF_CAPS");
				return;
			}
			case Alloc_error::DENIED:
			{
				Genode::log("DENIED");
				return;
			}
			};
		});
	Genode::log("After of alloc:");
	Genode::log(_map);

	_map.for_each_metadata([](UtilTrials::BlockType &block) {
		Genode::log("goo bar", block);
	});

}

struct UtilTrials::Main
{
	Genode::Env &_env;


	Main(Genode::Env &env) : _env(env)
	{
		/*
		 * Create a RPC object capability for the root interface and
		 * announce the service to our parent.
		 */
        Genode::log("Inside our util trial class", &_env);
		// avl_test(_env);
		// cap_tree_test(_env);

	}
};


void Component::construct(Genode::Env &env) {
        static UtilTrials::Main main(env);
}

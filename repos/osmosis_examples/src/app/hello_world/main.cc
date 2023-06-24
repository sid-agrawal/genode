/*
 * \brief  Trivial Hello World
 * \author Sid Agrawal
 * \date   2023-06-19
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

int foo(int a) {
        return a + 1;
}
void Component::construct(Genode::Env &env)
{
        int x = 7;
        Genode::log("Hello F**ing world!: addr of foo: ", (void *)foo,
        " and stack(x)", &x);

        Genode::Pd_connection pd(env);


        Genode::Region_map_client _address_space{pd.address_space()};
        // Genode::Region_map_client _stack_area{pd.stack_area()};
        // Genode::Region_map_client _linker_area{pd.linker_area()};

        Genode::error(">>>>>>>>>>>>>>>>>>>>>>>>> Diving into DS Allocation");
        Genode::Attached_ram_dataspace _ds{env.ram(), env.rm(), 2*4096};
        Genode::error(">>>>>>>>>>>>>>>>>>>>>>>>> Diving out of DS Allocation");


        Genode::log(">>>>>>>>>>>>>>>>>>>>>>>>> Diving into attach");
        _address_space.attach_at(_ds.cap(), 0x7000);
        Genode::log("<<<<<<<<<<<<<<<<<<<<<<<<<< Back from attach");

        Genode::error(">>>>>>>>>>>>>>>>>>>>>>>>> Diving into model_state");
        pd.model_state();
        Genode::error(">>>>>>>>>>>>>>>>>>>>>>>>> Returning from model_state");
        // Genode::log(">>>>>>>>>>-START---Address Space------SEEN FROM CHILD-------------");
        // Genode::log("Address Space");
        // _address_space.list();
        // Genode::log("STACK Area");
        // _stack_area.list();
        // Genode::log("Linker Area");
        // _linker_area.list();
        // Genode::log("<<<<<<<<<<-END-----Address Space------SEEN FROM CHILD-------------");
        // // // _address_space.detach((void *)0x08);

        // // Genode::log("Linker Space");
        // // _linker_area.list();

        // Genode::log(">>>>>>>>>>>START---Address Space------SEEN FROM parent-------------");
        // env.parent().rm_list();
        // Genode::log(" <<<<<<<<<<<-END---Address Space------SEEN FROM parent-------------");

}

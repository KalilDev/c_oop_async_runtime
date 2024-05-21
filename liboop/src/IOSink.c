#include "liboop.h"
#include <assert.h>


#define Self IOSink

SUPER_INTERFACE_CAST_IMPL(IOSink, Sink)
OBJECT_CAST_IMPL(IOSink, Object)

ENUMERATE_IO_SINK_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
        (Interface_vtable_t*)vtable,
        (Interface_vtable_t*)Sink_vtable(),
        sizeof(*Sink_vtable())
    );
}

#undef Self
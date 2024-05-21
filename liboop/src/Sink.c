#include "liboop.h"
#include <assert.h>


#define Self Sink

SUPER_INTERFACE_CAST_IMPL(Sink, Interface)
OBJECT_CAST_IMPL(Sink, Object)

ENUMERATE_SINK_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
        (Interface_vtable_t*)vtable,
        Interface_vtable(),
        sizeof(*Interface_vtable())
    );
}

#undef Self
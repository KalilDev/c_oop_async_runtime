#include "liboop.h"
#include <assert.h>

#define Self Closeable

SUPER_INTERFACE_CAST_IMPL(Closeable, Interface)
OBJECT_CAST_IMPL(Closeable, Object)

ENUMERATE_CLOSEABLE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_VTABLE() {
    initInterfaceVtable(
            (Interface_vtable_t*)vtable,
            (Interface_vtable_t*)Interface_vtable(),
            sizeof(*Interface_vtable())
    );
}

#undef Self
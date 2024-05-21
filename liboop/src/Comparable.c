#include "liboop.h"
#include <assert.h>


#define Self Comparable

SUPER_INTERFACE_CAST_IMPL(Comparable, Interface)
OBJECT_CAST_IMPL(Comparable, Object)

ENUMERATE_COMPARABLE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
    (Interface_vtable_t*)vtable,
    (Interface_vtable_t*)Interface_vtable(),
    sizeof(*Interface_vtable())
    );
}

#undef Self
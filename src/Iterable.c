#include "Iterable.h"
#include "Iterator.h"
#include "oop.h"
#include <assert.h>


#define Self Iterable

SUPER_INTERFACE_CAST_IMPL(Iterable, Interface)
OBJECT_CAST_IMPL(Iterable, Object)

ENUMERATE_ITERABLE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
        (Interface_vtable_t*)vtable,
        Interface_vtable(),
        sizeof(*Interface_vtable())
    );
}

#undef Self
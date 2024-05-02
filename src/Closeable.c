#include "Closeable.h"
#include "oop.h"

static const Closeable_vtable_t _Closeable_vtable = {0};

const Closeable_vtable_t* Closeable_vtable() {
    return &_Closeable_vtable;
}

SUPER_INTERFACE_CAST_IMPL(Closeable, Interface)
VIRTUAL_METHOD_IMPL(Closeable, close, void, (Closeable this), (this))

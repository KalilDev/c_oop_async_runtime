#include "Iterable.h"
#include "oop.h"

static const Iterable_vtable_t _Iterable_vtable = {0};

const Iterable_vtable_t* Iterable_vtable() {
    return &_Iterable_vtable;
}

SUPER_INTERFACE_CAST_IMPL(Iterable, Interface)
OBJECT_CAST_IMPL(Iterable, Object)

VIRTUAL_METHOD_IMPL(Iterable, iterator, Iterator, (Iterable this), (this))

#include "Interface.h"
#include "oop.h"

static const Interface_vtable_t _Interface_vtable = {0};

const Interface_vtable_t* Interface_vtable() {
    return &_Interface_vtable;
}
OBJECT_CAST_IMPL(Interface, Object)

#include "Interface.h"
#include "oop.h"
#include <assert.h>
#include <string.h>

#define Self Interface
void initInterfaceVtable(Interface_vtable_t * selfVtable, const  Interface_vtable_t *superVtable, size_t superVtableSize) {
    assert(selfVtable != NULL);
    assert(superVtable != NULL);
    assert(selfVtable->interface_vtable_tag != INTERFACE_VTABLE_TAG);
    assert(superVtable->interface_vtable_tag == INTERFACE_VTABLE_TAG);
    if (superVtable != NULL) {
        memcpy(selfVtable, superVtable, superVtableSize);
    }
    selfVtable->interface_vtable_tag = INTERFACE_VTABLE_TAG;
}
void initImplementedInterfaceVtable(Interface_vtable_t * implementedVtable, const Interface_vtable_t *baseVtable, size_t implementedVtableSize, size_t offset) {
    assert(implementedVtable != NULL);
    assert(baseVtable != NULL);
    assert(implementedVtable->interface_vtable_tag != INTERFACE_VTABLE_TAG);
    assert(baseVtable->interface_vtable_tag == INTERFACE_VTABLE_TAG);
    memcpy(implementedVtable, baseVtable, implementedVtableSize);
    implementedVtable->object_vtable_offset = offset;
}

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    vtable->interface_vtable_tag = INTERFACE_VTABLE_TAG;
}

OBJECT_CAST_IMPL(Interface, Object)

#undef Self
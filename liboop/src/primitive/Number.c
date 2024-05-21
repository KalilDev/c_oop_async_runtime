#include "liboop.h"
#include <assert.h>
#include <stddef.h>

#define Self Number
#define Super() Object_vtable()

ENUMERATE_NUMBER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_NUMBER_PARENTS)

IMPLEMENT_SELF_VTABLE() {
    // Init the vtable
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            1,
            "Comparable",
            offsetof(Number_vtable_t, Comparable_vtable)
    );

    // Comparable
    Comparable_vtable_t *comparable_vtable = &vtable->Comparable_vtable;
    initImplementedInterfaceVtable(
    (Interface_vtable_t*)comparable_vtable,
    (Interface_vtable_t*)Comparable_vtable(),
    sizeof(*Comparable_vtable()),
    offsetof(Number_vtable_t, Comparable_vtable)
    );
}

#undef Super
#undef Self
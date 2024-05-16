#include "Stream.h"
#include "StreamSubscription.h"
#include "Function.h"
#include "primitive/Bool.h"
#include "oop.h"
#include <assert.h>


#define Self Stream

SUPER_INTERFACE_CAST_IMPL(Stream, Interface)
OBJECT_CAST_IMPL(Stream, Object)

ENUMERATE_STREAM_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
        (Interface_vtable_t*)vtable,
        Interface_vtable(),
        sizeof(*Interface_vtable())
    );
}

#undef Self
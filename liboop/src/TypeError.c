#include "liboop.h"
#include <assert.h>
#include <string.h>

#define Super() Error_vtable()
#define Self TypeError
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_TYPE_ERROR_PARENTS)

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // TypeError
    // Error
    // Throwable
    // Object
}

IMPLEMENT_CONSTRUCTOR(new, Object obj, String wantedType) {
    String message = String_format_c("{} is not an instance of {}", obj, wantedType);
    Error$new(this.asError, message);
}

#undef Super
#undef Self
#include "liboop.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self Function

Object Function_call(Function this, size_t argc, ...) {
    assert(this.vtable->call != NULL);
    va_list args;
    va_start(args, argc);
    Object res = this.vtable->call(this, argc, args);
    va_end(args);
    return res;
}

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_FUNCTION_PARENTS)

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // Function
}

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new) {

}

#undef Super
#undef Self
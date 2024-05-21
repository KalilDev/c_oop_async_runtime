#include "liboop.h"
#include <assert.h>
#include <string.h>
#include <errno.h>

#define Super() Exception_vtable()
#define Self IOException
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_IO_EXCEPTION_PARENTS)

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // IOException
    // Exception
    // Throwable
    // Object
}

IMPLEMENT_CONSTRUCTOR(new, int num) {
    String message = StringRef$wrap(strerror(num)).asString;
    Exception$new(this.asException, message);
}


IMPLEMENT_CONSTRUCTOR(fromErrno) {
    IOException$new(this, errno);
}

#undef Super
#undef Self
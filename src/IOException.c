#include "Object.h"
#include "Throwable.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "Exception.h"
#include "IOException.h"
#include <assert.h>
#include <string.h>
#include <errno.h>

#define Super() Exception_vtable()
#define Self IOException
IMPLEMENT_OPERATOR_NEW()

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

SUPER_CAST_IMPL(IOException, Exception)
UPCAST_IMPL(IOException, Throwable)
UPCAST_IMPL(IOException, Object)

IMPLEMENT_CONSTRUCTOR(new, int num) {
    String message = StringRef_as_String(StringRef$wrap(strerror(num)));
    Exception$new(IOException_as_Exception(this), message);
}


IMPLEMENT_CONSTRUCTOR(fromErrno) {
    IOException$new(this, errno);
}

#undef Super
#undef Self
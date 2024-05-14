#include "Object.h"
#include "Function.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
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

IMPLEMENT_LAMBDA(MyLambda, NO_CAPTURES, NO_OWNED_CAPTURES) {
    Lambda_MyLambda self = DOWNCAST(this, Lambda_MyLambda);
    assert(argc == 2);
}
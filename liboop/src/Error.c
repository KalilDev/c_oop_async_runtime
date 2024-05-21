#include "liboop.h"
#include <assert.h>

#define Super() Throwable_vtable()
#define Self Error
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_ERROR_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    Error self = DOWNCAST(this, Error);
    Object_delete(self.data->message.asObject);
    ((Object_vtable_t*)Super())->delete(this);
}
IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Error self = DOWNCAST(this, Error);
    const char* class_name = "Error";
    #ifdef WITH_RTTI
    class_name = self.asObject.vtable->runtime_type_information.class_name;
    #endif
    return String_format_c("{}: {}", StringRef$wrap(class_name), self.data->message);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            0);
    // Error
    // Throwable
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Error_delete_impl;
    object_vtable->toString = _Error_toString_impl;
}

IMPLEMENT_CONSTRUCTOR(new, String message) {
    Throwable$new(this.asThrowable);
    this.data->message = message;
}

IMPLEMENT_CONSTRUCTOR(newCString, const char* message) {
    Error$new(this, StringRef$wrap(message).asString);
}

#undef Super
#undef Self
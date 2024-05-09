#include "Object.h"
#include "Throwable.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "Error.h"
#include <assert.h>

#define Super() Throwable_vtable()
#define Self Error
IMPLEMENT_OPERATOR_NEW()

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

SUPER_CAST_IMPL(Error, Throwable)
UPCAST_IMPL(Error, Object)

IMPLEMENT_CONSTRUCTOR(new, String message) {
    Throwable$new(Error_as_Throwable(this));
    this.data->message = message;
}

#undef Super
#undef Self
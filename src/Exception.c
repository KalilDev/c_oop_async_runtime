#include "Object.h"
#include "Throwable.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "Exception.h"
#include <assert.h>

#define Super() Throwable_vtable()
#define Self Exception
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    Exception self = DOWNCAST(this, Exception);
    Object_delete(self.data->message.asObject);
    ((Object_vtable_t*)Super())->delete(this);
}
IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Exception self = DOWNCAST(this, Exception);
    const char* class_name = "Exception";
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
    // Exception
    // Throwable
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Exception_delete_impl;
    object_vtable->toString = _Exception_toString_impl;
}

SUPER_CAST_IMPL(Exception, Throwable)
UPCAST_IMPL(Exception, Object)

IMPLEMENT_CONSTRUCTOR(new, String message) {
    Throwable$new(Exception_as_Throwable(this));
    this.data->message = message;
}

#undef Super
#undef Self
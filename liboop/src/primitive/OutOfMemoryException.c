#include "liboop.h"

#define Self OutOfMemoryException
#define Super() Exception_vtable()

static const char* _ToString = "OutOfMemoryException()";

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_OUT_OF_MEMORY_EXCEPTION_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    return StringRef$wrap(_ToString).asString;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {

}
IMPLEMENT_OVERRIDE_METHOD(void, Throwable, addStackFrame, StringRef stackFrame) {

}
IMPLEMENT_OVERRIDE_METHOD(void, Throwable, printStackTrace, FILE *to) {
    OutOfMemoryException self = DOWNCAST(this, OutOfMemoryException);
    fprintf(to, "0# %s\n", self.unwrap);
}



static const OutOfMemoryException_vtable_t _OutOfMemoryException_vtable = {
        .super = {
                .super = {
                        .super = {
                                .object_vtable_tag = OBJECT_VTABLE_TAG,
                                .runtime_type_information = {
                                        .class_name = "OutOfMemoryException",
                                        .interface_table = NULL,
                                        .inheritance_chain = "Object;Throwable;Exception;OutOfMemoryException"
                                },
                                .toString = _OutOfMemoryException_toString_impl,
                                .delete = _OutOfMemoryException_delete_impl
                        },
                        .addStackFrame = _OutOfMemoryException_addStackFrame_impl,
                        .printStackTrace = _OutOfMemoryException_printStackTrace_impl
                }
        }
};

const OutOfMemoryException_vtable_t* OutOfMemoryException_vtable() {
    return &_OutOfMemoryException_vtable;
}

const OutOfMemoryException OutOfMemoryException_atUnknownLocation =  {
        .vtable = &_OutOfMemoryException_vtable,
        .unwrap = "<unknown source location>"
};

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(at, const char* location) {
    OutOfMemoryException this = {
        .vtable = OutOfMemoryException_vtable(),
        .unwrap = location
    };
    return this;
}

#undef Super
#undef Self
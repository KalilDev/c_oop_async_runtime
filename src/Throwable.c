#include "Object.h"
#include "Throwable.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self Throwable
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_THROWABLE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    Throwable self = DOWNCAST(this, Throwable);
    Object_delete(self.data->frames.asObject);
    Super()->delete(this);
}
IMPLEMENT_SELF_METHOD(void, addStackFrame, StringRef stackFrame) {
    List_add(this.data->frames, stackFrame.asObject);
}
IMPLEMENT_SELF_METHOD(void, printStackTrace, FILE *to) {
    size_t i = 0;
    foreach(String, frame, List_as_Iterable(this.data->frames), {
        fprintf(to, "%ld# %s\n", i++, String_cStringView(frame));
    })
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // Throwable
    vtable->printStackTrace = _Throwable_printStackTrace_impl;
    vtable->addStackFrame = _Throwable_addStackFrame_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Throwable_delete_impl;
}

SUPER_CAST_IMPL(Throwable, Object)

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new) {
    this.data->frames = List_new();
}

#undef Super
#undef Self
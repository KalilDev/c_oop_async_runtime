#include "Object.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Integer.h"
#include "../oop.h"
#include "stddef.h"
#include "../oop.h"
#include "string.h"
#include <strings.h>
#include <assert.h>
#define Self StringRef
#define Super() String_vtable()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_STRING_REF_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {

}

IMPLEMENT_OVERRIDE_METHOD(size_t, String, length) {
    StringRef self = DOWNCAST(this, StringRef);
    return strlen(self.unwrap);
}
IMPLEMENT_OVERRIDE_METHOD(const char*, String, cStringView) {
    StringRef self = DOWNCAST(this, StringRef);
    return self.unwrap;
}
IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    StringRef self = DOWNCAST(this, StringRef);
    return self.asString;
}

IMPLEMENT_SELF_VTABLE() {
    // Init the vtable
    initVtable(
    (Object_vtable_t*)vtable,
    (Object_vtable_t*)Super(),
    sizeof(*Super()),
    STR(Self),
    0
    );

    // String
    String_vtable_t *string_vtable = (String_vtable_t *)vtable;
    string_vtable->length = _StringRef_length_impl;
    string_vtable->cStringView = _StringRef_cStringView_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _StringRef_delete_impl;
    object_vtable->toString = _StringRef_toString_impl;
}

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(wrap, const char* ref) {
    if (ref == NULL) {
        return DOWNCAST(null, StringRef);
    }
    StringRef this = {
        .vtable = StringRef_vtable(),
        .unwrap = ref
    };
    return this;
}

#undef Super
#undef Self
#include "Object.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Integer.h"
#include "../oop.h"
#include "stddef.h"
#include "../oop.h"
#include "string.h"
#include <strings.h>
#define Super() String_vtable()

OVERRIDE_IMPL(void, StringRef, delete, Object) {
}

OVERRIDE_IMPL(const char*, StringRef, cStringView, String) {
    StringRef self = DOWNCAST(this, StringRef);
    if (self.data == NULL) {
        return NULL;
    }
    return strdup(self.data);
}

OVERRIDE_IMPL(size_t, StringRef, length, String) {
    StringRef self = DOWNCAST(this, StringRef);
    return strlen(self.data);
}


OVERRIDE_IMPL(Integer, StringRef, at, Integer, size_t i) {
    StringRef self = DOWNCAST(this, StringRef);
    size_t length = strlen(self.data);
    if (i >= length) {
        return DOWNCAST(Object_null, Integer);
    }
    const char* c_string = self.data;
    return Integer_box(c_string[i]);
}

static StringRef_vtable_t _StringRef_vtable = {0};

const StringRef_vtable_t* StringRef_vtable() {
    if (_StringRef_vtable.super.at == NULL) {
        // String
        memmove(&_StringRef_vtable.super, Super(), sizeof(*Super()));
        _StringRef_vtable.super.cStringView = _StringRef_cStringView;
        _StringRef_vtable.super.length = _StringRef_length;
        // Object
        _StringRef_vtable.super.super.delete = _StringRef_delete;
    }
    return &_StringRef_vtable;
}

PRIMITIVE_SUPER_CAST_IMPL(StringRef, String)
PRIMITIVE_UPCAST_IMPL(StringRef, Object)

StringRef StringRef_alloc() {
    StringRef ptr = {
            .vtable = StringRef_vtable(),
            .data = NULL
    };
    return ptr;
}

void StringRef_construct(StringRef this, const char* ref) {
    String_construct(StringRef_as_String(this), ref);
}

StringRef StringRef_new(const char* ref) {
    StringRef this = StringRef_alloc();
    if (Object_isNull(StringRef_as_Object(this))) {
        return this;
    }
    StringRef_construct(this, ref);
    return this;
}

#undef Super
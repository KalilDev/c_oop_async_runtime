#include "Object.h"
#include "String.h"
#include "primitive/Integer.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include <strings.h>
#define Super() Object_vtable()

const String_vtable_t* String_vtable();
//
//METHOD_IMPL(String, add, void, (String this, Object e), (this, e)) {
//    size_t *length = &this.data->length;
//    String_ensure(this, *length + 1);
//    Object* elements = this.data->elements;
//    // TODO: Document this behavior! (pass by reference)
//    memcpy(elements + *length, &e, sizeof(Object));
//    *length += 1;
//}
//METHOD_IMPL(String, ensure, void, (String this, size_t min_capacity), (this, min_capacity)) {
//    size_t capacity = this.data->capacity;
//    if (capacity >= min_capacity) {
//        return;
//    }
//    // Assume the initial capacity to be 4
//    size_t new_capacity = capacity == 0 ? 4 : capacity;
//    while (new_capacity < min_capacity) {
//        new_capacity <<= 1;
//    }
//    Object* old_elements = this.data->elements;
//    Object* new_elements = realloc(old_elements, new_capacity * sizeof(Object));
//    if (new_elements == NULL) {
//        // TODO: throw
//    }
//    this.data->elements = new_elements;
//    this.data->capacity = new_capacity;
//}

METHOD_IMPL(String, at, Integer, (String this, size_t i), (this, i)) {
    size_t length = this.data->length;
    if (i >= length) {
        return DOWNCAST(Object_null, Integer);
    }
    char* c_string = this.data->c_string;
    return Integer_box(c_string[i]);
}

METHOD_IMPL(String, length, size_t, (String this), (this)) {
    return this.data->length;
}

METHOD_IMPL(String, cStringView, const char*, (String this), (this)) {
    return this.data->c_string;
}


METHOD_IMPL(String, concat, String, (String this, String other), (this, other)) {
    const char* a = String_cStringView(this);
    size_t aLen = String_length(this);
    const char* b = String_cStringView(other);
    size_t bLen = String_length(other);
    if (aLen == 0) {
        return String_new(b);
    }
    if (bLen == 0) {
        return String_new(a);
    }
    char *res = malloc(aLen + bLen + 1);
    memcpy(res, a, aLen);
    memcpy(res + aLen, b, bLen);
    res[aLen+bLen] = '\0';
    return String_own_len(res, aLen + bLen);
}

OVERRIDE_IMPL(void , String, delete, Object) {
    String self = DOWNCAST(this, String);
    char* c_string = self.data->c_string;
    if (c_string == NULL) {
        Super()->delete(this);
        return;
    }

    // Delete the array
    free(c_string);
    Super()->delete(this);
}
//
//Iterator _String_iterator(Iterable this) {
//    String self = Iterable_as_String(this);
//    return StringIterator_as_Iterator(StringIterator_new(self));
//}

static String_vtable_t _String_vtable = {0};

const String_vtable_t* String_vtable() {
    if (_String_vtable.at == NULL) {
        // String
        _String_vtable.at = _String_at;
        _String_vtable.cStringView = _String_cStringView;
        _String_vtable.length = _String_length;
        _String_vtable.concat = _String_concat;
//        // Iterable
//        _String_vtable.Iterable_vtable.super.vtable_offset = offsetof(String_vtable_t, Iterable_vtable);
//        _String_vtable.Iterable_vtable.iterator = _String_iterator;
        // Object
        memmove(&_String_vtable.super, Super(), sizeof(*Super()));
        _String_vtable.super.delete = _String_delete;
    }
    return &_String_vtable;
}

//OBJECT_CAST_IMPL(Iterable, String)
//INTERFACE_CAST_IMPL(String, Iterable, Object)

SUPER_CAST_IMPL(String, Object)

String String_alloc() {
    String ptr = {
            .vtable = String_vtable(),
            .data = Object_allocate(sizeof(String_data))
    };
    if (ptr.data == NULL) {
        return DOWNCAST(Object_null, String);
    }
    return ptr;
}

void String_construct(String this, const char* ref) {
    this.data->length = strlen(ref);
    this.data->c_string = strdup(ref);
}
void String_construct_own(String this, char* cString) {
    String_construct_own_len(this, cString, strlen(cString));
}
void String_construct_own_len(String this, char* cString, size_t len) {
    this.data->length = len;
    this.data->c_string = cString;
}

String String_new(const char* ref) {
    String this = String_alloc();
    if (Object_isNull(String_as_Object(this))) {
        return this;
    }
    String_construct(this, ref);
    return this;
}

String String_own(char* cString) {
    String this = String_alloc();
    if (Object_isNull(String_as_Object(this))) {
        return this;
    }
    String_construct_own(this, cString);
    return this;
}
String String_own_len(char* cString, size_t len) {
    String this = String_alloc();
    if (Object_isNull(String_as_Object(this))) {
        return this;
    }
    String_construct_own_len(this, cString, len);
    return this;
}


#undef Super
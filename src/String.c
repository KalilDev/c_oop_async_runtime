#include "Object.h"
#include "String.h"
#include "primitive/Integer.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "Iterator.h"
#include "StringIterator.h"
#include "StringBuffer.h"
#include "primitive/StringRef.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self String

ENUMERATE_STRING_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_OPERATOR_NEW()

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

IMPLEMENT_SELF_METHOD(Integer, at, size_t i) {
    size_t length = String_length(this);
    if (i >= length) {
        return DOWNCAST(null, Integer);
    }
    const char* c_string = String_cStringView(this);
    return Integer$box(c_string[i]);
}

IMPLEMENT_SELF_METHOD(size_t, length) {
    return this.data->length;
}

IMPLEMENT_SELF_METHOD(const char*, cStringView) {
    return this.data->c_string;
}


IMPLEMENT_SELF_METHOD(String, concat, String other) {
    const char* a = String_cStringView(this);
    size_t aLen = String_length(this);
    const char* b = String_cStringView(other);
    size_t bLen = String_length(other);
    if (aLen == 0) {
        return String$make_new(b);
    }
    if (bLen == 0) {
        return String$make_new(a);
    }
    char *res = malloc(aLen + bLen + 1);
    memcpy(res, a, aLen);
    memcpy(res + aLen, b, bLen);
    res[aLen+bLen] = '\0';
    return String$make_own_len(res, aLen + bLen);
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    String self = DOWNCAST(this, String);
    char* c_string = (char*)self.data->c_string;
    if (c_string == NULL) {
        Super()->delete(this);
        return;
    }

    // Delete the array
    free(c_string);
    Super()->delete(this);
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    String self = DOWNCAST(this, String);

    return String$make_new_len(self.data->c_string, self.data->length);
}

IMPLEMENT_OVERRIDE_METHOD(Iterator, Iterable, iterator) {
    String self = Iterable_as_String(this);
    StringIterator iter = StringIterator$make_new(self);
    return StringIterator_as_Iterator(iter);
}

IMPLEMENT_SELF_VTABLE() {
    // Init the vtable
    size_t iterable_vtable_offset = offsetof(String_vtable_t, Iterable_vtable);
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            1,
            "Iterable",
            iterable_vtable_offset
    );

    // String
    vtable->at = _String_at_impl;
    vtable->cStringView = _String_cStringView_impl;
    vtable->length = _String_length_impl;
    vtable->concat = _String_concat_impl;
    // Iterable
    initImplementedInterfaceVtable(
    (Interface_vtable_t*)&vtable->Iterable_vtable,
    (Interface_vtable_t*)Iterable_vtable(),
    sizeof(*Iterable_vtable()),
    offsetof(String_vtable_t, Iterable_vtable)
    );
    vtable->Iterable_vtable.iterator = _String_iterator_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _String_delete_impl;
    object_vtable->toString = _String_toString_impl;
}

OBJECT_CAST_IMPL(Iterable, String)
INTERFACE_CAST_IMPL(String, Iterable, Object)

SUPER_CAST_IMPL(String, Object)


IMPLEMENT_CONSTRUCTOR(new, const char* str) {
    this.data->length = strlen(str);
    this.data->c_string = strdup(str);
}


IMPLEMENT_CONSTRUCTOR(new_len, const char* str, size_t len) {
    assert(len == strlen(str));
    this.data->length = len;
    this.data->c_string = strndup(str, len);
}

IMPLEMENT_CONSTRUCTOR(own, char* ownStr) {
    this.data->length = strlen(ownStr);
    this.data->c_string = ownStr;
}

IMPLEMENT_CONSTRUCTOR(own_len, char* ownStr, size_t len) {
    this.data->length = len;
    this.data->c_string = ownStr;
}

typedef enum FormatStringState {
    S_READING_CHAR,
    S_READING_ESCAPED_CHAR,
    S_BRACKET_OPEN
} FormatStringState;

typedef enum FormatStringCharacterType {
    T_ESCAPE,
    T_BRACKET_OPEN,
    T_BRACKET_CLOSE,
    T_REGULAR
} FormatStringCharacterType;

FormatStringCharacterType _characterType(char c) {
    switch (c) {
        case '\\':
            return T_ESCAPE;
        case '{':
            return T_BRACKET_OPEN;
        case '}':
            return T_BRACKET_CLOSE;
        default:
            return T_REGULAR;
    }
}
void _String_format(StringBuffer buffer, const char* string, size_t len, va_list args) {
    FormatStringState state = S_READING_CHAR;
    for (size_t i = 0; i < len; i++) {
        char c =  string[i];
        FormatStringCharacterType type = _characterType(c);
        switch (state) {
            case S_READING_CHAR: {
                switch (type) {
                    case T_ESCAPE:
                        goto transition_to_escape;
                    case T_BRACKET_OPEN:
                        goto transition_to_bracket_open;
                    case T_BRACKET_CLOSE:
                    case T_REGULAR:
                        StringBuffer_writeCharCode(buffer, c);
                        goto no_transition;
                }
            }
            case S_READING_ESCAPED_CHAR:
                StringBuffer_writeCharCode(buffer, c);
                goto transition_to_regular;
            case S_BRACKET_OPEN:
                switch (type) {
                    case T_ESCAPE:
                        StringBuffer_writeCharCode(buffer, '{');
                        goto transition_to_escape;
                    case T_BRACKET_OPEN:
                        StringBuffer_writeCharCode(buffer, '{');
                        goto no_transition;
                    case T_BRACKET_CLOSE: {
                        Object arg = va_arg(args, Object);
                        StringBuffer_write(buffer, arg);
                        goto transition_to_regular;
                    }
                    case T_REGULAR:
                        StringBuffer_writeCharCode(buffer, '{');
                        StringBuffer_writeCharCode(buffer, c);
                        goto transition_to_regular;
                }
        }

        transition_to_escape:
        state = S_READING_ESCAPED_CHAR;
        goto no_transition;

        transition_to_bracket_open:
        state = S_BRACKET_OPEN;
        goto no_transition;

        transition_to_regular:
        state = S_READING_CHAR;
        goto no_transition;

        no_transition:
        continue;
    }
}


String String_format(String this, ...) {
    if (Object_isNull(String_as_Object(this))) {
        return this;
    }
    if (String_length(this) == 0) {
        return this;
    }

    StringBuffer buffer = StringBuffer$make_new();
    size_t len = String_length(this);
    const char *string = String_cStringView(this);

    va_list args;
    va_start(args, this);
    _String_format(buffer, string, len, args);
    va_end(args);

    String out = StringBuffer_releaseToString(buffer);
    Object_delete(StringBuffer_as_Object(buffer));
    return out;
}

String String_format_c(const char* str, ...) {
    if (str == NULL) {
        return DOWNCAST(null, String);
    }
    size_t len = strlen(str);
    if (len == 0) {
        return StringRef_as_String(StringRef$wrap(str));
    }

    StringBuffer buffer = StringBuffer$make_new();

    va_list args;
    va_start(args, str);
    _String_format(buffer, str, len, args);
    va_end(args);

    String out = StringBuffer_releaseToString(buffer);
    Object_delete(StringBuffer_as_Object(buffer));
    return out;
}


#undef Super
#undef Self
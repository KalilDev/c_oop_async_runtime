#include <strings.h>
#include <assert.h>
#include "liboop.h"

#define Super() Object_vtable()
#define Self StringBuffer
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_STRING_BUFFER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_STRING_BUFFER_PARENTS)

IMPLEMENT_SELF_METHOD(void, clear) {
    if (this.data->length > 0) {
        this.data->c_string[0] = '\0';
    }
    this.data->length = 0;
}

IMPLEMENT_SELF_METHOD(String, releaseToString) {
    char *cstring = this.data->c_string;
    size_t len = this.data->length;
    this.data->c_string = NULL;
    this.data->capacity = 0;
    this.data->length = 0;
    return String$make_own_len(cstring, len);
}

IMPLEMENT_SELF_METHOD(UInt8List, toBytes) {
    char *cstring = this.data->c_string;
    size_t len = this.data->length;

    return UInt8List$make_fromBuffer((unsigned char *)cstring, len);
}

IMPLEMENT_SELF_METHOD(UInt8List, releaseToBytes) {
    char *cstring = this.data->c_string;
    size_t len = this.data->length;
    this.data->c_string = NULL;
    this.data->capacity = 0;
    this.data->length = 0;

    return UInt8List$make_fromBuffer((unsigned char *)cstring, len);
}

IMPLEMENT_SELF_METHOD(void, write, Object obj) {
    if (Object_isNull(obj)) {
        StringBuffer_writeCString(this, "null");
        return;
    }
    // is string
    if (obj.vtable == String_vtable()) {
        String s = DOWNCAST(obj, String);
        StringBuffer_writeCString(this, String_cStringView(s));
        return;
    }
    // is bool
    if (obj.vtable == Bool_vtable()) {
        Bool b = DOWNCAST(obj, Bool);
        if (b.unwrap) {
            StringBuffer_writeCString(this, "true");
            return;
        }
        StringBuffer_writeCString(this, "false");
        return;
    }
    // general case
    String s = Object_toString(obj);
    StringBuffer_writeCString(this, String_cStringView(s));
    Object_delete(s.asObject);
}

IMPLEMENT_SELF_METHOD(void, writeAll, List list, String separator) {
    size_t i = 0;
    size_t length = List_length(list);
    Object sep = separator.asObject;

    StringBuffer_writeCharCode(this, '[');

    foreach(Object, obj, List_as_Iterable(list), {
        StringBuffer_write(this, obj);
        if (i + 1 != length) {
            StringBuffer_write(this, sep);
        }
        i++;
    })

    StringBuffer_writeCharCode(this, ']');
}
Object Object$$fromObject(Object this);
IMPLEMENT_SELF_METHOD(void, writeCharCode, char code) {
    if (code == '\0') {
        return;
    }
    size_t selfLen = this.data->length;
    StringBuffer_ensure(this, selfLen + 1);
    char* selfString = this.data->c_string;
    size_t selfCap = this.data->capacity;
    assert(selfCap >= selfLen + 2);
    selfString[selfLen] = code;
    selfString[selfLen + 1] = '\0';
    this.data->length++;
}

IMPLEMENT_SELF_METHOD(void, writeCString, const char* cstring) {
    if (cstring == NULL) {
        return;
    }
    size_t cstringLen = strlen(cstring);
    size_t selfLen = this.data->length;
    StringBuffer_ensure(this, selfLen + cstringLen);
    char* selfString = this.data->c_string;
    size_t capacity = this.data->capacity;
    strncpy(selfString + selfLen, cstring, cstringLen);
    this.data->length += cstringLen;
}


IMPLEMENT_SELF_METHOD(void, writeBuffer, const char* buffer, size_t size) {
    if (buffer == NULL) {
        return;
    }
    if (size == 0) {
        return;
    }
    size_t selfLen = this.data->length;
    StringBuffer_ensure(this, selfLen + size);
    char* selfString = this.data->c_string;
    size_t capacity = this.data->capacity;
    memcpy(selfString + selfLen, buffer, size);
    this.data->length += size;
    selfString[this.data->length] = '\0';
}

IMPLEMENT_SELF_METHOD(void, writeLn, Object obj) {
    StringBuffer_write(this, obj);
    StringBuffer_writeCharCode(this, '\n');
}
//
//IMPLEMENT_SELF_METHOD(void, writeLnCrlf, Object obj) {
//    StringBuffer_write(this, obj);
//    StringBuffer_writeCharCode(this, '\r');
//    StringBuffer_writeCharCode(this, '\n');
//}


IMPLEMENT_SELF_METHOD(void, ensure, size_t min_capacity) {
    // null terminator
    min_capacity += 1;
    size_t capacity = this.data->capacity;
    if (capacity >= min_capacity) {
        return;
    }
    // Assume the initial capacity to be 16
    size_t new_capacity = capacity == 0 ? 16 : capacity;
    while (new_capacity < min_capacity) {
        new_capacity <<= 1;
    }
    char* old_str = this.data->c_string;
    char* new_str = realloc(old_str, new_capacity * sizeof(char));
    if (new_str == NULL) {
        // TODO: throw
    }
    this.data->c_string = new_str;
    this.data->capacity = new_capacity;
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    StringBuffer self = DOWNCAST(this, StringBuffer);
    return String$make_own_len(self.data->c_string, self.data->length);
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    StringBuffer self = DOWNCAST(this, StringBuffer);
    size_t capacity = self.data->capacity;
    size_t length = self.data->length;
    char* c_string = self.data->c_string;
    if (capacity == 0) {
        Super()->delete(this);
        return;
    }
    // Delete the string
    free(c_string);
    Super()->delete(this);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // StringBuffer
    vtable->ensure = _StringBuffer_ensure_impl;
    vtable->clear = _StringBuffer_clear_impl;
    vtable->releaseToString = _StringBuffer_releaseToString_impl;
    vtable->write = _StringBuffer_write_impl;
    vtable->writeAll = _StringBuffer_writeAll_impl;
    vtable->writeCharCode = _StringBuffer_writeCharCode_impl;
    vtable->writeCString = _StringBuffer_writeCString_impl;
    vtable->writeBuffer = _StringBuffer_writeBuffer_impl;
    vtable->writeLn = _StringBuffer_writeLn_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _StringBuffer_delete_impl;
    object_vtable->toString = _StringBuffer_toString_impl;
}

IMPLEMENT_CONSTRUCTOR(new) {

}

IMPLEMENT_SELF_GETTER(size_t, length) {
    return this.data->length;
}

#undef Super
#undef Self
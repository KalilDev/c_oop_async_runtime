#include "Object.h"
#include "ByteBuffer.h"
#include "UInt8List.h"
#include "String.h"
#include "List.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "primitive/Bool.h"
#include "primitive/Number.h"
#include "primitive/Integer.h"
#include "foreach.h"
#include <strings.h>
#include <assert.h>

#define Super() Object_vtable()
#define Self ByteBuffer
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_BYTE_BUFFER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)


IMPLEMENT_SELF_METHOD(void, clear) {
    this.data->length = 0;
}


IMPLEMENT_SELF_METHOD(UInt8List, toBytes) {
    unsigned char *buffer = this.data->buffer;
    size_t len = this.data->length;
    unsigned char *outBuffer = malloc(len);
    memcpy(outBuffer, buffer, len * sizeof(unsigned char));
    return UInt8List$make_fromBuffer(buffer, len);
}

IMPLEMENT_SELF_METHOD(UInt8List, releaseToBytes) {
    unsigned char *buffer = this.data->buffer;
    size_t len = this.data->length;
    this.data->buffer = NULL;
    this.data->capacity = 0;
    this.data->length = 0;
    return UInt8List$make_fromBuffer(buffer, len);
}

IMPLEMENT_SELF_METHOD(void, write, unsigned char byte) {
    size_t selfLen = this.data->length;
    ByteBuffer_ensure(this, selfLen + 1);
    unsigned char* buffer = this.data->buffer;
    buffer[selfLen] = byte;
    this.data->length++;
}

IMPLEMENT_SELF_METHOD(void, writeAll, List list) {
    size_t i = 0;
    unsigned char *buffer = this.data->buffer;
    size_t length = this.data->length;
    size_t list_length = List_length(list);
    ByteBuffer_ensure(this, length + list_length);

    // fast path!!!
    if (list.vtable == UInt8List_vtable()) {
        UInt8List l = DOWNCAST(list, UInt8List);
        unsigned char *list_buffer = UInt8List_list(l);
        memcpy(buffer + length, list_buffer, list_length);
        return;
    }

    foreach(Object, b, List_as_Iterable(list), {
        // is not number
        if (!Object_isObjectTypeAssignable(b, "Number")) {
            // todo: throw
        }
        Number b_number = DOWNCAST(b, Number);
        int num = Integer_unbox_i(Number_toInteger(b_number));
        buffer[length] = (unsigned char)num;
        length++;
    })
}

IMPLEMENT_SELF_METHOD(void, writeCString, const char* cstring) {
    if (cstring == NULL) {
        return;
    }
    ByteBuffer_writeBuffer(this, (const unsigned char*)cstring, strlen(cstring));
}

IMPLEMENT_SELF_METHOD(void, writeString, String string) {
    if (Object_isNull(string.asObject)) {
        return;
    }
    const char *cstring = String_cStringView(string);
    size_t strLen = String_length(string);
    ByteBuffer_writeBuffer(this, (const unsigned char*)cstring, strLen);
}


IMPLEMENT_SELF_METHOD(void, writeBuffer, const unsigned char* buffer, size_t size) {
    if (buffer == NULL) {
        return;
    }
    if (size == 0) {
        return;
    }
    size_t selfLen = this.data->length;
    ByteBuffer_ensure(this, selfLen + size);
    unsigned char* selfBuffer = this.data->buffer;
    memcpy(selfBuffer + selfLen, buffer, size);
    this.data->length += size;
}

IMPLEMENT_SELF_METHOD(void, ensure, size_t min_capacity) {
    size_t capacity = this.data->capacity;
    if (capacity >= min_capacity) {
        return;
    }
    // Assume the initial capacity to be 16
    size_t new_capacity = capacity == 0 ? 16 : capacity;
    while (new_capacity < min_capacity) {
        new_capacity <<= 1;
    }
    unsigned char* old_buffer = this.data->buffer;
    unsigned char* new_buffer = realloc(old_buffer, new_capacity * sizeof(unsigned char));
    if (new_buffer == NULL) {
        // TODO: throw
    }
    this.data->buffer = new_buffer;
    this.data->capacity = new_capacity;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    ByteBuffer self = DOWNCAST(this, ByteBuffer);
    size_t capacity = self.data->capacity;
    size_t length = self.data->length;
    unsigned char* buffer = self.data->buffer;
    if (capacity == 0) {
        Super()->delete(this);
        return;
    }
    // Delete the buffer
    free(buffer);
    Super()->delete(this);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // ByteBuffer
    vtable->ensure = _ByteBuffer_ensure_impl;
    vtable->clear = _ByteBuffer_clear_impl;
    vtable->releaseToBytes = _ByteBuffer_releaseToBytes_impl;
    vtable->toBytes = _ByteBuffer_toBytes_impl;
    vtable->write = _ByteBuffer_write_impl;
    vtable->writeAll = _ByteBuffer_writeAll_impl;
    vtable->writeCString = _ByteBuffer_writeCString_impl;
    vtable->writeBuffer = _ByteBuffer_writeBuffer_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _ByteBuffer_delete_impl;
}

SUPER_CAST_IMPL(ByteBuffer, Object)

IMPLEMENT_CONSTRUCTOR(new) {

}

IMPLEMENT_SELF_GETTER(size_t, length) {
    return this.data->length;
}

#undef Super
#undef Self
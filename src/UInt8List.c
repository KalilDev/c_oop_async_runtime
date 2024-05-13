#include "UInt8List.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include <assert.h>
#include "String.h"
#include "primitive/Number.h"
#include "primitive/Integer.h"

#define Super() TypedList_vtable()
#define Self UInt8List
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_UINT8_LIST_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    TypedList self = DOWNCAST(this, TypedList);
    size_t bufferSize = self.data->bufferSize;
    Object* buffer = self.data->buffer;
    Object_vtable()->delete(this);
    if (bufferSize != 0 || buffer != NULL) {
        free(buffer);
    }
}

IMPLEMENT_OVERRIDE_METHOD(Object, List, at, size_t i) {
    UInt8List self = DOWNCAST(this, UInt8List);
    unsigned char* list = UInt8List_list(self);
    size_t length = UInt8List_length(self);
    if (i >= length) {
        // todo: throw
    }
    unsigned char c = list[i];
    int num = (int)c;
    return Integer$box_i(num).asObject;
}
IMPLEMENT_OVERRIDE_METHOD(void, List, setAt, size_t i, Object e) {
    UInt8List self = DOWNCAST(this, UInt8List);
    unsigned char* list = UInt8List_list(self);
    size_t length = UInt8List_length(self);
    if (i >= length) {
        // todo: throw
    }
    // is not number
    if (!Object_isObjectTypeAssignable(e, "Number")) {
        // todo: throw
    }
    Number n = DOWNCAST(e, Number);
    int num = Integer_unbox_i(Number_toInteger(n));
    list[i] = (unsigned char)num;
}
IMPLEMENT_OVERRIDE_METHOD(size_t, List, length) {
    UInt8List self = DOWNCAST(this, UInt8List);
    return UInt8List_length(self);
}


IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // UInt8List

    // List
    List_vtable_t *list_vtable = (List_vtable_t *)vtable;
    list_vtable->at = _UInt8List_at_impl;
    list_vtable->setAt = _UInt8List_setAt_impl;
    list_vtable->length = _UInt8List_length_impl;
}

OBJECT_CAST_IMPL(Iterable, UInt8List)

IMPLEMENT_CONSTRUCTOR(new, size_t length) {
    TypedList$new(this.asTypedList, calloc(length, sizeof(unsigned char)), length);
}

IMPLEMENT_CONSTRUCTOR(fromBuffer, unsigned char* list, size_t length) {
    TypedList$new(this.asTypedList, list, length * sizeof(unsigned char));
}

IMPLEMENT_SELF_GETTER(size_t, length) {
    return this.data->super.bufferSize / sizeof(unsigned char);
}

IMPLEMENT_SELF_GETTER(unsigned char*, list) {
    return (unsigned char*)this.data->super.buffer;
}

#undef Super
#undef Self
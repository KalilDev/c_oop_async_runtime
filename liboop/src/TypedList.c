#include <assert.h>
#include "liboop.h"

#define Super() List_vtable()
#define Self TypedList
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_TYPED_LIST_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_TYPED_LIST_PARENTS)

IMPLEMENT_SELF_METHOD(void*, release)       {
    void *buf = this.data->buffer;
    this.data->buffer = NULL;
    this.data->bufferSize = 0;
    return buf;
}
IMPLEMENT_SELF_METHOD(UInt8List, releaseToUInt8list)       {
    UInt8List list = TypedList_asUInt8list(this);
    this.data->buffer = NULL;
    this.data->bufferSize = 0;
    return list;
}
IMPLEMENT_SELF_METHOD(UInt8List, asUInt8list) {
    unsigned char *elements = (unsigned char*)this.data->buffer;
    size_t length = this.data->bufferSize / sizeof(unsigned char);
    return UInt8List$make_fromBuffer(elements, length);
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    TypedList self = DOWNCAST(this, TypedList);
    size_t bufferSize = self.data->bufferSize;
    Object* buffer = self.data->buffer;
    Object_vtable()->delete(this);
    if (bufferSize != 0 || buffer != NULL) {
        free(buffer);
    }
}

IMPLEMENT_OVERRIDE_METHOD(Object, List, removeAt, size_t i, THROWS) {
    THROW(Exception$make_newCString("Unsupported operation on TypedList"), null);
}

IMPLEMENT_OVERRIDE_METHOD(void, List, add, Object e, THROWS) {
    THROW(Exception$make_newCString("Unsupported operation on TypedList"))
}

IMPLEMENT_OVERRIDE_METHOD(void, List, setLength, size_t newLength, THROWS) {
    THROW(Exception$make_newCString("Unsupported operation on TypedList"))
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // TypedList
    vtable->asUInt8list = _TypedList_asUInt8list_impl;
    vtable->releaseToUInt8list = _TypedList_releaseToUInt8list_impl;
    vtable->release = _TypedList_release_impl;
    // List
    List_vtable_t *list_vtable = (List_vtable_t *)vtable;
    list_vtable->add = _TypedList_add_impl;
    list_vtable->setLength = _TypedList_setLength_impl;
    list_vtable->removeAt = _TypedList_removeAt_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _TypedList_delete_impl;
}

OBJECT_CAST_IMPL(Iterable, TypedList)

IMPLEMENT_CONSTRUCTOR(new, void* buffer, size_t bufferSize) {
    List$new(this.asList);
    this.data->buffer = buffer;
    this.data->bufferSize = bufferSize;
}

IMPLEMENT_SELF_GETTER(size_t, bufferSize) {
    return this.data->bufferSize;
}

IMPLEMENT_SELF_GETTER(void*, buffer) {
    return this.data->buffer;
}

#undef Super
#undef Self
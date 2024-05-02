#include <string.h>
#include "Object.h"
#include "String.h"
#include "oop.h"

#define Self Object

ENUMERATE_OBJECT_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_METHOD(long, getHashCode) {
    return (long)this.data;
}
IMPLEMENT_SELF_METHOD(bool, equals, Object other) {
    return this.data == other.data;
}
IMPLEMENT_SELF_METHOD(void, delete ) {
    return Object_free_memory(this.data);
}
IMPLEMENT_SELF_METHOD(String, toString ) {
    return DOWNCAST(Object_null, String);
}

IMPLEMENT_STATIC_METHOD(void*, allocate_memory, size_t size) {
    void * mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}

IMPLEMENT_STATIC_METHOD(void, free_memory, void* memory) {
    return free(memory);
}
IMPLEMENT_STATIC_METHOD(void, delete_ptr, Object* pointer) {
    if (pointer == NULL) {
        return;
    }
    Object_delete(*pointer);
}
IMPLEMENT_STATIC_METHOD(void, delete_any_ptr, void* pointer) {
    if (pointer == NULL) {
        return;
    }
    Object_delete_ptr((Object *)pointer);
}
IMPLEMENT_STATIC_METHOD(bool, isNull, Object object) {
    return object.data == NULL;
}

IMPLEMENT_OPERATOR_NEW()

#define ENUMERATE_OBJECT_CONSTRUCTORS(CONSTRUCTOR) \
    /* returns the hash code of the object */       \
    CONSTRUCTOR(new)

const Object_vtable_t _Object_vtable = {
        .getHashCode = _Object_getHashCode_impl,
        .equals = _Object_equals_impl,
        .delete = _Object_delete_impl,
};


const Object_vtable_t* Object_vtable() {
    return &_Object_vtable;
}
Object Object_as_Object(Object this) {
    return this;
}
IMPLEMENT_CONSTRUCTOR(new) {

}

Object Object_new() {
    Object ptr = {
            .vtable = &_Object_vtable,
            .data = Object_allocate(sizeof(Object_data))
    };
    return ptr;
}

const Object Object_null = {0};




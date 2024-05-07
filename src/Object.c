#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "Object.h"
#include "String.h"
#include "oop.h"
#include "primitive/StringRef.h"

#define Self Object

ENUMERATE_OBJECT_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_OPERATOR_NEW()




static void _Null_delete_impl(Object this ) {

}

static bool _Null_equals_impl(Object this , Object other) {
    return Object_isNull(other);
}

static long _Null_getHashCode_impl(Object this ) {
    return 0;
}

static const char* _NullString = "null";
static String _Null_toString_impl(Object this ) {
    return StringRef_as_String(StringRef$wrap(_NullString));
}


static const Object_vtable_t _Null_vtable = {
        .object_vtable_tag = OBJECT_VTABLE_TAG,
#ifdef WITH_RTTI
        .runtime_type_information = {
                .class_name = "Null",
                .interface_table = NULL,
                .inheritance_chain = "Null"
        },
#endif
        .toString = _Null_toString_impl,
        .getHashCode = _Null_getHashCode_impl,
        .equals = _Null_equals_impl,
        .delete = _Null_delete_impl
};

const Object null =  {
        .vtable = &_Null_vtable,
        .data = NULL
};


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
    const char* class_name = "Object";
#ifdef WITH_RTTI
    runtime_type_information_t *object_rtti = Object_runtimeTypeInformation(this);
    class_name = object_rtti->class_name;
#endif
    long hashCode = Object_getHashCode(this);
    char* string = NULL;
    size_t len = strlen(class_name);
// #
    len += 1;

    // \0
    len += 1;
    return DOWNCAST(null, String);
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
    return object.vtable == null.vtable && object.data == null.data;
}

#define ENUMERATE_OBJECT_CONSTRUCTORS(CONSTRUCTOR) \
    /* returns the hash code of the object */       \
    CONSTRUCTOR(new)

static const Object_vtable_t _Object_vtable = {
        .object_vtable_tag = OBJECT_VTABLE_TAG,
#ifdef WITH_RTTI
        .runtime_type_information = {
                .class_name = "Object",
                .inheritance_chain = "Object",
                .interface_table = "",
        },
#endif
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

#ifdef WITH_RTTI
const runtime_type_information_t* Object_runtimeTypeInformation(Object object) {
    return &object.vtable->runtime_type_information;
}
#endif


// The var parameters are the interfaces implemented
void initVtable(Object_vtable_t * selfVtable, Object_vtable_t *superVtable, size_t superVtableSize, const char* class_name, size_t implemented_interface_count, ...) {
    assert(selfVtable != NULL);
    assert(superVtable != NULL);
    assert(selfVtable->object_vtable_tag != OBJECT_VTABLE_TAG);
    assert(superVtable->object_vtable_tag == OBJECT_VTABLE_TAG);
    if (superVtable != NULL) {
        memcpy(selfVtable, superVtable, superVtableSize);
    }
    selfVtable->object_vtable_tag = OBJECT_VTABLE_TAG;
#ifdef WITH_RTTI
    va_list args;
    va_start(args, implemented_interface_count);
    initRtti_va((runtime_type_information_t *)&selfVtable->runtime_type_information, (runtime_type_information_t *)&superVtable->runtime_type_information, class_name, implemented_interface_count, args);
    va_end(args);
#endif
}

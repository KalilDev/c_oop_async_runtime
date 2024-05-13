#pragma once
#include "oop.h"
#include <stdlib.h>
#include <stdbool.h>
#define WITH_RTTI
#include "rtti.h"
#include "any.h"

#define Self Object

START_CLASS

#define ENUMERATE_OBJECT_PARENTS(PARENT) \

FORWARD_DECL_CLASS(String)
#define PARAMS_INVOCATION_Object_equals other
#define ENUMERATE_OBJECT_METHODS(METHOD) \
    /* returns the hash code of the object */       \
    METHOD(long, getHashCode) \
    /* compares two objects */                                     \
    METHOD(bool, equals, Object other) \
    /* frees all the object's resources and then frees it  */                                     \
    METHOD(void, delete)                       \
    /* returns the string representation of an object */                                     \
    METHOD(String, toString)

#define ENUMERATE_OBJECT_STATIC_METHODS(METHOD) \
    METHOD(void*, allocate_memory, size_t size)              \
    METHOD(void, free_memory, void* memory) \
    METHOD(void, delete_ptr, Object* pointer) \
    METHOD(void, delete_any_ptr, void* pointer)                       \
    METHOD(bool, isNull, Object object)

#define ENUMERATE_OBJECT_ATTRIBUTES(ATTRIBUTE)

#define ENUMERATE_OBJECT_STATIC_ATTRIBUTES(ATTRIBUTE) \


#define ENUMERATE_OBJECT_CONSTRUCTORS(CONSTRUCTOR) \
    /* returns the hash code of the object */       \
    CONSTRUCTOR(new)

ENUMERATE_OBJECT_METHODS(DECLARE_SELF_METHOD)

typedef struct {
    size_t object_vtable_tag;
    #ifdef WITH_RTTI
    const runtime_type_information_t runtime_type_information;
    #endif
    ENUMERATE_OBJECT_METHODS(DEFINE_SELF_VIRTUAL_METHOD)
} Object_vtable_t;

typedef struct {
    ENUMERATE_OBJECT_ATTRIBUTES(DEFINE_ATTRIBUTE)
} Object_data;

typedef union Object {
    struct {
        const Object_vtable_t* vtable;
        Object_data* data;
    };
    any asAny;
} Object;

DECLARE_SELF_VTABLE_GETTER()


ENUMERATE_OBJECT_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD)

ENUMERATE_OBJECT_CONSTRUCTORS(DECLARE_SELF_CONSTRUCTOR)

ENUMERATE_OBJECT_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)

#ifdef WITH_RTTI
const runtime_type_information_t* Object_runtimeTypeInformation(Object object);
#endif

extern const Object null;

Object Object$$fromObject(Object this);

// The var parameters are the interfaces implemented
void initVtable(Object_vtable_t * selfVtable, Object_vtable_t *superVtable, size_t superVtableSize, const char* class_name, size_t implemented_interface_count, ...);

END_CLASS
#undef Self
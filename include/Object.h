#pragma once
#include "oop.h"
#include <stdlib.h>
#include <stdbool.h>

#define Self Object

START_CLASS

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
    METHOD(struct String, toString)

#define ENUMERATE_OBJECT_STATIC_METHODS(METHOD) \
    METHOD(void*, allocate_memory, size_t size)              \
    METHOD(void, free_memory, void* memory) \
    METHOD(void, delete_ptr, Object* pointer) \
    METHOD(void, delete_any_ptr, void* pointer)                       \
    METHOD(bool, isNull, Object object)

#define ENUMERATE_OBJECT_ATTRIBUTES(ATTRIBUTE) \


#define ENUMERATE_OBJECT_CONSTRUCTORS(CONSTRUCTOR) \
    /* returns the hash code of the object */       \
    CONSTRUCTOR(new)

ENUMERATE_OBJECT_METHODS(DECLARE_SELF_METHOD)

typedef struct {
    ENUMERATE_OBJECT_METHODS(DEFINE_SELF_VIRTUAL_METHOD)
} Object_vtable_t;

typedef struct {

} Object_data;

// An fat pointer to an object
typedef struct Object {
    const Object_vtable_t* vtable;
    Object_data* data;
} Object;

const Object_vtable_t* Object_vtable();


ENUMERATE_OBJECT_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD)

ENUMERATE_OBJECT_CONSTRUCTORS(DECLARE_SELF_CONSTRUCTOR)

extern const Object Object_null;


END_CLASS
#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterable.h"
#include "Interface.h"

#define Super Object
#define Self String

START_CLASS

FORWARD_DECL_CLASS(String)
FORWARD_DECL_CLASS(Integer)
#define PARAMS_INVOCATION_String_at i
#define PARAMS_INVOCATION_String_concat other
#define ENUMERATE_STRING_METHODS(METHOD) \
    METHOD(Integer, at, size_t i)                           \
    METHOD(String, concat, String other)                       \
    METHOD(size_t, length)                       \
    METHOD(const char*, cStringView)

#define ENUMERATE_STRING_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(const char*, c_string)            \
    ATTRIBUTE(size_t, length)

#define ENUMERATE_STRING_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)                          \
    CONSTRUCTOR(own)                    \
    CONSTRUCTOR(own_len)

DEFINE_SELF_CLASS(
        NO_IMPLEMENTS,
        ENUMERATE_STRING_METHODS,
        ENUMERATE_STRING_ATTRIBUTES,
        ENUMERATE_STRING_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

//DECLARE_OBJECT_CAST(Iterable, String)
//DECLARE_INTERFACE_CAST(String, Iterable)

DECLARE_SUPER_CAST(String, Object)

END_CLASS
#undef Super
#undef Self
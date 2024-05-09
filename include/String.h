#pragma once
#include "oop.h"
#include "any.h"
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

#define PARAMS_INVOCATION_String$new str
#define PARAMS_INVOCATION_String$new_len str, len
#define PARAMS_INVOCATION_String$own ownStr
#define PARAMS_INVOCATION_String$own_len ownStr, len
#define ENUMERATE_STRING_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, const char* str)                          \
    CONSTRUCTOR(new_len, const char* str, size_t len)                          \
    CONSTRUCTOR(own, char* ownStr)                    \
    CONSTRUCTOR(own_len, char* ownStr, size_t len)

#define ENUMERATE_STRING_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Iterable)


DEFINE_SELF_CLASS(
        ENUMERATE_STRING_IMPLEMENTS,
        ENUMERATE_STRING_METHODS,
        ENUMERATE_STRING_ATTRIBUTES,
        ENUMERATE_STRING_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

String String_format(String this, ...);
String String_format_c(const char* str, ...);

DECLARE_OBJECT_CAST(Iterable, String)
DECLARE_INTERFACE_CAST(String, Iterable)

DECLARE_SUPER_CAST(String, Object)

END_CLASS
#undef Super
#undef Self
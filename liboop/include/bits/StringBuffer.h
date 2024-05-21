#pragma once
#include "bits/oop.h"
#include <stdlib.h>
#include "bits/Object.h"
#include "bits/Iterable.h"
#include "bits/Interface.h"
#include "bits/any.h"

#define Super Object
#define Self StringBuffer

#define ENUMERATE_STRING_BUFFER_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

START_CLASS

FORWARD_DECL_CLASS(String)
FORWARD_DECL_CLASS(UInt8List)
FORWARD_DECL_CLASS(List)
#define PARAMS_INVOCATION_StringBuffer_ensure capacity
#define PARAMS_INVOCATION_StringBuffer_write obj
#define PARAMS_INVOCATION_StringBuffer_writeAll list, separator
#define PARAMS_INVOCATION_StringBuffer_writeCharCode code
#define PARAMS_INVOCATION_StringBuffer_writeCString cstring
#define PARAMS_INVOCATION_StringBuffer_writeLn obj
#define PARAMS_INVOCATION_StringBuffer_writeBuffer buffer, size
#define ENUMERATE_STRING_BUFFER_METHODS(METHOD) \
    METHOD(void, ensure, size_t capacity) \
    METHOD(void, clear)                           \
    METHOD(String, releaseToString)                           \
    METHOD(void, write, Object obj)                       \
    METHOD(void, writeAll, List list, String separator)                       \
    METHOD(void, writeCharCode, char code)    \
    METHOD(void, writeCString, const char*cstring)\
    METHOD(void, writeBuffer, const char*buffer, size_t size) \
    METHOD(void, writeLn, Object obj) \
    METHOD(UInt8List, toBytes) \
    METHOD(UInt8List, releaseToBytes)

#define ENUMERATE_STRING_BUFFER_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(char*, c_string)            \
    ATTRIBUTE(size_t, length) \
    ATTRIBUTE(size_t, capacity)

#define PARAMS_INVOCATION_StringBuffer$new str
#define ENUMERATE_STRING_BUFFER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_STRING_BUFFER_GETTERS(GETTER) \
    GETTER(size_t, length)


DEFINE_SELF_CLASS(
        ENUMERATE_STRING_BUFFER_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_STRING_BUFFER_METHODS,
        ENUMERATE_STRING_BUFFER_ATTRIBUTES,
        ENUMERATE_STRING_BUFFER_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_STRING_BUFFER_GETTERS
)

END_CLASS
#undef Super
#undef Self
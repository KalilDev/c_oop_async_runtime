#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterable.h"
#include "Interface.h"
#include "any.h"

#define Super Object
#define Self ByteBuffer

START_CLASS

#define ENUMERATE_BYTE_BUFFER_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_CLASS(List)
FORWARD_DECL_CLASS(UInt8List)
#define PARAMS_INVOCATION_ByteBuffer_ensure capacity
#define PARAMS_INVOCATION_ByteBuffer_write byte
#define PARAMS_INVOCATION_ByteBuffer_writeAll list
#define PARAMS_INVOCATION_ByteBuffer_writeString string
#define PARAMS_INVOCATION_ByteBuffer_writeCString cstring
#define PARAMS_INVOCATION_ByteBuffer_writeBuffer buffer, size
#define ENUMERATE_BYTE_BUFFER_METHODS(METHOD) \
    METHOD(void, ensure, size_t capacity) \
    METHOD(void, clear)                           \
    METHOD(UInt8List, releaseToBytes)           \
    METHOD(UInt8List, toBytes)                           \
    METHOD(void, write, unsigned char byte)                       \
    METHOD(void, writeAll, List list)                       \
    METHOD(void, writeString, String string) \
    METHOD(void, writeCString, const char *cstring)\
    METHOD(void, writeBuffer, const unsigned char* buffer, size_t size) \

#define ENUMERATE_BYTE_BUFFER_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(unsigned char*, buffer)            \
    ATTRIBUTE(size_t, length) \
    ATTRIBUTE(size_t, capacity)

#define PARAMS_INVOCATION_ByteBuffer$new str
#define ENUMERATE_BYTE_BUFFER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_BYTE_BUFFER_GETTERS(GETTER) \
    GETTER(size_t, length)


DEFINE_SELF_CLASS(
        ENUMERATE_BYTE_BUFFER_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_BYTE_BUFFER_METHODS,
        ENUMERATE_BYTE_BUFFER_ATTRIBUTES,
        ENUMERATE_BYTE_BUFFER_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_BYTE_BUFFER_GETTERS
)

DECLARE_SUPER_CAST(ByteBuffer, Object)

END_CLASS
#undef Super
#undef Self
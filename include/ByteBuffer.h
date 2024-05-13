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
FORWARD_DECL_THROWABLE
#define PARAMS_INVOCATION_ByteBuffer_ensure capacity, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ByteBuffer_write byte, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ByteBuffer_writeAll list, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ByteBuffer_writeString string, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ByteBuffer_writeCString cstring, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ByteBuffer_writeBuffer buffer, size, THROWS_PARAM_INVOCATION
#define ENUMERATE_BYTE_BUFFER_METHODS(METHOD) \
    METHOD(void, ensure, size_t capacity, THROWS) \
    METHOD(void, clear)                           \
    METHOD(UInt8List, releaseToBytes)           \
    METHOD(UInt8List, toBytes)                           \
    METHOD(void, write, unsigned char byte, THROWS)                       \
    METHOD(void, writeAll, List list, THROWS)                       \
    METHOD(void, writeString, String string, THROWS) \
    METHOD(void, writeCString, const char *cstring, THROWS)\
    METHOD(void, writeBuffer, const unsigned char* buffer, size_t size, THROWS) \

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

END_CLASS
#undef Super
#undef Self
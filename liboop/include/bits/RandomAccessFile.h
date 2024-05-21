#pragma once
#include "bits/Object.h"
#include "bits/oop.h"
#include "bits/Closeable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "bits/any.h"

#define WITH_RTTI
#include "bits/rtti.h"

#include "bits/String.h"

#define Super Object
#define Self RandomAccessFile


#define ENUMERATE_RANDOM_ACCESS_FILE_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

START_CLASS

FORWARD_DECL_CLASS(List)
FORWARD_DECL_THROWABLE
#define PARAMS_INVOCATION_RandomAccessFile_readInto buffer, start, end, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_read count, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_readByte THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_position THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_flush THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_length THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_readString count, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_setPosition position, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_truncate length, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_writeByte byte, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_writeFrom buffer, start, end, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_RandomAccessFile_writeString string, THROWS_PARAM_INVOCATION
#define ENUMERATE_RANDOM_ACCESS_FILE_METHODS(METHOD) \
    METHOD(void, flush, THROWS)           \
    METHOD(size_t, length, THROWS)           \
    METHOD(void, lock)           \
    METHOD(bool, tryLock)           \
    METHOD(size_t, position, THROWS)     \
    METHOD(Integer, readByte, THROWS)     \
    METHOD(size_t, readInto, List buffer, size_t start, Integer end, THROWS)     \
    METHOD(List, read, size_t count, THROWS)                 \
    METHOD(String, readString, size_t count, THROWS)          \
    METHOD(void, setPosition, size_t position, THROWS) \
    METHOD(void, truncate, size_t length, THROWS)           \
    METHOD(void, unlock)           \
    METHOD(void, writeByte, char byte, THROWS)           \
    METHOD(void, writeFrom, List buffer, size_t start, Integer end, THROWS)         \
    METHOD(void, writeString, String string, THROWS)

#define ENUMERATE_RANDOM_ACCESS_FILE_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(FILE*, unwrap) \
    ATTRIBUTE(String, path)

#define ENUMERATE_RANDOM_ACCESS_FILE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(FILE*, file)                 \
    ATTRIBUTE(String, path)

#define PARAMS_INVOCATION_RandomAccessFile$open path, modes, THROWS_PARAM_INVOCATION
#define ENUMERATE_RANDOM_ACCESS_FILE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(open, String path, const char* modes, THROWS)

#define ENUMERATE_RANDOM_ACCESS_FILE_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Closeable)

DEFINE_SELF_CLASS(
        ENUMERATE_RANDOM_ACCESS_FILE_PARENTS,
        ENUMERATE_RANDOM_ACCESS_FILE_IMPLEMENTS,
        ENUMERATE_RANDOM_ACCESS_FILE_METHODS,
        ENUMERATE_RANDOM_ACCESS_FILE_ATTRIBUTES,
        ENUMERATE_RANDOM_ACCESS_FILE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_RANDOM_ACCESS_FILE_GETTERS
)

DECLARE_INTERFACE_CAST(RandomAccessFile, Closeable)
DECLARE_OBJECT_CAST(Closeable, RandomAccessFile)

END_CLASS

#undef Self
#undef Super
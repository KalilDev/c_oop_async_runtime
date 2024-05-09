#pragma once
#include "Object.h"
#include "oop.h"
#include "Closeable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "any.h"
#define WITH_RTTI
#include "rtti.h"

#include <String.h>

#define Super Object
#define Self RandomAccessFile

START_CLASS

FORWARD_DECL_CLASS(List)

#define PARAMS_INVOCATION_RandomAccessFile_lock start, end
#define PARAMS_INVOCATION_RandomAccessFile_readInto buffer, start, end
#define PARAMS_INVOCATION_RandomAccessFile_read count
#define PARAMS_INVOCATION_RandomAccessFile_readString count
#define PARAMS_INVOCATION_RandomAccessFile_setPosition position
#define PARAMS_INVOCATION_RandomAccessFile_truncate length
#define PARAMS_INVOCATION_RandomAccessFile_unlock start, end
#define PARAMS_INVOCATION_RandomAccessFile_writeByte byte
#define PARAMS_INVOCATION_RandomAccessFile_writeFrom buffer, start, end
#define PARAMS_INVOCATION_RandomAccessFile_writeString string
#define ENUMERATE_RANDOM_ACCESS_FILE_METHODS(METHOD) \
    METHOD(void, flush)           \
    METHOD(size_t, length)           \
    METHOD(void, lock)           \
    METHOD(bool, tryLock)           \
    METHOD(size_t, position)     \
    METHOD(Integer, readByte)     \
    METHOD(size_t, readInto, List buffer, size_t start, Integer end)     \
    METHOD(List, read, size_t count)                 \
    METHOD(String, readString, size_t count)          \
    METHOD(void, setPosition, size_t position) \
    METHOD(void, truncate, size_t length)           \
    METHOD(void, unlock)           \
    METHOD(void, writeByte, char byte)           \
    METHOD(void, writeFrom, List buffer, size_t start, Integer end)         \
    METHOD(void, writeString, String string)

#define ENUMERATE_RANDOM_ACCESS_FILE_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(FILE*, unwrap) \
    ATTRIBUTE(String, path)

#define ENUMERATE_RANDOM_ACCESS_FILE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(FILE*, file)                 \
    ATTRIBUTE(String, path)

#define PARAMS_INVOCATION_RandomAccessFile$open path, modes
#define ENUMERATE_RANDOM_ACCESS_FILE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(open, String path, const char* modes)

#define ENUMERATE_RANDOM_ACCESS_FILE_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Closeable)

DEFINE_SELF_CLASS(
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

DECLARE_SUPER_CAST(RandomAccessFile, Object)

END_CLASS

#undef Self
#undef Super
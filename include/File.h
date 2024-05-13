#pragma once
#include "Object.h"
#include "oop.h"
#include "Closeable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "FileSystemEntity.h"
#define WITH_RTTI
#include "rtti.h"

#include <String.h>

#define Super FileSystemEntity
#define Self File

START_CLASS

#define ENUMERATE_FILE_PARENTS(PARENT) \
    ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS(PARENT)                                     \
    PARENT(FileSystemEntity)

typedef enum FileMode {
    FileMode$append,
    FileMode$read,
    FileMode$write,
    FileMode$writeOnly,
    FileMode$writeOnlyAppend
} FileMode;

FORWARD_DECL_CLASS(RandomAccessFile)
FORWARD_DECL_CLASS(Directory)

FORWARD_DECL_THROWABLE
#define PARAMS_INVOCATION_File_readStringSync THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_copy newPath, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_createSync THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_deleteSync THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_existsSync THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_lengthSync THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_File_openSync mode, THROWS_PARAM_INVOCATION
#define ENUMERATE_FILE_METHODS(METHOD) \
    METHOD(String, readStringSync, THROWS)     \
    METHOD(Directory, parent)          \
    METHOD(void, copy, String newPath, THROWS) \
    METHOD(void, createSync, THROWS)           \
    METHOD(void, deleteSync, THROWS)           \
    METHOD(bool, existsSync, THROWS)           \
    METHOD(size_t, lengthSync, THROWS)         \
    METHOD(RandomAccessFile, openSync, FileMode mode, THROWS)

#define ENUMERATE_FILE_GETTERS(ATTRIBUTE) \

#define ENUMERATE_FILE_ATTRIBUTES(ATTRIBUTE) \

#define PARAMS_INVOCATION_File$new path
#define ENUMERATE_FILE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String path)

DEFINE_SELF_CLASS(
        ENUMERATE_FILE_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_FILE_METHODS,
        ENUMERATE_FILE_ATTRIBUTES,
        ENUMERATE_FILE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_FILE_GETTERS
)

END_CLASS

#undef Self
#undef Super
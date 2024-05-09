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

#define PARAMS_INVOCATION_File_copy newPath
#define PARAMS_INVOCATION_File_openSync mode
#define ENUMERATE_FILE_METHODS(METHOD) \
    METHOD(String, readStringSync)     \
    METHOD(Directory, parent)          \
    METHOD(void, copy, String newPath) \
    METHOD(void, createSync)           \
    METHOD(void, deleteSync)           \
    METHOD(bool, existsSync)           \
    METHOD(size_t, lengthSync)         \
    METHOD(RandomAccessFile, openSync, FileMode mode)

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

DECLARE_SUPER_CAST(File, FileSystemEntity)
DECLARE_UPCAST(File, Object)

END_CLASS

#undef Self
#undef Super
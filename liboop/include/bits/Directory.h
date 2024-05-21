#pragma once
#include "bits/Object.h"
#include "bits/oop.h"
#include "bits/Closeable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "bits/FileSystemEntity.h"
#define WITH_RTTI
#include "bits/rtti.h"

#include "bits/String.h"

#define Super FileSystemEntity
#define Self Directory

START_CLASS

FORWARD_DECL_CLASS(List)

#define ENUMERATE_DIRECTORY_PARENTS(PARENT) \
    ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS(PARENT)                                     \
    PARENT(FileSystemEntity)

#define PARAMS_INVOCATION_Directory_openSync mode
#define PARAMS_INVOCATION_Directory_copy newPath
#define PARAMS_INVOCATION_Directory_list THROWS_PARAM_INVOCATION
#define ENUMERATE_DIRECTORY_METHODS(METHOD) \
    METHOD(List, list, THROWS)     \
    METHOD(Directory, parent)          \
    METHOD(void, copy, String newPath) \
    METHOD(void, createSync)           \
    METHOD(void, deleteSync)           \
    METHOD(bool, existsSync)

#define ENUMERATE_DIRECTORY_GETTERS(ATTRIBUTE) \

#define ENUMERATE_DIRECTORY_ATTRIBUTES(ATTRIBUTE) \

#define PARAMS_INVOCATION_Directory$new path
#define ENUMERATE_DIRECTORY_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String path)

#define ENUMERATE_DIRECTORY_STATIC_METHODS(ATTRIBUTE) \
    ATTRIBUTE(Directory, current)                     \
    ATTRIBUTE(bool, isDirectorySync, String path)

DEFINE_SELF_CLASS(
        ENUMERATE_DIRECTORY_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_DIRECTORY_METHODS,
        ENUMERATE_DIRECTORY_ATTRIBUTES,
        ENUMERATE_DIRECTORY_CONSTRUCTORS,
        ENUMERATE_DIRECTORY_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_DIRECTORY_GETTERS
)

END_CLASS

#undef Self
#undef Super
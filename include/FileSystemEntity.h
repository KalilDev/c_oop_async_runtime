#pragma once
#include "Object.h"
#include "oop.h"
#include "Closeable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define WITH_RTTI
#include "rtti.h"

#include <String.h>

#define Super Object
#define Self FileSystemEntity

START_CLASS

#define ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)
FORWARD_DECL_THROWABLE
#define PARAMS_INVOCATION_FileSystemEntity_absolute THROWS_PARAM_INVOCATION
#define ENUMERATE_FILE_SYSTEM_ENTITY_METHODS(METHOD) \
    METHOD(FileSystemEntity, absolute, THROWS)

#define ENUMERATE_FILE_SYSTEM_ENTITY_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(String, path)

#define ENUMERATE_FILE_SYSTEM_ENTITY_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(String, path)

#define PARAMS_INVOCATION_FileSystemEntity$new path
#define ENUMERATE_FILE_SYSTEM_ENTITY_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String path)

DEFINE_SELF_ABSTRACT(
        ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_FILE_SYSTEM_ENTITY_METHODS,
        ENUMERATE_FILE_SYSTEM_ENTITY_ATTRIBUTES,
        ENUMERATE_FILE_SYSTEM_ENTITY_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_FILE_SYSTEM_ENTITY_GETTERS
)

END_CLASS

#undef Self
#undef Super
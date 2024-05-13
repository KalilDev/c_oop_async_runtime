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
#define Self Link

START_CLASS

#define ENUMERATE_LINK_PARENTS(PARENT) \
    ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS(PARENT)                                     \
    PARENT(FileSystemEntity)

#define PARAMS_INVOCATION_Link_openSync mode
#define ENUMERATE_LINK_METHODS(METHOD) \

#define ENUMERATE_LINK_GETTERS(ATTRIBUTE) \

#define ENUMERATE_LINK_ATTRIBUTES(ATTRIBUTE) \

#define PARAMS_INVOCATION_Link$new path
#define ENUMERATE_LINK_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String path)

DEFINE_SELF_CLASS(
        ENUMERATE_LINK_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_LINK_METHODS,
        ENUMERATE_LINK_ATTRIBUTES,
        ENUMERATE_LINK_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_LINK_GETTERS
)

END_CLASS

#undef Self
#undef Super
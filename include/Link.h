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

#define PARAMS_INVOCATION_Link_openSync mode
#define ENUMERATE_LINK_METHODS(METHOD) \

#define ENUMERATE_LINK_GETTERS(ATTRIBUTE) \

#define ENUMERATE_LINK_ATTRIBUTES(ATTRIBUTE) \

#define PARAMS_INVOCATION_Link$new path
#define ENUMERATE_LINK_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String path)

DEFINE_SELF_CLASS(
        NO_IMPLEMENTS,
        ENUMERATE_LINK_METHODS,
        ENUMERATE_LINK_ATTRIBUTES,
        ENUMERATE_LINK_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_LINK_GETTERS
)

DECLARE_SUPER_CAST(Link, FileSystemEntity)
DECLARE_UPCAST(Link, Object)

END_CLASS

#undef Self
#undef Super
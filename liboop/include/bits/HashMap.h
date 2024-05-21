#pragma once
#include "Object.h"
#include "oop.h"
#include "Map.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "List.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Map
#define Self HashMap

START_CLASS

#define ENUMERATE_HASH_MAP_PARENTS(PARENT) \
    ENUMERATE_MAP_PARENTS(PARENT)                                     \
    PARENT(Map)

FORWARD_DECL_CLASS(String)

#define ENUMERATE_HASH_MAP_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(size_t, length)                 \
    ATTRIBUTE(List, buckets)

#define ENUMERATE_HASH_MAP_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)


DEFINE_SELF_CLASS(
        ENUMERATE_HASH_MAP_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_HASH_MAP_ATTRIBUTES,
        ENUMERATE_HASH_MAP_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, HashMap)

END_CLASS

#undef Self
#undef Super
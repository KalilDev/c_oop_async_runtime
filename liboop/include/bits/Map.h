#ifndef Map
#define Map Map

#include "Object.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self Map

START_CLASS

#define ENUMERATE_MAP_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_Map_set key, value, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_Map_get key
#define PARAMS_INVOCATION_Map_hasKey key
#define PARAMS_INVOCATION_Map_remove key, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_Map_clear THROWS_PARAM_INVOCATION
#define ENUMERATE_MAP_METHODS(METHOD) \
    METHOD(void, set, Object key, Object value, THROWS) \
    METHOD(Object, get, Object key)   \
    METHOD(bool, hasKey, Object key) \
    METHOD(Object, remove, Object key, THROWS)                                                     \
    METHOD(size_t, length) \
    METHOD(void, clear, THROWS)

#define ENUMERATE_MAP_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_MAP_STATIC_METHODS(METHOD) \
    METHOD(Map, new)

DEFINE_SELF_ABSTRACT(
    ENUMERATE_MAP_PARENTS,
    NO_IMPLEMENTS,
    ENUMERATE_MAP_METHODS,
    NO_ATTRIBUTES,
    ENUMERATE_MAP_CONSTRUCTORS,
    ENUMERATE_MAP_STATIC_METHODS,
    NO_STATIC_ATTRIBUTES,
    NO_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
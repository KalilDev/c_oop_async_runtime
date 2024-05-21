#pragma once
#include "Object.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "List.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self HashMapBucket

START_CLASS

#define ENUMERATE_HASH_MAP_BUCKET_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

#define PARAMS_INVOCATION_HashMapBucket_set keyHash, key, value, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_HashMapBucket_get keyHash, key
#define PARAMS_INVOCATION_HashMapBucket_hasKey keyHash, key
#define PARAMS_INVOCATION_HashMapBucket_remove keyHash, key, removed
#define ENUMERATE_HASH_MAP_BUCKET_METHODS(METHOD) \
    METHOD(bool, set, long keyHash, Object key, Object value, THROWS) \
    METHOD(Object, get, long keyHash, Object key)   \
    METHOD(bool, hasKey, long keyHash, Object key)              \
    METHOD(Object, remove, long keyHash, Object key, bool* removed) \
    METHOD(void, release) \
    METHOD(void, clear)

#define ENUMERATE_HASH_MAP_BUCKET_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(bool, filled)                             \
    ATTRIBUTE(long, keyHash)                                                    \
    ATTRIBUTE(Object, key)                                                                                 \
    ATTRIBUTE(Object, value)                                                    \
    ATTRIBUTE(Object, next)

#define ENUMERATE_HASH_MAP_BUCKET_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(empty)


DEFINE_SELF_CLASS(
        ENUMERATE_HASH_MAP_BUCKET_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_HASH_MAP_BUCKET_METHODS,
        ENUMERATE_HASH_MAP_BUCKET_ATTRIBUTES,
        ENUMERATE_HASH_MAP_BUCKET_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super
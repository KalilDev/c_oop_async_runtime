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

#define Super List
#define Self GrowableList

START_CLASS

#define ENUMERATE_GROWABLE_LIST_PARENTS(PARENT) \
    ENUMERATE_LIST_PARENTS(PARENT)                                     \
    PARENT(List)

FORWARD_DECL_CLASS(String)

#define PARAMS_INVOCATION_GrowableList_ensure capacity, THROWS_PARAM_INVOCATION
#define ENUMERATE_GROWABLE_LIST_METHODS(METHOD) \
    METHOD(void, ensure, size_t capacity, THROWS)

#define ENUMERATE_GROWABLE_LIST_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(size_t, length)                 \
    ATTRIBUTE(size_t, capacity)                     \
    ATTRIBUTE(Object*, elements)

#define ENUMERATE_GROWABLE_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)


#define ENUMERATE_GROWABLE_LIST_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(size_t, capacity)

DEFINE_SELF_CLASS(
        ENUMERATE_GROWABLE_LIST_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_GROWABLE_LIST_METHODS,
        ENUMERATE_GROWABLE_LIST_ATTRIBUTES,
        ENUMERATE_GROWABLE_LIST_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_GROWABLE_LIST_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, GrowableList)

END_CLASS

#undef Self
#undef Super
#pragma once
#include "Object.h"
#include "oop.h"
#include <stdlib.h>
#include <stdbool.h>
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self Iterator

START_CLASS

#define ENUMERATE_ITERATOR_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

#define ENUMERATE_ITERATOR_METHODS(METHOD) \
    /* Moves an single step in the iteration */       \
    METHOD(bool, moveNext)               \
    /* Retrieves the current object */                                     \
    METHOD(Object, current)

#define ENUMERATE_ITERATOR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

DEFINE_SELF_ABSTRACT(
        ENUMERATE_ITERATOR_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_ITERATOR_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_ITERATOR_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super
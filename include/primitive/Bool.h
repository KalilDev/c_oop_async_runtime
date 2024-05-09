#pragma once
#include "oop.h"
#include <stdlib.h>

#define Super Object
#define Self Bool

START_CLASS

#define ENUMERATE_BOOL_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

#define ENUMERATE_BOOL_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(box, bool b)

#define ENUMERATE_BOOL_GETTERS(GETTER) \
    GETTER(bool, unbox)


#define ENUMERATE_BOOL_METHODS(METHOD) \
    METHOD(String, toStringFixed)

DEFINE_SELF_PRIMITIVE_CLASS(
        ENUMERATE_BOOL_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_BOOL_METHODS,
        bool,
        ENUMERATE_BOOL_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_BOOL_GETTERS
)

extern const Bool True;
extern const Bool False;

DECLARE_SUPER_CAST(Bool, Object)

END_CLASS
#undef Super
#undef Self


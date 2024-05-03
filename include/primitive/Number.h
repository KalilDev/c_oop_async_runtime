#pragma once
#include "Object.h"
#include "oop.h"
#include "Comparable.h"
#include <stdlib.h>
#include <stdbool.h>
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self Number

START_CLASS

FORWARD_DECL_CLASS(Double)
FORWARD_DECL_CLASS(Integer)

#define ENUMERATE_NUMBER_METHODS(METHOD) \
    /* Gets the absolute value for this number */       \
    METHOD(Number, abs)               \
    /* Clamps an number */                                     \
    METHOD(Number, clamp, Number lower, Number upper) \
    METHOD(Double, toDouble) \
    METHOD(Integer, toInteger)

// no constructors because it is an primitive
#define ENUMERATE_NUMBER_CONSTRUCTORS(CONSTRUCTOR)

#define ENUMERATE_NUMBER_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Comparable)


DEFINE_SELF_ABSTRACT(
        ENUMERATE_NUMBER_IMPLEMENTS,
        ENUMERATE_NUMBER_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_NUMBER_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(Number, Object)

END_CLASS

#undef Self
#undef Super
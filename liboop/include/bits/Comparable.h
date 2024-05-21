#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"

#define Self Comparable

START_CLASS

#define PARAMS_INVOCATION_Comparable_compareTo other
#define ENUMERATE_COMPARABLE_METHODS(METHOD) \
    METHOD(int, compareTo, Object other)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_COMPARABLE_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

END_CLASS
#undef Super
#undef Self

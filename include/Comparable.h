#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"

#define Self Comparable

START_CLASS

#define ENUMERATE_COMPARABLE_METHODS(METHOD) \
    METHOD(void, compareTo, Object other)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_COMPARABLE_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

DECLARE_SUPER_CAST(Comparable, Interface)
DECLARE_OBJECT_CAST(Comparable, Object)

END_CLASS
#undef Super
#undef Self

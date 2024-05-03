#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"

#define Self Closeable

START_CLASS

#define ENUMERATE_CLOSEABLE_METHODS(METHOD) \
    METHOD(void, close)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_CLOSEABLE_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

DECLARE_SUPER_CAST(Closeable, Interface)
DECLARE_OBJECT_CAST(Closeable, Object)

END_CLASS
#undef Super
#undef Self

#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"

#define Self Sink

START_CLASS

#define PARAMS_INVOCATION_Sink_add value
#define ENUMERATE_SINK_METHODS(METHOD) \
    METHOD(void, add, Object value)      \
    METHOD(void, close)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_SINK_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

END_CLASS
#undef Super
#undef Self

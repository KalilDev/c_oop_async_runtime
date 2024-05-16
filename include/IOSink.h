#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"
#include "Sink.h"

#define Self IOSink

START_CLASS

#define PARAMS_INVOCATION_IOSink_add value
#define ENUMERATE_IO_SINK_METHODS(METHOD) \
    METHOD(void, flush)

DEFINE_SELF_EXTENDED_INTERFACE(
        Sink,
        Object,
        ENUMERATE_IO_SINK_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

END_CLASS
#undef Super
#undef Self

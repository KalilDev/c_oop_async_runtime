#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"
#include "Stream.h"
#include "Sink.h"

#define Self IOSink

START_CLASS
FORWARD_DECL_CLASS(Future)
FORWARD_DECL_CLASS(Stream)
#define PARAMS_INVOCATION_IOSink_add value
#define PARAMS_INVOCATION_IOSink_addStream stream
#define ENUMERATE_IO_SINK_METHODS(METHOD) \
    METHOD(Future, flush)                 \
    METHOD(Future, addStream, Stream stream)

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

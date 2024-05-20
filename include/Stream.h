#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Object.h"

#define Self Stream

START_CLASS

FORWARD_DECL_CLASS(StreamSubscription)
FORWARD_DECL_CLASS(Bool)
FORWARD_DECL_CLASS(Function)
FORWARD_DECL_CLASS(Future)
#define PARAMS_INVOCATION_Stream_listen onData, onError, onDone, cancelOnError
#define ENUMERATE_STREAM_METHODS(METHOD) \
    METHOD(StreamSubscription, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) \
    METHOD(Future, toList)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_STREAM_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

END_CLASS
#undef Super
#undef Self

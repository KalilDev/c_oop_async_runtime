#ifndef StreamController
#define StreamController StreamController

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "Sink.h"
#include "Function.h"
#include "StreamSubscription.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self StreamController

START_CLASS

#define ENUMERATE_STREAM_CONTROLLER_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_StreamController$new onListen, onCancelSubscription
#define ENUMERATE_STREAM_CONTROLLER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Function onListen, Function onCancelSubscription)

#define ENUMERATE_STREAM_CONTROLLER_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)                                     \
    IMPLEMENTS(Sink)

#define ENUMERATE_STREAM_CONTROLLER_METHODS(METHOD) \
    METHOD(void, addError, Throwable error)

#define ENUMERATE_STREAM_CONTROLLER_STATIC_METHODS(METHOD) \
    METHOD(void, add, StreamController this, Object value)  \
    METHOD(void, close, StreamController this)

#define ENUMERATE_STREAM_CONTROLLER_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Function, onListen)                      \
    ATTRIBUTE(Function, onCancelSubscription)   \
    ATTRIBUTE(StreamSubscription, subs)                   \
    ATTRIBUTE(List, bufferedEvents)


DEFINE_SELF_CLASS(
        ENUMERATE_STREAM_CONTROLLER_PARENTS,
        ENUMERATE_STREAM_CONTROLLER_IMPLEMENTS,
        ENUMERATE_STREAM_CONTROLLER_METHODS,
        ENUMERATE_STREAM_CONTROLLER_ATTRIBUTES,
        ENUMERATE_STREAM_CONTROLLER_CONSTRUCTORS,
        ENUMERATE_STREAM_CONTROLLER_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_OBJECT_CAST(Sink, StreamController)
DECLARE_INTERFACE_CAST(StreamController, Sink)

DECLARE_OBJECT_CAST(Stream, StreamController)
DECLARE_INTERFACE_CAST(StreamController, Stream)

END_CLASS

#undef Self
#undef Super

#endif
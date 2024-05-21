#pragma once
#include "bits/Object.h"
#include "bits/Function.h"
#include "bits/EventLoop.h"
#include "bits/oop.h"
#include <stdlib.h>
#include <stdbool.h>
#define WITH_RTTI
#include "bits/rtti.h"

#define Super Object
#define Self StreamSubscription

START_CLASS

#define ENUMERATE_STREAM_SUBSCRIPTION_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_THROWABLE
FORWARD_DECL_CLASS(Future)
FORWARD_DECL_CLASS(Bool)
#define PARAMS_INVOCATION_StreamSubscription_handleData data
#define PARAMS_INVOCATION_StreamSubscription_handleError error
#define PARAMS_INVOCATION_StreamSubscription_handleDone
#define ENUMERATE_STREAM_SUBSCRIPTION_METHODS(METHOD) \
    METHOD(Future, handleData, Object data)             \
    METHOD(Future, handleError, Throwable error)        \
    METHOD(Future, handleDone)                  \
    METHOD(Future, cancel)

#define ENUMERATE_STREAM_SUBSCRIPTION_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Function, onData)                                         \
    ATTRIBUTE(Function, onError)                                        \
    ATTRIBUTE(Function, onDone)                             \
    ATTRIBUTE(Function, onCancel)                           \
    ATTRIBUTE(EventLoop, attachedLoop)    \
    ATTRIBUTE(bool, cancelOnError)    \
    ATTRIBUTE(bool, cancelScheduled)

#define PARAMS_INVOCATION_StreamSubscription$new onData, onError, onDone, onCancel, cancelOnError
#define ENUMERATE_STREAM_SUBSCRIPTION_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Function onData, Function onError, Function onDone, Function onCancel, Bool cancelOnError)

#define ENUMERATE_STREAM_SUBSCRIPTION_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(bool, cancelOnError)

DEFINE_SELF_CLASS(
        ENUMERATE_STREAM_SUBSCRIPTION_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_STREAM_SUBSCRIPTION_METHODS,
        ENUMERATE_STREAM_SUBSCRIPTION_ATTRIBUTES,
        ENUMERATE_STREAM_SUBSCRIPTION_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_STREAM_SUBSCRIPTION_GETTERS
)

END_CLASS

#undef Self
#undef Super
#ifndef Future
#define Future Future

#include "bits/Object.h"
#include "bits/any.h"
#include "bits/oop.h"
#include "bits/Iterable.h"
#include "bits/EventLoop.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bits/List.h"
#include "bits/Function.h"
#include "bits/Throwable.h"
#define WITH_RTTI
#include "bits/rtti.h"

#define Super Object
#define Self Future

#define ENUMERATE_FUTURE_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)

START_CLASS

#define PARAMS_INVOCATION_Future_then then
#define PARAMS_INVOCATION_Future_onThen then
#define PARAMS_INVOCATION_Future_catch catch
#define PARAMS_INVOCATION_Future_onCatch catch
#define PARAMS_INVOCATION_Future_handle then, catch
#define PARAMS_INVOCATION_Future_setCallbacks then, catch
#define ENUMERATE_FUTURE_METHODS(METHOD) \
    METHOD(Future, then, Function then) \
    METHOD(Future, catch, Function catch)\
    METHOD(Future, handle, Function then, Function catch) \
    METHOD(void, setCallbacks, Function then, Function catch)\
    METHOD(void, _onComplete)            \
    METHOD(void, _onCompleteWithError)

#define PARAMS_INVOCATION_Future$value value
#define PARAMS_INVOCATION_Future$exception exception
#define ENUMERATE_FUTURE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(value, Object value)                              \
    CONSTRUCTOR(exception, Throwable exception)    \
    CONSTRUCTOR(_)
FORWARD_DECL_CLASS(EventLoop)

#define PARAMS_INVOCATION_Future_computation function, loop
#define ENUMERATE_FUTURE_STATIC_METHODS(METHOD) \
    METHOD(Future, computation, Function function) \
    METHOD(Future, computationAt, Function function, EventLoop loop)

typedef enum FutureState {
    FutureState$pending,
    FutureState$complete,
    FutureState$completedWithError
} FutureState;

#define ENUMERATE_FUTURE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Object, value)                   \
    ATTRIBUTE(Throwable, exception)            \
    ATTRIBUTE(FutureState, state) \
    ATTRIBUTE(Function, then)                  \
    ATTRIBUTE(Function, catch) \
    ATTRIBUTE(EventLoop, attachedLoop)

DEFINE_SELF_CLASS(
        ENUMERATE_FUTURE_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_FUTURE_METHODS,
        ENUMERATE_FUTURE_ATTRIBUTES,
        ENUMERATE_FUTURE_CONSTRUCTORS,
        ENUMERATE_FUTURE_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
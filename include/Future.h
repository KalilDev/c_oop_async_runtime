#ifndef Future
#define Future Future

#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "List.h"
#include "Function.h"
#include "Throwable.h"
#define WITH_RTTI
#include "rtti.h"

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
#define ENUMERATE_FUTURE_METHODS(METHOD) \
    METHOD(Future, then, Function then) \
    METHOD(void, onThen, Function then) \
    METHOD(Future, catch, Function catch)\
    METHOD(void, onCatch, Function catch)\
    METHOD(void, _onComplete)            \
    METHOD(void, _onCompleteWithError)

#define PARAMS_INVOCATION_Future$value value
#define PARAMS_INVOCATION_Future$exception exception
#define ENUMERATE_FUTURE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(value, Object value)                              \
    CONSTRUCTOR(exception, Throwable exception)    \
    CONSTRUCTOR(_)

#define PARAMS_INVOCATION_Future_computation function
#define ENUMERATE_FUTURE_STATIC_METHODS(METHOD) \
    METHOD(Future, computation, Function function)

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
    ATTRIBUTE(Function, catch)

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
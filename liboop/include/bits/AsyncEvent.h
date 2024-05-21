#ifndef AsyncEvent
#define AsyncEvent AsyncEvent
#include "oop.h"
#include "Object.h"
#include "any.h"


#define Super Object
#define Self AsyncEvent

START_CLASS

#define ENUMERATE_ASYNC_EVENT_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(StreamSubscription)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_AsyncEvent$value value
#define PARAMS_INVOCATION_AsyncEvent$error error
#define ENUMERATE_ASYNC_EVENT_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(value, Object value)                    \
    CONSTRUCTOR(error, Throwable error)                 \
    CONSTRUCTOR(done)

#define PARAMS_INVOCATION_AsyncEvent_addToStreamSubscription subscription
#define ENUMERATE_ASYNC_EVENT_METHODS(METHOD) \
    METHOD(void, addToStreamSubscription, StreamSubscription subscription)

typedef enum AsyncEventType {
    AsyncEventType$value,
    AsyncEventType$error,
    AsyncEventType$done,
} AsyncEventType;

#define ENUMERATE_ASYNC_EVENT_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(AsyncEventType, type)                      \
    ATTRIBUTE(Object, value)

DEFINE_SELF_CLASS(
        ENUMERATE_ASYNC_EVENT_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_ASYNC_EVENT_METHODS,
        ENUMERATE_ASYNC_EVENT_ATTRIBUTES,
        ENUMERATE_ASYNC_EVENT_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#endif //C_OOP_ASYNCEVENT_H

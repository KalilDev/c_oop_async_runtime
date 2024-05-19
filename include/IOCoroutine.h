#ifndef IOCoroutine
#define IOCoroutine IOCoroutine

#include "Object.h"
#include "any.h"
#include "oop.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "Function.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self IOCoroutine

#define ENUMERATE_IO_COROUTINE_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)


START_CLASS

#define PARAMS_INVOCATION_IOCoroutine_ownsFd fd
#define ENUMERATE_IO_COROUTINE_METHODS(METHOD) \
    METHOD(bool, ownsFd, int fd)           \
    METHOD(void, scheduleStep)         \
    METHOD(void, remove)
    
#define PARAMS_INVOCATION_IOCoroutine$new step, fd, usedEvents
#define ENUMERATE_IO_COROUTINE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Function step, int fd, short usedEvents)

#define ENUMERATE_IO_COROUTINE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(int, fd)                       \
    ATTRIBUTE(Function, step)

DEFINE_SELF_CLASS(
        ENUMERATE_IO_COROUTINE_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_IO_COROUTINE_METHODS,
        ENUMERATE_IO_COROUTINE_ATTRIBUTES,
        ENUMERATE_IO_COROUTINE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
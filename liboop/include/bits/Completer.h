#ifndef Completer
#define Completer Completer

#include "bits/Object.h"
#include "bits/any.h"
#include "bits/oop.h"
#include "bits/Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bits/Future.h"
#include "bits/Throwable.h"
#define WITH_RTTI
#include "bits/rtti.h"

#define Super Object
#define Self Completer

#define ENUMERATE_COMPLETER_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)

START_CLASS

#define PARAMS_INVOCATION_Completer_complete value
#define PARAMS_INVOCATION_Completer_completeException exception
#define PARAMS_INVOCATION_Completer_follow future
#define ENUMERATE_COMPLETER_METHODS(METHOD) \
    METHOD(void, complete, Object value)                  \
    METHOD(void, completeException, Throwable exception)  \
    METHOD(void, follow, Future future)

#define ENUMERATE_COMPLETER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_COMPLETER_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Future, future)

#define ENUMERATE_COMPLETER_GETTERS(GETTER) \
    GETTER(Future, future)


DEFINE_SELF_CLASS(
        ENUMERATE_COMPLETER_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_COMPLETER_METHODS,
        ENUMERATE_COMPLETER_ATTRIBUTES,
        ENUMERATE_COMPLETER_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_COMPLETER_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
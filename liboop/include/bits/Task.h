#ifndef Task
#define Task Task

#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "Future.h"
#include "Completer.h"
#include "Function.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self Task

#define ENUMERATE_TASK_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)


START_CLASS

FORWARD_DECL_CLASS(StringRef)
#define PARAMS_INVOCATION_Task_addStackFrame stackFrame
#define PARAMS_INVOCATION_Task_printStackTrace to
#define ENUMERATE_TASK_METHODS(METHOD) \
    METHOD(void, invokeSync)           \
    METHOD(Future, invoke)

#define PARAMS_INVOCATION_Task$new function
#define ENUMERATE_TASK_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Function function)

#define ENUMERATE_TASK_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Completer, completer)          \
    ATTRIBUTE(Function, function)

#define ENUMERATE_TASK_GETTERS(GETTER) \
    GETTER(Future, future)

DEFINE_SELF_CLASS(
        ENUMERATE_TASK_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_TASK_METHODS,
        ENUMERATE_TASK_ATTRIBUTES,
        ENUMERATE_TASK_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_TASK_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
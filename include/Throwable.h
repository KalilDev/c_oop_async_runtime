#pragma once
#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "List.h"
#include "TypedList.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self Throwable

#define ENUMERATE_THROWABLE_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)


START_CLASS

FORWARD_DECL_CLASS(StringRef)
#define PARAMS_INVOCATION_Throwable_addStackFrame stackFrame
#define PARAMS_INVOCATION_Throwable_printStackTrace to
#define ENUMERATE_THROWABLE_METHODS(METHOD) \
    METHOD(void, addStackFrame, StringRef stackFrame) \
    METHOD(void, printStackTrace, FILE* to)

#define ENUMERATE_THROWABLE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_THROWABLE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(List, frames)

DEFINE_SELF_ABSTRACT(
        ENUMERATE_THROWABLE_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_THROWABLE_METHODS,
        ENUMERATE_THROWABLE_ATTRIBUTES,
        ENUMERATE_THROWABLE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(Throwable, Object)

END_CLASS

#undef Self
#undef Super
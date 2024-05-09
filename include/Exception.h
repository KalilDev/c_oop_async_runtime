#pragma once
#include "Object.h"
#include "any.h"
#include "oop.h"
#include "String.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "List.h"
#include "Throwable.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Throwable
#define Self Exception

START_CLASS

FORWARD_DECL_CLASS(StringRef)

#define ENUMERATE_EXCEPTION_PARENTS(PARENT) \
    ENUMERATE_THROWABLE_PARENTS(PARENT)                                     \
    PARENT(Throwable)

#define PARAMS_INVOCATION_Exception$new message
#define ENUMERATE_EXCEPTION_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String message)

#define ENUMERATE_EXCEPTION_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(String, message)
    
DEFINE_SELF_CLASS(
        ENUMERATE_EXCEPTION_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_EXCEPTION_ATTRIBUTES,
        ENUMERATE_EXCEPTION_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(Exception, Throwable)
DECLARE_UPCAST(Exception, Object)

END_CLASS

#undef Self
#undef Super
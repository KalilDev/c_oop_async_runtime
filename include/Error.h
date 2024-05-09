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
#define Self Error

START_CLASS

FORWARD_DECL_CLASS(StringRef)

#define ENUMERATE_ERROR_PARENTS(PARENT) \
    ENUMERATE_THROWABLE_PARENTS(PARENT)                                     \
    PARENT(Throwable)

#define PARAMS_INVOCATION_Error$new message
#define ENUMERATE_ERROR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String message)

#define ENUMERATE_ERROR_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(String, message)
    
DEFINE_SELF_CLASS(
        ENUMERATE_ERROR_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_ERROR_ATTRIBUTES,
        ENUMERATE_ERROR_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(Error, Throwable)
DECLARE_UPCAST(Error, Object)

END_CLASS

#undef Self
#undef Super
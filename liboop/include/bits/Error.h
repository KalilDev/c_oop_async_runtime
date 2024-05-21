#pragma once
#include "bits/Object.h"
#include "bits/any.h"
#include "bits/oop.h"
#include "bits/String.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bits/List.h"
#include "bits/Throwable.h"
#define WITH_RTTI
#include "bits/rtti.h"

#define Super Throwable
#define Self Error

START_CLASS

FORWARD_DECL_CLASS(StringRef)

#define ENUMERATE_ERROR_PARENTS(PARENT) \
    ENUMERATE_THROWABLE_PARENTS(PARENT)                                     \
    PARENT(Throwable)

#define PARAMS_INVOCATION_Error$new message
#define PARAMS_INVOCATION_Error$newCString message
#define ENUMERATE_ERROR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String message) \
    CONSTRUCTOR(newCString, const char* message)

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

END_CLASS

#undef Self
#undef Super
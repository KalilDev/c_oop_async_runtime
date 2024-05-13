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
#include "Error.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Error
#define Self TypeError

START_CLASS

#define ENUMERATE_TYPE_ERROR_PARENTS(PARENT) \
    ENUMERATE_ERROR_PARENTS(PARENT)                                     \
    PARENT(Error)

#define PARAMS_INVOCATION_TypeError$new obj, wantedType
#define ENUMERATE_TYPE_ERROR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Object obj, String wantedType)

DEFINE_SELF_CLASS(
        ENUMERATE_TYPE_ERROR_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_TYPE_ERROR_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super
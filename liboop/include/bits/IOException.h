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
#include "Exception.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Exception
#define Self IOException

START_CLASS

#define ENUMERATE_IO_EXCEPTION_PARENTS(PARENT) \
    ENUMERATE_EXCEPTION_PARENTS(PARENT)                                     \
    PARENT(Exception)

#define PARAMS_INVOCATION_IOException$new num
#define ENUMERATE_IO_EXCEPTION_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, int num)                          \
    CONSTRUCTOR(fromErrno)

DEFINE_SELF_CLASS(
        ENUMERATE_IO_EXCEPTION_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_IO_EXCEPTION_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS

#undef Self
#undef Super
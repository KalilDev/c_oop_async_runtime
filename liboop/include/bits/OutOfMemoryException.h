#pragma once
#include "bits/Exception.h"
#include "bits/oop.h"
#include <stdlib.h>

#define Super Exception
#define Self OutOfMemoryException

#define ENUMERATE_OUT_OF_MEMORY_EXCEPTION_PARENTS(PARENT) \
    ENUMERATE_EXCEPTION_PARENTS(PARENT)                                     \
    PARENT(Exception)

START_CLASS

#define ENUMERATE_OUT_OF_MEMORY_EXCEPTION_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(at, const char *location)

#define ENUMERATE_OUT_OF_MEMORY_EXCEPTION_STATIC_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(const OutOfMemoryException, atUnknownLocation)

DEFINE_SELF_PRIMITIVE_CLASS(
    ENUMERATE_OUT_OF_MEMORY_EXCEPTION_PARENTS,
    NO_IMPLEMENTS,
    NO_METHODS,
    const char*,
    ENUMERATE_OUT_OF_MEMORY_EXCEPTION_CONSTRUCTORS,
    NO_STATIC_METHODS,
    ENUMERATE_OUT_OF_MEMORY_EXCEPTION_STATIC_ATTRIBUTES,
    NO_GETTERS
)

END_CLASS
#undef Super
#undef Self


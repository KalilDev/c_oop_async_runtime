#pragma once
#include "bits/oop.h"
#include "bits/String.h"
#include <stdlib.h>

#define Super String
#define Self StringRef

START_CLASS

#define ENUMERATE_STRING_REF_PARENTS(PARENT) \
    ENUMERATE_STRING_PARENTS(PARENT)                                     \
    PARENT(String)

#define ENUMERATE_STRING_REF_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(wrap, const char* ref)

DEFINE_SELF_PRIMITIVE_CLASS(
        ENUMERATE_STRING_REF_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        const char*,
        ENUMERATE_STRING_REF_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

END_CLASS
#undef Super
#undef Self

#pragma once
#include "oop.h"
#include <stdlib.h>

#define Super String
#define Self StringRef

START_CLASS

#define ENUMERATE_STRING_REF_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(wrap, const char* ref)

DEFINE_SELF_PRIMITIVE_CLASS(
        NO_IMPLEMENTS,
        NO_METHODS,
        const char*,
        ENUMERATE_STRING_REF_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(StringRef, String)
DECLARE_UPCAST(StringRef, Object)

END_CLASS
#undef Super
#undef Self

#pragma once
#include "oop.h"
#include "Iterator.h"
#include "String.h"
#include <stdlib.h>

#define Super Iterator
#define Self StringIterator

START_CLASS

#define ENUMERATE_STRING_ITERATOR_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(String, string)            \
    ATTRIBUTE(size_t, i)                                \
    ATTRIBUTE(bool, started)

#define PARAMS_INVOCATION_StringIterator$new string
#define ENUMERATE_STRING_ITERATOR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, String string)

DEFINE_SELF_CLASS(
        NO_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_STRING_ITERATOR_ATTRIBUTES,
        ENUMERATE_STRING_ITERATOR_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(StringIterator, Iterator)
DECLARE_UPCAST(StringIterator, Object)

END_CLASS
#undef Super
#undef Self
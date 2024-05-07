#pragma once
#include "oop.h"
#include "Iterator.h"
#include <stdlib.h>

#define Super Iterator
#define Self ListIterator

START_CLASS

FORWARD_DECL_CLASS(Iterator)

#define ENUMERATE_LIST_ITERATOR_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(List, list)            \
    ATTRIBUTE(long long, i)

#define PARAMS_INVOCATION_ListIterator$new list
#define ENUMERATE_LIST_ITERATOR_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, List list)

DEFINE_SELF_CLASS(
        NO_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_LIST_ITERATOR_ATTRIBUTES,
        ENUMERATE_LIST_ITERATOR_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_SUPER_CAST(ListIterator, Iterator)
DECLARE_UPCAST(ListIterator, Object)

END_CLASS
#undef Super
#undef Self
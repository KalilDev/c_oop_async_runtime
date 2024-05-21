#pragma once
#include "bits/oop.h"
#include <stdlib.h>
#include "bits/Interface.h"
#include "bits/Object.h"

#define Self Iterable

START_CLASS

FORWARD_DECL_CLASS(Iterator)

#define ENUMERATE_ITERABLE_METHODS(METHOD) \
    METHOD(Iterator, iterator)

DEFINE_SELF_INTERFACE(
        Object,
        ENUMERATE_ITERABLE_METHODS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES
)

END_CLASS
#undef Super
#undef Self

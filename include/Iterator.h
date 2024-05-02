#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"

#define ENUMERATE_ITERATOR_METHODS(METHOD) \
    METHOD(moveNext) \
    METHOD(current)

#define ENUMERATE_INT_ITERABLE_ATTRIBUTES(ATTRIBUTE) \

struct Iterator;

_DECLARE_METHOD(Iterator, moveNext, bool, (struct Iterator))
_DECLARE_METHOD(Iterator, current, Object, (struct Iterator))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Iterator, method)

DECLARE_CLASS(Iterator, Object, {
    ENUMERATE_ITERATOR_METHODS(METHOD)
}, NO_DATA)

#undef METHOD

const Iterator_vtable_t* Iterator_vtable();
DECLARE_SUPER_CAST(Iterator, Object)
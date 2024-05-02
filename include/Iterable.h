#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Interface.h"
#include "Iterator.h"
#include "Object.h"

#define ENUMERATE_ITERABLE_METHODS(METHOD) \
    METHOD(iterator)

#define ENUMERATE_ITERABLE_ATTRIBUTES(ATTRIBUTE) \

struct Iterable;

_DECLARE_METHOD(Iterable, iterator, Iterator, (struct Iterable))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Iterable, method)

DECLARE_INTERFACE(Iterable, Interface, Object, {
    ENUMERATE_ITERABLE_METHODS(METHOD)\
})

#undef METHOD

const Iterable_vtable_t* Iterable_vtable();
DECLARE_SUPER_CAST(Iterable, Interface)
DECLARE_OBJECT_CAST(Iterable, Object)
#pragma once
#include "oop.h"
#include "Interface.h"

struct Closeable;

#define ENUMERATE_CLOSEABLE_METHODS(METHOD) \
    METHOD(close)

_DECLARE_METHOD(Closeable, close, void, (struct Closeable))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Closeable, method)

DECLARE_INTERFACE(Closeable, Interface, Object, {
    ENUMERATE_CLOSEABLE_METHODS(METHOD)
})

#undef METHOD

const Closeable_vtable_t* Iterable_vtable();
DECLARE_SUPER_CAST(Closeable, Interface)
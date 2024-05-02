#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterable.h"
#include "Interface.h"

#define ENUMERATE_LIST_METHODS(METHOD) \
    METHOD(add) \
    METHOD(ensure) \
    METHOD(at)

#define ENUMERATE_LIST_ATTRIBUTES(ATTRIBUTE) \

struct List;

_DECLARE_METHOD(List, add, void, (struct List, Object))
_DECLARE_METHOD(List, ensure, void, (struct List, size_t))
_DECLARE_METHOD(List, at, Object, (struct List, size_t))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(List, method)

DECLARE_CLASS(List, Object, {
    IMPLEMENTS(Iterable)
    ENUMERATE_LIST_METHODS(METHOD)
}, {
    size_t length;
    size_t capacity;
    Object* elements;
})

#undef METHOD

const List_vtable_t* List_vtable();

DECLARE_OBJECT_CAST(Iterable, List)
DECLARE_INTERFACE_CAST(List, Iterable)

DECLARE_SUPER_CAST(List, Object)

List List_new();
size_t List_length(List this);
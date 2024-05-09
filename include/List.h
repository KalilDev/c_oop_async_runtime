#pragma once
#include "Object.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self List

START_CLASS

#define ENUMERATE_LIST_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)

#define PARAMS_INVOCATION_List_join sep
#define PARAMS_INVOCATION_List_add element
#define PARAMS_INVOCATION_List_ensure capacity
#define PARAMS_INVOCATION_List_at i
#define PARAMS_INVOCATION_List_setAt i, obj
#define PARAMS_INVOCATION_List_setLength newLength
#define ENUMERATE_LIST_METHODS(METHOD) \
    METHOD(String, join, String sep)                           \
    METHOD(void, add, Object element) \
    METHOD(Object, at, size_t i) \
    METHOD(void, setAt, size_t i, Object obj)                  \
    METHOD(size_t, length) \
    METHOD(void, setLength, size_t newLength)

#define ENUMERATE_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_LIST_STATIC_METHODS(METHOD) \
    METHOD(List, new)

#define ENUMERATE_LIST_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Iterable)

DEFINE_SELF_ABSTRACT(
        ENUMERATE_LIST_PARENTS,
        ENUMERATE_LIST_IMPLEMENTS,
        ENUMERATE_LIST_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_LIST_CONSTRUCTORS,
        ENUMERATE_LIST_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, List)
DECLARE_INTERFACE_CAST(List, Iterable)

DECLARE_SUPER_CAST(List, Object)

END_CLASS

#undef Self
#undef Super
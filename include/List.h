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

FORWARD_DECL_CLASS(String)

#define PARAMS_INVOCATION_List_add element
#define PARAMS_INVOCATION_List_ensure capacity
#define PARAMS_INVOCATION_List_at i
#define ENUMERATE_LIST_METHODS(METHOD) \
    METHOD(void, add, Object element) \
    METHOD(void, ensure, size_t capacity) \
    METHOD(Object, at, size_t i)

#define ENUMERATE_LIST_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(size_t, length)

#define ENUMERATE_LIST_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(size_t, length)                 \
    ATTRIBUTE(size_t, capacity)                     \
    ATTRIBUTE(Object*, elements)

#define ENUMERATE_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_LIST_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Iterable)

DEFINE_SELF_CLASS(
        ENUMERATE_LIST_IMPLEMENTS,
        ENUMERATE_LIST_METHODS,
        ENUMERATE_LIST_ATTRIBUTES,
        ENUMERATE_LIST_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_LIST_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, List)
DECLARE_INTERFACE_CAST(List, Iterable)

DECLARE_SUPER_CAST(List, Object)

END_CLASS

#undef Self
#undef Super
#ifndef List
#define List List

#include "bits/Object.h"
#include "bits/oop.h"
#include "bits/Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define WITH_RTTI

#include "bits/rtti.h"

#define Super Object
#define Self List

START_CLASS

#define ENUMERATE_LIST_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_List_join sep
#define PARAMS_INVOCATION_List_add element, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_List_at i
#define PARAMS_INVOCATION_List_remove obj, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_List_removeAt i, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_List_setAt i, obj
#define PARAMS_INVOCATION_List_clear THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_List_setLength newLength, THROWS_PARAM_INVOCATION
#define ENUMERATE_LIST_METHODS(METHOD) \
    METHOD(String, join, String sep)                           \
    METHOD(void, add, Object element, THROWS) \
    METHOD(Object, at, size_t i) \
    METHOD(void, setAt, size_t i, Object obj)                  \
    METHOD(Object, remove, Object obj, THROWS)                                                     \
    METHOD(Object, removeAt, size_t i, THROWS)                  \
    METHOD(size_t, length) \
    METHOD(void, clear, THROWS) \
    METHOD(void, setLength, size_t newLength, THROWS)

#define ENUMERATE_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_LIST_STATIC_METHODS(METHOD) \
    METHOD(List, new)

#define ENUMERATE_LIST_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Iterable)

DEFINE_SELF_ABSTRACT(ENUMERATE_LIST_PARENTS, ENUMERATE_LIST_IMPLEMENTS, ENUMERATE_LIST_METHODS, NO_ATTRIBUTES,
                     ENUMERATE_LIST_CONSTRUCTORS, ENUMERATE_LIST_STATIC_METHODS, NO_STATIC_ATTRIBUTES, NO_GETTERS)

DECLARE_OBJECT_CAST(Iterable, List)
DECLARE_INTERFACE_CAST(List, Iterable)

END_CLASS

#undef Self
#undef Super

#endif
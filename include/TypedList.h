#pragma once
#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "List.h"
#define WITH_RTTI
#include "rtti.h"

#define Super List
#define Self TypedList

START_CLASS

FORWARD_DECL_CLASS(UInt8List)
#define ENUMERATE_TYPED_LIST_METHODS(METHOD) \
    METHOD(UInt8List, releaseToUInt8list)       \
    METHOD(UInt8List, asUInt8list)

#define ENUMERATE_TYPED_LIST_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(size_t, bufferSize)                  \
    ATTRIBUTE(void*, buffer)

#define PARAMS_INVOCATION_TypedList$new buffer, bufferSize
#define ENUMERATE_TYPED_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, void* buffer, size_t bufferSize)

#define ENUMERATE_TYPED_LIST_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(void*, buffer)                       \
    ATTRIBUTE(size_t, bufferSize)
    
DEFINE_SELF_ABSTRACT(
        NO_IMPLEMENTS,
        ENUMERATE_TYPED_LIST_METHODS,
        ENUMERATE_TYPED_LIST_ATTRIBUTES,
        ENUMERATE_TYPED_LIST_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_TYPED_LIST_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, TypedList)

DECLARE_SUPER_CAST(TypedList, List)
DECLARE_UPCAST(TypedList, Object)

END_CLASS

#undef Self
#undef Super
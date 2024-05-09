#pragma once
#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "List.h"
#include "TypedList.h"
#define WITH_RTTI
#include "rtti.h"

#define Super TypedList
#define Self UInt8List

#define ENUMERATE_UINT8_LIST_PARENTS(PARENT) \
    ENUMERATE_TYPED_LIST_PARENTS(PARENT)                                     \
    PARENT(TypedList)

START_CLASS

FORWARD_DECL_CLASS(UInt8List)
#define PARAMS_INVOCATION_UInt8List$new length
#define PARAMS_INVOCATION_UInt8List$fromBuffer list, length
#define ENUMERATE_UINT8_LIST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(fromBuffer, unsigned char* list, size_t length) \
    CONSTRUCTOR(new, size_t length)

#define ENUMERATE_UINT8_LIST_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(unsigned char*, list)                       \
    ATTRIBUTE(size_t, length)

DEFINE_SELF_CLASS(
        ENUMERATE_UINT8_LIST_PARENTS,
        NO_IMPLEMENTS,
        NO_METHODS,
        NO_ATTRIBUTES,
        ENUMERATE_UINT8_LIST_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_UINT8_LIST_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, UInt8List)

DECLARE_SUPER_CAST(UInt8List, TypedList)
DECLARE_UPCAST(UInt8List, List)
DECLARE_UPCAST(UInt8List, Object)

END_CLASS

#undef Self
#undef Super
#ifndef TypedList
#define TypedList TypedList

#include "List.h"

#define Super List
#define Self TypedList

#define ENUMERATE_TYPED_LIST_PARENTS(PARENT) \
    ENUMERATE_LIST_PARENTS(PARENT)                                     \
    PARENT(List)

START_CLASS

FORWARD_DECL_CLASS(UInt8List)
#define ENUMERATE_TYPED_LIST_METHODS(METHOD) \
    METHOD(void*, release)       \
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
        ENUMERATE_TYPED_LIST_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_TYPED_LIST_METHODS,
        ENUMERATE_TYPED_LIST_ATTRIBUTES,
        ENUMERATE_TYPED_LIST_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_TYPED_LIST_GETTERS
)
DECLARE_OBJECT_CAST(Iterable, TypedList)

END_CLASS

#undef Self
#undef Super

#endif
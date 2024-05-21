#ifndef ThreadChildren
#define ThreadChildren ThreadChildren

#include "bits/Object.h"
#include "bits/Function.h"
#include "bits/List.h"
#include "bits/Completer.h"
#include "bits/EventLoop.h"
#include "bits/oop.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "bits/rtti.h"

#define Super Object
#define Self ThreadChildren

START_CLASS

#define ENUMERATE_THREAD_CHILDREN_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_CLASS(Thread)
FORWARD_DECL_THROWABLE

#define ENUMERATE_THREAD_CHILDREN_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define PARAMS_INVOCATION_ThreadChildren_spawned thread, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ThreadChildren_stopped thread, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_ThreadChildren_blockUntilChanges THROWS_PARAM_INVOCATION
#define ENUMERATE_THREAD_CHILDREN_METHODS(METHOD) \
    METHOD(bool, empty)                                        \
    METHOD(void, spawned, Thread thread, THROWS)   \
    METHOD(void, stopped, Thread thread, THROWS) \
    METHOD(void, blockUntilChanges, THROWS)


#define ENUMERATE_THREAD_CHILDREN_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(List, threads)                 \
    ATTRIBUTE(cnd_t, threadsChanged)                    \
    ATTRIBUTE(mtx_t, threadsMutex)

DEFINE_SELF_CLASS(
        ENUMERATE_THREAD_CHILDREN_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_THREAD_CHILDREN_METHODS,
        ENUMERATE_THREAD_CHILDREN_ATTRIBUTES,
        ENUMERATE_THREAD_CHILDREN_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_INTERFACE_CAST(Self, Iterable)

END_CLASS

#undef Self
#undef Super

#endif
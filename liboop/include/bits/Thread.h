#ifndef Thread
#define Thread Thread

#include "bits/Object.h"
#include "bits/Function.h"
#include "bits/List.h"
#include "bits/Completer.h"
#include "bits/ThreadChildren.h"
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
#define Self Thread

START_CLASS

#define ENUMERATE_THREAD_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE
#define PARAMS_INVOCATION_Thread_kill urgency
#define PARAMS_INVOCATION_Thread_unhandledAsyncException exception
#define ENUMERATE_THREAD_METHODS(METHOD) \
    METHOD(Future, kill, KillUrgency urgency) \
    METHOD(int, runInCurrentThread)      \
    METHOD(bool, hasChildren)                     \
    METHOD(void, unhandledAsyncException, Throwable exception)

#define PARAMS_INVOCATION_Thread$new main
#define PARAMS_INVOCATION_Thread$main entry
#define ENUMERATE_THREAD_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, Function main) \
    CONSTRUCTOR(main, Function entry)

#define ENUMERATE_THREAD_STATIC_METHODS(METHOD) \
    METHOD(Thread, spawnSync, Function main, THROWS)   \
    METHOD(Thread, current)

#define ENUMERATE_THREAD_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(Future, result)               \
    ATTRIBUTE(EventLoop, loop)

typedef enum KillUrgency {
    KillUrgency$immediate,
    KillUrgency$nextMicrotask
} KillUrgency;

#define ENUMERATE_THREAD_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(thrd_t, thread)                  \
    ATTRIBUTE(Function, main)                  \
    ATTRIBUTE(Completer, completer)                  \
    ATTRIBUTE(EventLoop, loop)                 \
    ATTRIBUTE(ThreadChildren, children) \
    ATTRIBUTE(ThreadChildren, siblings)

DEFINE_SELF_CLASS(
        ENUMERATE_THREAD_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_THREAD_METHODS,
        ENUMERATE_THREAD_ATTRIBUTES,
        ENUMERATE_THREAD_CONSTRUCTORS,
        ENUMERATE_THREAD_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_THREAD_GETTERS
)
extern _Thread_local Thread _Thread_current;

END_CLASS

#undef Self
#undef Super

#endif
#ifndef Thread
#define Thread Thread

#include "Object.h"
#include "Function.h"
#include "List.h"
#include "Completer.h"
#include "ThreadChildren.h"
#include "EventLoop.h"
#include "oop.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>
#include "poll.h"

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self Thread

START_CLASS

#define ENUMERATE_THREAD_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE
FORWARD_DECL_CLASS(IOCoroutine)
#define PARAMS_INVOCATION_Thread_kill urgency
#define PARAMS_INVOCATION_Thread_removeWatchedFd fd
#define PARAMS_INVOCATION_Thread_watchFd fd, events
#define PARAMS_INVOCATION_Thread_addCoroutine coroutine
#define PARAMS_INVOCATION_Thread_removeCoroutine coroutine
#define ENUMERATE_THREAD_METHODS(METHOD) \
    METHOD(Future, kill, KillUrgency urgency) \
    METHOD(int, runInCurrentThread)      \
    METHOD(void, removeWatchedFd, int fd)\
    METHOD(void, watchFd, int fd, short events) \
    METHOD(void, addCoroutine, IOCoroutine coroutine) \
    METHOD(void, removeCoroutine, IOCoroutine coroutine)

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
    ATTRIBUTE(List, ioCoroutines) \
    ATTRIBUTE(ThreadChildren, siblings)        \
    ATTRIBUTE(struct pollfd *, watched_fds)    \
    ATTRIBUTE(nfds_t, watched_fds_length)      \
    ATTRIBUTE(nfds_t, watched_fds_capacity)

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
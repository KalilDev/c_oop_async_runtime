#ifndef EventLoop
#define EventLoop EventLoop

#include "Object.h"
#include "any.h"
#include "oop.h"
#include "Iterable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>
#include "List.h"
#include "poll.h"
#define WITH_RTTI
#include "rtti.h"

#define Super Object
#define Self EventLoop

#define ENUMERATE_EVENT_LOOP_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)        \
    PARENT(Object)


START_CLASS
FORWARD_DECL_CLASS(Future)
FORWARD_DECL_CLASS(Function)
FORWARD_DECL_CLASS(IOCoroutine)
FORWARD_DECL_CLASS(Task)
#define PARAMS_INVOCATION_EventLoop_pushTask task
#define PARAMS_INVOCATION_EventLoop_invokeTask task
#define PARAMS_INVOCATION_EventLoop_blockUntilNextTask THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_EventLoop_runUntilCompletion onUnhandledAsyncException, THROWS_PARAM_INVOCATION
#define PARAMS_INVOCATION_EventLoop_removeWatchedFd fd
#define PARAMS_INVOCATION_EventLoop_watchFd fd, events
#define PARAMS_INVOCATION_EventLoop_addCoroutine coroutine
#define PARAMS_INVOCATION_EventLoop_removeCoroutine coroutine
#define ENUMERATE_EVENT_LOOP_METHODS(METHOD) \
    METHOD(Task, popTask) \
    METHOD(void, pushTask, Task task)        \
    METHOD(Future, invokeTask, Task task)                                         \
    METHOD(void, drain)                      \
    METHOD(bool, empty) \
    METHOD(void, ackNotification) \
    METHOD(void, notifyWakeup)               \
    METHOD(void, runUntilCompletion, Function onUnhandledAsyncException, THROWS) \
    METHOD(void, removeWatchedFd, int fd)\
    METHOD(void, watchFd, int fd, short events) \
    METHOD(void, addCoroutine, IOCoroutine coroutine) \
    METHOD(void, removeCoroutine, IOCoroutine coroutine)

#define ENUMERATE_EVENT_LOOP_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_EVENT_LOOP_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(List, enqueuedTasks)                 \
    ATTRIBUTE(mtx_t, queueMutex)                   \
    ATTRIBUTE(mtx_t, ioCoroutinesMutex)            \
    ATTRIBUTE(mtx_t, watchedFdsMutex)                                                \
    ATTRIBUTE(bool, wasStarted)                                                                         \
    ATTRIBUTE(int, wakeupListenerFd)                          \
    ATTRIBUTE(int, wakeupFd) \
    ATTRIBUTE(List, ioCoroutines) \
    ATTRIBUTE(struct pollfd *, watched_fds)    \
    ATTRIBUTE(nfds_t, watched_fds_length)      \
    ATTRIBUTE(nfds_t, watched_fds_capacity)

#define ENUMERATE_EVENT_LOOP_STATIC_METHODS(METHOD) \
    METHOD(EventLoop, current)                                                    \

#define ENUMERATE_EVENT_LOOP_GETTERS(GETTER) \
    GETTER(int, fd)                          \
    GETTER(bool, hasCoroutines)

DEFINE_SELF_CLASS(
        ENUMERATE_EVENT_LOOP_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_EVENT_LOOP_METHODS,
        ENUMERATE_EVENT_LOOP_ATTRIBUTES,
        ENUMERATE_EVENT_LOOP_CONSTRUCTORS,
        ENUMERATE_EVENT_LOOP_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_EVENT_LOOP_GETTERS
)

END_CLASS

#undef Self
#undef Super

#endif
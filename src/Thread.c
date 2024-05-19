#include "Object.h"
#include "primitive/Integer.h"
#include "Thread.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "StringBuffer.h"
#include <assert.h>
#include "primitive/StringRef.h"
#include "foreach.h"
#include "GrowableList.h"
#include "Throwable.h"
#include "Future.h"
#include "Function.h"
#include "StreamSubscription.h"
#include "Completer.h"
#include "Error.h"
#include "IOCoroutine.h"
#include "poll.h"
#include <string.h>
#include "IOException.h"
#include <threads.h>

#define Super() Object_vtable()
#define Self Thread
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THREAD_PARENTS)
ENUMERATE_THREAD_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    List self = DOWNCAST(this, List);
    if (List_length(self) == 0) {
        Super()->delete(this);
        return;
    }
    // If there are objects, delete them
    foreach (Object, e, List_as_Iterable(self), {
        Object_delete(e);
    })

    // Delete the array
    Super()->delete(this);
}

IMPLEMENT_SELF_METHOD(Future, kill, KillUrgency urgency) {
    foreach(Thread, child, ThreadChildren_as_Iterable(this.data->children), {
        Thread_kill(child, urgency);
    })
    // TODO: actually kill
    return Thread_result(this);
}

int threadMain(void* data) {
    Thread this = {
        .vtable = Thread_vtable(),
        .data = (Thread_data*)data
    };
    return Thread_runInCurrentThread(this);
}

IMPLEMENT_SELF_METHOD(void, removeWatchedFd, int fd) {
    struct pollfd * wfds = this.data->watched_fds;
    nfds_t wFdsLen = this.data->watched_fds_length;
    if (wfds == NULL) {
        return;
    }
    for (size_t i = 0; i < wFdsLen; i++) {
        if (wfds[i].fd != fd) {
            continue;
        }
        size_t remaining = wFdsLen - i;
        memmove(wfds + i, wfds + i + 1, remaining * sizeof(struct pollfd));
        return;
    }
}

IMPLEMENT_SELF_METHOD(void, addCoroutine, IOCoroutine coroutine) {
    List_add(this.data->ioCoroutines, coroutine.asObject, CRASH_ON_EXCEPTION);
}

IMPLEMENT_SELF_METHOD(void, removeCoroutine, IOCoroutine coroutine) {
    // todo: remove
    size_t i = 0;

    foreach(Object, coro, List_as_Iterable(this.data->ioCoroutines), {
        if (Object_equals(coro, coroutine.asObject)) {
            break;
        }
        i++;
    })
    List_removeAt(this.data->ioCoroutines, i, CRASH_ON_EXCEPTION);
}

// Beautiful piece of code!
IMPLEMENT_SELF_METHOD(void, watchFd, int fd, short events) {
    struct pollfd **wfds = &this.data->watched_fds;
    nfds_t *wFdsLen = &this.data->watched_fds_length;
    nfds_t *wFdsCap = &this.data->watched_fds_capacity;
    // Allocate initial
    if (*wfds == NULL) {
        *wFdsCap = 16;
        *wFdsLen = 0;
        *wfds = malloc(sizeof(struct pollfd) * *wFdsCap);
    }
    // Ensure can fit one
    if (*wFdsCap < *wFdsLen + 1) {
        *wFdsCap <<= 1;
        *wfds = realloc(*wfds, sizeof(struct pollfd) * *wFdsCap);
    }
    // Set it!
    struct pollfd *newWatcher = *wfds + (*wFdsLen)++;
    newWatcher->fd = fd;
    newWatcher->revents = 0;
    newWatcher->events = events; // all
}

_Thread_local Thread _Thread_current;
IMPLEMENT_SELF_METHOD(int, runInCurrentThread) {
    _Thread_current = this;
    EventLoop loop = this.data->loop;
    Function main = this.data->main;
    Completer completer = this.data->completer;
    ThreadChildren children = this.data->children;
    List ioCoroutines = this.data->ioCoroutines;
    Throwable EXCEPTION = DOWNCAST(null, Throwable);
    Object res = Function_call(main, 1, &EXCEPTION);
    int thread_res = 0;
    // todo: do i need to reset revent?
    if (!Object_isNull(EXCEPTION.asObject)) {
        // thrown
        APPEND_STACK(EXCEPTION)
        goto exception;
    }
    if (EventLoop_empty(loop) && ThreadChildren_empty(children) && List_length(ioCoroutines) == 0) {
        assert(!IS_OBJECT_ASSIGNABLE(res, Future));
        goto success;
    }

    while (true) {
        EventLoop_drain(loop);
        if (ThreadChildren_empty(children) && List_length(ioCoroutines) == 0) {
            break;
        }

        struct pollfd *watched_fds = this.data->watched_fds;
        nfds_t watched_fds_length = this.data->watched_fds_length;
        int pollRes = poll(watched_fds, watched_fds_length, -1);
        if (pollRes < 0) {
            EXCEPTION = IOException$make_fromErrno().asThrowable;
            APPEND_STACK(EXCEPTION)
            goto exception;
        }
        nfds_t fdsWithEvent = pollRes;
        if (fdsWithEvent == 0) {
            // wtf, this is a timeout
            continue;
        }
        // first check the event loop notifier
        struct pollfd *eventLoopFd = watched_fds;
        if (eventLoopFd->revents != 0) {
            // weee. we will loop again.
            EventLoop_ackNotification(loop);
            fdsWithEvent--;
            eventLoopFd->revents = 0;
        }
        if (fdsWithEvent == 0) {
            continue;
        }
        // find the culprits and tell them to perform a step
        foreach(IOCoroutine, coro, List_as_Iterable(ioCoroutines), {
            // unfortunate loop inside of loop but i dont give a fuck
            // TODO: Give a fuck
            for (size_t i = 1; i < watched_fds_length; i++) {
                struct pollfd *fd = watched_fds + i;
                if (fd->revents != 0) {
                    if (IOCoroutine_ownsFd(coro, fd->fd)) {
                        IOCoroutine_scheduleStep(coro);
                        fd->revents = 0;
                        break;
                    }
                }
            }
            if (fdsWithEvent == 0) {
                break;
            }
        })
    }

    // Check if it is future
    if (IS_OBJECT_ASSIGNABLE(res, Future)) {
        Future futureRes = Future$$fromObject(res);
        switch (futureRes.data->state) {
            case FutureState$pending: {
                EXCEPTION = Error$make_new(StringRef$wrap("Unexpected state, this means the event loop was not drained!").asString).asThrowable;
                APPEND_STACK(EXCEPTION)
                goto exception;
            }
            case FutureState$complete: {
                res = futureRes.data->value;
                break;
            }
            case FutureState$completedWithError: {
                EXCEPTION = futureRes.data->exception;
                APPEND_STACK(EXCEPTION)
                goto exception;
            }
        }
    }

    success:

    Completer_complete(completer, res);
    if (IS_OBJECT_ASSIGNABLE(res, Integer)) {
        thread_res = DOWNCAST(res, Integer).unwrap;
    }
    goto exit;

    exception:
    Completer_completeException(completer, EXCEPTION);
    thread_res = -1;

    goto exit;

    exit:;
    if (Object_isNull(this.data->siblings.asObject)) {
        return thread_res;
    }
    ThreadChildren_stopped(this.data->siblings, this, &EXCEPTION);
    return thread_res;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0
    );
    // Thread
    vtable->kill = _Thread_kill_impl;
    vtable->runInCurrentThread = _Thread_runInCurrentThread_impl;
    vtable->watchFd = _Thread_watchFd_impl;
    vtable->removeWatchedFd = _Thread_removeWatchedFd_impl;
    vtable->addCoroutine = _Thread_addCoroutine_impl;
    vtable->removeCoroutine = _Thread_removeCoroutine_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}

IMPLEMENT_SELF_GETTER(Future, result) {
    return Completer_future(this.data->completer);
}

IMPLEMENT_SELF_GETTER(EventLoop, loop) {
    return this.data->loop;
}

IMPLEMENT_CONSTRUCTOR(main, Function entry) {
    this.data->main = entry;
    this.data->completer = Completer$make_new();
    this.data->loop = EventLoop$make_new();
    this.data->children = ThreadChildren$make_new();
    this.data->ioCoroutines = List_new();
    this.data->siblings = DOWNCAST(null, ThreadChildren);
    this.data->thread = thrd_current();
    this.data->watched_fds = NULL;
    this.data->watched_fds_capacity = 0;
    this.data->watched_fds_length = 0;

    Thread_watchFd(this, EventLoop_fd(this.data->loop), -1);
}

IMPLEMENT_CONSTRUCTOR(new, Function main) {
    this.data->main = main;
    this.data->completer = Completer$make_new();
    this.data->loop = EventLoop$make_new();
    this.data->ioCoroutines = List_new();
    this.data->children = ThreadChildren$make_new();
    this.data->siblings = Thread_current().data->children;
    this.data->watched_fds = NULL;
    this.data->watched_fds_capacity = 0;
    this.data->watched_fds_length = 0;

    Thread_watchFd(this, EventLoop_fd(this.data->loop), -1);
}

IMPLEMENT_STATIC_METHOD(Thread, spawnSync, Function main, THROWS) {
    Thread thread = Thread$make_new(main);
    thrd_create(&thread.data->thread, threadMain, thread.data);
    ThreadChildren_spawned(thread.data->siblings, thread, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, Thread))
    }
    return thread;
}


IMPLEMENT_STATIC_METHOD(Thread, current) {
    return _Thread_current;
}


#undef Super
#undef Self
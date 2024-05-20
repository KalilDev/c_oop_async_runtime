#include "Object.h"
#include "Throwable.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "Task.h"
#include "EventLoop.h"
#include "IOException.h"
#include "IOCoroutine.h"
#include "Thread.h"
#include "StreamSubscription.h"
#include <assert.h>
#include <threads.h>
#include <unistd.h>
#include <fcntl.h>

#define Super() Object_vtable()
#define Self EventLoop
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_EVENT_LOOP_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    printf("Deleting event loop!\n");
    fflush(stdout);
}

IMPLEMENT_SELF_METHOD(void, notifyWakeup) {
    if (!this.data->needToBeNotified) {
        return;
    }
    assert(!this.data->notified);
    char msg = 'O';
    write(this.data->wakeupFd, &msg, 1);
    this.data->notified = true;
}

IMPLEMENT_SELF_METHOD(void, ackNotification) {
    assert(this.data->notified);
    char msg = '\0';
    ssize_t res = read(this.data->wakeupListenerFd, &msg, 1);
    assert(res > 0);
    assert(res == 1);
    assert(msg == 'O');
    this.data->notified = false;
}

IMPLEMENT_SELF_METHOD(Task, popTask) {
    mtx_t *queueMutex = &this.data->queueMutex;
    List tasks = this.data->enqueuedTasks;
    mtx_lock(queueMutex);

    if (List_length(tasks) == 0) {
        mtx_unlock(queueMutex);
        return DOWNCAST(null, Task);
    }
    Object task = List_removeAt(tasks, 0, CRASH_ON_EXCEPTION);

    mtx_unlock(queueMutex);
    return Task$$fromObject(task);
}

IMPLEMENT_SELF_METHOD(bool, empty) {
    mtx_t *queueMutex = &this.data->queueMutex;
    List tasks = this.data->enqueuedTasks;
    mtx_lock(queueMutex);
    bool isEmpty = List_length(tasks) == 0;
    mtx_unlock(queueMutex);
    return isEmpty;
}
IMPLEMENT_SELF_METHOD(void, pushTask, Task task) {
    mtx_t *queueMutex = &this.data->queueMutex;
    List tasks = this.data->enqueuedTasks;
    mtx_lock(queueMutex);
    bool wasEmpty = List_length(tasks) == 0;
    List_add(tasks, task.asObject, CRASH_ON_EXCEPTION);
    mtx_unlock(queueMutex);
    if (wasEmpty) {
        EventLoop_notifyWakeup(this);
    }
}

IMPLEMENT_SELF_METHOD(Future, invokeTask, Task task) {
    Future onComplete = Task_future(task);
    EventLoop_pushTask(this, task);
    return onComplete;
}

IMPLEMENT_SELF_METHOD(void, drain) {
    Task task = DOWNCAST(null, Task);
    this.data->needToBeNotified = false;

    while (!Object_isNull((task = EventLoop_popTask(this)).asObject)) {
        Task_invokeSync(task);
    }

    this.data->needToBeNotified = true;
}
#define Self EventLoop
IMPLEMENT_SELF_METHOD(void, runUntilCompletion, Function onUnhandledAsyncException, THROWS) {
    Thread thread = Thread_current();
    if (EventLoop_empty(this) && !Thread_hasChildren(thread) && !EventLoop_hasCoroutines(this) && !EventLoop_hasActiveSubscriptions(this)) {
        return;
    }

    mtx_t *fdsMtx = &this.data->watchedFdsMutex;
    mtx_t *ioCorosMtx = &this.data->ioCoroutinesMutex;
    List ioCoroutines = this.data->ioCoroutines;
    while (true) {
        // first drain the current events
        EventLoop_drain(this);
        assert(EventLoop_empty(this));

        // If there arent any possibilities that the event loop will have new events added, exit
        if (!Thread_hasChildren(thread) && !EventLoop_hasCoroutines(this) && !EventLoop_hasActiveSubscriptions(this)) {
            return;
        }

        // Poll for the next time we need to loop (when another thread added something, or when an io coroutine is ready
        // to read
        mtx_lock(fdsMtx);
        struct pollfd *watched_fds = this.data->watched_fds;
        nfds_t watched_fds_length = this.data->watched_fds_length;

        int pollRes = poll(watched_fds, watched_fds_length, -1);
        if (pollRes < 0) {
            mtx_unlock(fdsMtx);
            THROW(IOException$make_fromErrno())
        }
        nfds_t fdsWithEvent = pollRes;
        if (fdsWithEvent == 0) {
            // wtf, this is a timeout
            // TODO: Throw?
            mtx_unlock(fdsMtx);
            continue;
        }
        // first check the event loop notifier
        struct pollfd *eventLoopFd = watched_fds;
        if (eventLoopFd->revents != 0) {
            // weee. we will loop again.
            EventLoop_ackNotification(this);
            fdsWithEvent--;
            eventLoopFd->revents = 0;
        }
        if (fdsWithEvent == 0) {
            // only the event loop had changes
            mtx_unlock(fdsMtx);
            continue;
        }
        // find the culprits and tell them to perform a step
        mtx_lock(ioCorosMtx);
        foreach(IOCoroutine, coro, List_as_Iterable(ioCoroutines), {
            for (size_t i = 1; i < watched_fds_length; i++) {
                struct pollfd *fd = watched_fds + i;
                if (fd->revents != 0) {
                    if (IOCoroutine_ownsFd(coro, fd->fd)) {
                        // this coro had an event. step it
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
        mtx_unlock(ioCorosMtx);
        mtx_unlock(fdsMtx);
    }
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
    mtx_lock(&this.data->ioCoroutinesMutex);
    List_add(this.data->ioCoroutines, coroutine.asObject, CRASH_ON_EXCEPTION);
    mtx_unlock(&this.data->ioCoroutinesMutex);
}

IMPLEMENT_SELF_METHOD(void, removeCoroutine, IOCoroutine coroutine) {
    mtx_lock(&this.data->ioCoroutinesMutex);
    List_remove(this.data->ioCoroutines, coroutine.asObject, CRASH_ON_EXCEPTION);;
    mtx_unlock(&this.data->ioCoroutinesMutex);
}

IMPLEMENT_SELF_METHOD(void, addSubscription, StreamSubscription subscription) {
    mtx_lock(&this.data->subscriptionsMutex);
    List_add(this.data->activeSubscriptions, subscription.asObject, CRASH_ON_EXCEPTION);
    mtx_unlock(&this.data->subscriptionsMutex);
}

IMPLEMENT_SELF_METHOD(void, removeSubscription, StreamSubscription subscription) {
    mtx_lock(&this.data->subscriptionsMutex);
    List_remove(this.data->activeSubscriptions, subscription.asObject, CRASH_ON_EXCEPTION);;
    mtx_unlock(&this.data->subscriptionsMutex);
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

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // EventLoop
    vtable->popTask = _EventLoop_popTask_impl;
    vtable->pushTask = _EventLoop_pushTask_impl;
    vtable->invokeTask = _EventLoop_invokeTask_impl;
    vtable->notifyWakeup = _EventLoop_notifyWakeup_impl;
    vtable->ackNotification = _EventLoop_ackNotification_impl;
    vtable->drain = _EventLoop_drain_impl;
    vtable->empty = _EventLoop_empty_impl;
    vtable->runUntilCompletion = _EventLoop_runUntilCompletion_impl;

    vtable->watchFd = _EventLoop_watchFd_impl;
    vtable->removeWatchedFd = _EventLoop_removeWatchedFd_impl;
    vtable->addCoroutine = _EventLoop_addCoroutine_impl;
    vtable->removeCoroutine = _EventLoop_removeCoroutine_impl;
    vtable->addSubscription = _EventLoop_addSubscription_impl;
    vtable->removeSubscription = _EventLoop_removeSubscription_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _EventLoop_delete_impl;
}

IMPLEMENT_STATIC_METHOD(EventLoop, current) {
    return Thread_loop(Thread_current());
}

IMPLEMENT_CONSTRUCTOR(new) {
    this.data->enqueuedTasks = List_new();
    mtx_init(&this.data->queueMutex, mtx_plain);
    mtx_init(&this.data->ioCoroutinesMutex, mtx_plain);
    mtx_init(&this.data->watchedFdsMutex, mtx_plain);
    mtx_init(&this.data->subscriptionsMutex, mtx_plain);
    int fds[2] = {0};
    int res = pipe(fds);
    if (res < 0) {
        perror("event loop new");
        return;
    }
    this.data->wakeupListenerFd = fds[0];
    fcntl( fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);
    this.data->wakeupFd = fds[1];

    this.data->ioCoroutines = List_new();
    this.data->watched_fds = NULL;
    this.data->watched_fds_capacity = 0;
    this.data->watched_fds_length = 0;
    this.data->activeSubscriptions = List_new();

    this.data->notified = false;
    this.data->needToBeNotified = false;

    EventLoop_watchFd(this, fds[0], -1);
}

IMPLEMENT_SELF_GETTER(int, fd) {
    return this.data->wakeupListenerFd;
}
IMPLEMENT_SELF_GETTER(bool, hasCoroutines) {
    mtx_t *mtx = &this.data->ioCoroutinesMutex;
    mtx_lock(mtx);
    bool has = List_length(this.data->ioCoroutines) != 0;
    mtx_unlock(mtx);
    return has;
}

IMPLEMENT_SELF_GETTER(bool, hasActiveSubscriptions) {
    mtx_t *mtx = &this.data->subscriptionsMutex;
    mtx_lock(mtx);
    bool has = List_length(this.data->activeSubscriptions) != 0;
    mtx_unlock(mtx);
    return has;
}

#undef Super
#undef Self
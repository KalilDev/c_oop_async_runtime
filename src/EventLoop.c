#include "Object.h"
#include "Throwable.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "Task.h"
#include "EventLoop.h"
#include "IOException.h"
#include "Thread.h"
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
    // todo
}

IMPLEMENT_SELF_METHOD(void, notifyWakeup) {
    if (!this.data->wasStarted) {
        return;
    }
    char msg = 'O';
    write(this.data->wakeupFd, &msg, 1);
}

IMPLEMENT_SELF_METHOD(void, ackNotification) {
    char msg = '\0';
    ssize_t res = read(this.data->wakeupListenerFd, &msg, 1);
    assert(res > 0);
    assert(res == 1);
    assert(msg == 'O');
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
    if (wasEmpty && !this.data->wasStarted) {
        this.data->wasStarted = true;
    }
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
    while (!Object_isNull((task = EventLoop_popTask(this)).asObject)) {
        Task_invokeSync(task);
    }
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
    this.data->wasStarted = false;
    int fds[2] = {0};
    int res = pipe(fds);
    if (res < 0) {
        perror("event loop new");
        return;
    }
    this.data->wakeupListenerFd = fds[0];
    fcntl( fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);
    this.data->wakeupFd = fds[1];
}

IMPLEMENT_SELF_GETTER(int, fd) {
    return this.data->wakeupListenerFd;
}

#undef Super
#undef Self
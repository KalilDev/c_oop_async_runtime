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

#define Super() Object_vtable()
#define Self EventLoop
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_EVENT_LOOP_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // todo
}

IMPLEMENT_SELF_METHOD(void, blockUntilNextTask, THROWS) {
    cnd_t *taskAdded = &this.data->taskAdded;
    mtx_t *queueMutex = &this.data->queueMutex;
    mtx_lock(queueMutex);
    int res = cnd_wait(taskAdded, queueMutex);
    if (res == thrd_error) {
        THROW(IOException$make_fromErrno())
    }
    if (res == thrd_success) {
        return;
    }
    THROW(Exception$make_new(StringRef$wrap("Error").asString))
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
    cnd_t *taskAdded = &this.data->taskAdded;
    mtx_t *queueMutex = &this.data->queueMutex;
    List tasks = this.data->enqueuedTasks;
    mtx_lock(queueMutex);
    List_add(tasks, task.asObject, CRASH_ON_EXCEPTION);
    mtx_unlock(queueMutex);
    cnd_signal(taskAdded);
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
    cnd_init(&this.data->taskAdded);
}

#undef Super
#undef Self
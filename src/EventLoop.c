#include "Object.h"
#include "Throwable.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "Task.h"
#include "EventLoop.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self EventLoop
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_EVENT_LOOP_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // todo
}

static EventLoop loop = {0};

IMPLEMENT_SELF_METHOD(Task, popTask) {
    List tasks = this.data->enqueuedTasks;
    if (List_length(tasks) == 0) {
        return DOWNCAST(null, Task);
    }
    return Task$$fromObject(List_removeAt(tasks, 0, CRASH_ON_EXCEPTION));
}
IMPLEMENT_SELF_METHOD(void, pushTask, Task task) {
    List tasks = this.data->enqueuedTasks;
    List_add(tasks, task.asObject, CRASH_ON_EXCEPTION);
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
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _EventLoop_delete_impl;
}

IMPLEMENT_STATIC_METHOD(EventLoop, instance) {
    if (loop.vtable != NULL) {
        return loop;
    }
    loop = EventLoop$make__();
    return loop;
}

IMPLEMENT_CONSTRUCTOR(_) {
    this.data->enqueuedTasks = List_new();
}

#undef Super
#undef Self
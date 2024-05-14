#include "Object.h"
#include "Task.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "EventLoop.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self Task
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_TASK_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // todo
}

#define ENUMERATE_TASK_GETTERS(GETTER) \
    GETTER(Future, future)

IMPLEMENT_SELF_METHOD(void, invokeSync) {
    Completer completer = this.data->completer;
    Function function = this.data->function;
    Throwable EXCEPTION = DOWNCAST(null, Throwable);
    Object res = Function_call(function, 1, &EXCEPTION);
    if (!Object_isNull(EXCEPTION.asObject)) {
        APPEND_STACK(EXCEPTION)
        Completer_completeException(completer, EXCEPTION);
        return;
    }
    Completer_complete(completer, res);
}

IMPLEMENT_SELF_METHOD(Future, invoke) {
    EventLoop loop = EventLoop_instance();
    return EventLoop_invokeTask(loop, this);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // Task
    vtable->invoke = _Task_invoke_impl;
    vtable->invokeSync = _Task_invokeSync_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Task_delete_impl;
}

IMPLEMENT_SELF_GETTER(Future, future) {
    return Completer_future(this.data->completer);
}

IMPLEMENT_CONSTRUCTOR(new, Function function) {
    this.data->completer = Completer$make_new();
    this.data->function = function;
}

#undef Super
#undef Self
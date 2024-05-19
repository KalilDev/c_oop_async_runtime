#include "Object.h"
#include "Task.h"
#include "String.h"
#include "Thread.h"
#include "IOCoroutine.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self IOCoroutine
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_IO_COROUTINE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_IO_COROUTINE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // todo
}

IMPLEMENT_SELF_METHOD(bool, ownsFd, int fd) {
    return this.data->fd == fd;
}
IMPLEMENT_SELF_METHOD(void, remove) {
    int fd = this.data->fd;
    Thread thread = Thread_current();
    Thread_removeWatchedFd(thread, fd);
    Thread_removeCoroutine(thread, this);
}

IMPLEMENT_SELF_METHOD(void, scheduleStep) {
    Future_computation(this.data->step);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // IOCoroutine
    vtable->ownsFd = _IOCoroutine_ownsFd_impl;
    vtable->remove = _IOCoroutine_remove_impl;
    vtable->scheduleStep = _IOCoroutine_scheduleStep_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _IOCoroutine_delete_impl;
}

IMPLEMENT_CONSTRUCTOR(new, Function step, int fd, short usedEvents) {
    this.data->fd = fd;
    this.data->step = step;
    Thread thread = Thread_current();
    Thread_addCoroutine(thread, this);
    Thread_watchFd(thread, fd, usedEvents);
}

#undef Super
#undef Self
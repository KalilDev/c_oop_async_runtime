#include "Object.h"
#include "Task.h"
#include "String.h"
#include "Thread.h"
#include "IOCoroutine.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "primitive/Bool.h"
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
    IOCoroutineState *state = &this.data->state;
    switch (*state) {
        case IOCoroutineState$pending: {
            assert(!"The IOCoroutine could not have been removed if it was pending");
            break;
        }
        case IOCoroutineState$enabled: {
            *state = IOCoroutineState$removed;
            break;
        }
        case IOCoroutineState$disabled: {
            *state = IOCoroutineState$removed;
            break;
        }
        case IOCoroutineState$removed: {
            assert(!"The IOCoroutine cannot be removed twice");
            break;
        }
    }
    int fd = this.data->fd;
    EventLoop loop = EventLoop_current();
    EventLoop_removeWatchedFd(loop, fd);
    EventLoop_removeCoroutine(loop, this);
}

#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(IOCoroutine, myself)

IMPLEMENT_LAMBDA(OnFinishStep, CAPTURE_MYSELF, NO_OWNED_CAPTURES, IOCoroutine myself) {
    Lambda_OnFinishStep self = DOWNCAST(this, Lambda_OnFinishStep);
    IOCoroutine myself = self.data->myself;
    Object rawValue = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);
    Bool result = Bool$$fromObject(rawValue);
    if (result.unwrap) {
        // business as usual
        return null;
    }
    IOCoroutineState *state = &myself.data->state;
    switch (*state) {
        case IOCoroutineState$pending: {
            assert(!"The IOCoroutine could not have ran if it was pending");
            break;
        }
        case IOCoroutineState$enabled: {
            *state = IOCoroutineState$disabled;
            break;
        }
        case IOCoroutineState$disabled: {
            assert(!"The IOCoroutine could not have ran if it was disabled");
            break;
        }
        case IOCoroutineState$removed: {
            // okie, it was removed between the step being scheduled and now
            return null;
        }
    }
}

IMPLEMENT_SELF_METHOD(void, scheduleStep) {
    switch (this.data->state) {
        case IOCoroutineState$pending: {
            assert(!"The IOCoroutine cannot run a step before being added!");
            break;
        }
        case IOCoroutineState$enabled: {
        // todo: deal with exception?
            Future_then(Future_computation(this.data->step), Lambda_OnFinishStep$make_new(this).asFunction);
            return;
        }
        case IOCoroutineState$disabled: {
            return;
        }
        case IOCoroutineState$removed: {
            assert(!"The IOCoroutine cannot run a step after being removed!");
            break;
        }
    }
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

    IOCoroutineState *state = &this.data->state;
    *state = IOCoroutineState$pending;
    EventLoop loop = EventLoop_current();
    EventLoop_watchFd(loop, fd, usedEvents);
    EventLoop_addCoroutine(loop, this);
    *state = IOCoroutineState$enabled;
}

#undef Super
#undef Self
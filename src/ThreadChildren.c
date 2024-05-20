#include "Object.h"
#include "Throwable.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include "Task.h"
#include "ThreadChildren.h"
#include "IOException.h"
#include "Thread.h"
#include <assert.h>
#include <threads.h>

#define Super() Object_vtable()
#define Self ThreadChildren
IMPLEMENT_OPERATOR_NEW()
Iterable ThreadChildren_as_Iterable(ThreadChildren children) {
    return List_as_Iterable(children.data->threads);
}
ENUMERATE_THREAD_CHILDREN_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // todo
}

IMPLEMENT_SELF_METHOD(void, blockUntilChanges, THROWS) {
    cnd_t *threadsChanged = &this.data->threadsChanged;
    mtx_t *threadsMutex = &this.data->threadsMutex;
    mtx_lock(threadsMutex);
    int res = cnd_wait(threadsChanged, threadsMutex);
    mtx_unlock(threadsMutex);
    if (res == thrd_error) {
        THROW(IOException$make_fromErrno())
    }
    if (res == thrd_success) {
        return;
    }
    THROW(Exception$make_new(StringRef$wrap("Error").asString))
}

IMPLEMENT_SELF_METHOD(void, spawned, Thread thread, THROWS) {
    cnd_t *threadsChanged = &this.data->threadsChanged;
    mtx_t *threadsMutex = &this.data->threadsMutex;
    List threads = this.data->threads;
    mtx_lock(threadsMutex);

    List_add(threads, thread.asObject, EXCEPTION);
    if (HAS_EXCEPTION) {
        mtx_unlock(threadsMutex);
        RETHROW()
    }

    cnd_signal(threadsChanged);
    mtx_unlock(threadsMutex);
}

IMPLEMENT_SELF_METHOD(void, stopped, Thread thread, THROWS) {
    cnd_t *threadsChanged = &this.data->threadsChanged;
    mtx_t *threadsMutex = &this.data->threadsMutex;
    List threads = this.data->threads;
    mtx_lock(threadsMutex);

    Object removed = List_remove(threads, thread.asObject, CRASH_ON_EXCEPTION);
    if (!Object_isNull(removed)) {
        mtx_unlock(threadsMutex);
        THROW(Exception$make_new(StringRef$wrap("Couldnt find the stopped thread").asString))
    }

    if (HAS_EXCEPTION) {
        mtx_unlock(threadsMutex);
        RETHROW()
    }

    cnd_signal(threadsChanged);
    mtx_unlock(threadsMutex);
}

IMPLEMENT_SELF_METHOD(bool, empty) {
    mtx_t *threadsMutex = &this.data->threadsMutex;
    List threads = this.data->threads;
    mtx_lock(threadsMutex);
    bool isEmpty = List_length(threads) == 0;
    mtx_unlock(threadsMutex);
    return isEmpty;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // ThreadChildren
    vtable->empty = _ThreadChildren_empty_impl;
    vtable->spawned = _ThreadChildren_spawned_impl;
    vtable->stopped = _ThreadChildren_stopped_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _ThreadChildren_delete_impl;
}

IMPLEMENT_STATIC_METHOD(EventLoop, current) {
    return Thread_loop(Thread_current());
}

IMPLEMENT_CONSTRUCTOR(new) {
    this.data->threads = List_new();
    mtx_init(&this.data->threadsMutex, mtx_plain);
    cnd_init(&this.data->threadsChanged);
}

#undef Super
#undef Self
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

_Thread_local Thread _Thread_current;
IMPLEMENT_SELF_METHOD(int, runInCurrentThread) {
    _Thread_current = this;
    EventLoop loop = this.data->loop;
    Function main = this.data->main;
    Completer completer = this.data->completer;
    ThreadChildren children = this.data->children;
    Throwable EXCEPTION = DOWNCAST(null, Throwable);
    Object res = Function_call(main, 1, &EXCEPTION);
    int thread_res = 0;

    if (!Object_isNull(EXCEPTION.asObject)) {
        // thrown
        APPEND_STACK(EXCEPTION)
        goto exception;
    }
    if (EventLoop_empty(loop) && ThreadChildren_empty(children)) {
        assert(!IS_OBJECT_ASSIGNABLE(res, Future));
        goto success;
    }

    while (true) {
        EventLoop_drain(loop);
        if (ThreadChildren_empty(children)) {
            break;
        }

        EventLoop_blockUntilNextTask(loop, &EXCEPTION);
        if (!Object_isNull(EXCEPTION.asObject)) {
            // locking error
            APPEND_STACK(EXCEPTION)
            goto exception;
        }
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
    this.data->siblings = DOWNCAST(null, ThreadChildren);
    this.data->thread = thrd_current();
}

IMPLEMENT_CONSTRUCTOR(new, Function main) {
    this.data->main = main;
    this.data->completer = Completer$make_new();
    this.data->loop = EventLoop$make_new();
    this.data->children = ThreadChildren$make_new();
    this.data->siblings = Thread_current().data->children;
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
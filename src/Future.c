#include "Object.h"
#include "Future.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include <assert.h>
#include "Task.h"
#include "Thread.h"
#include "EventLoop.h"
#define Super() Object_vtable()
#define Self Future
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_FUTURE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_FUTURE_PARENTS)
// TODO
//IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
//    Throwable self = DOWNCAST(this, Throwable);
//    Object_delete(self.data->frames.asObject);
//    Super()->delete(this);
//}

#define ENUMERATE_THEN_CAPTURES(CAPTURE) \
    CAPTURE(Object, value)               \
    CAPTURE(Function, function)

#define ENUMERATE_THEN_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(function)

#define ENUMERATE_CATCH_CAPTURES(CAPTURE) \
    CAPTURE(Throwable, exception)          \
    CAPTURE(Function, function)

#define ENUMERATE_CATCH_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(exception)

IMPLEMENT_LAMBDA(Then, ENUMERATE_THEN_CAPTURES, ENUMERATE_THEN_OWNED_CAPTURES, Object value, Function function) {
    Lambda_Then self = DOWNCAST(this, Lambda_Then);
    THROWS = va_arg(args, Throwable*);
    Object value = self.data->value;
    Function function = self.data->function;
    Object res =  Function_call(function, 2, value, THROWS_PARAM_INVOCATION);
    if (HAS_EXCEPTION) {
        RETHROW(res)
    }
    return res;
}

IMPLEMENT_LAMBDA(Catch, ENUMERATE_CATCH_CAPTURES, ENUMERATE_CATCH_OWNED_CAPTURES, Throwable exception, Function function) {
    Lambda_Catch self = DOWNCAST(this, Lambda_Catch);
    THROWS = va_arg(args, Throwable*);
    Throwable exception = self.data->exception;
    assert(!Object_isNull(exception.asObject));
    Function function = self.data->function;
    Object res =  Function_call(function, 2, exception, THROWS_PARAM_INVOCATION);
    if (HAS_EXCEPTION) {
        RETHROW(res)
    }
    return res;
}

#define ENUMERATE_COMPLETE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)            \
    CAPTURE(Function, then)                  \
    CAPTURE(EventLoop, loop)

#define ENUMERATE_COMPLETE_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(completer)                             \
    CAPTURE(then)

IMPLEMENT_LAMBDA(FutureCompleteThen, ENUMERATE_COMPLETE_CAPTURES, ENUMERATE_COMPLETE_OWNED_CAPTURES, Completer completer, Function then, EventLoop loop) {
    Lambda_FutureCompleteThen self = DOWNCAST(this, Lambda_FutureCompleteThen);
    Completer completer = self.data->completer;
    EventLoop loop = self.data->loop;
    Function then = self.data->then;
    Object value = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);

    Lambda_Then lambda = Lambda_Then$make_new(value, then);
    Future result = Future_computationAt(lambda.asFunction, loop);
    Completer_follow(completer, result);

    return null;
}


#define ENUMERATE_COMPLETE_WITH_ERROR_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

#define ENUMERATE_COMPLETE_WITH_ERROR_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(completer)

IMPLEMENT_LAMBDA(FutureCompleteWithErrorThen, ENUMERATE_COMPLETE_WITH_ERROR_CAPTURES, ENUMERATE_COMPLETE_WITH_ERROR_OWNED_CAPTURES, Completer completer) {
    Lambda_FutureCompleteThen self = DOWNCAST(this, Lambda_FutureCompleteThen);
    Completer completer = self.data->completer;
    Throwable exception = va_arg(args, Throwable);
    assert(!Object_isNull(exception.asObject));
    THROWS = va_arg(args, Throwable*);

    Completer_completeException(completer, exception);
    return null;
}


#define ENUMERATE_COMPLETE_WITH_ERROR_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

#define ENUMERATE_COMPLETE_WITH_ERROR_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(completer)

IMPLEMENT_LAMBDA(FutureCompleteWithDataCatch, ENUMERATE_COMPLETE_WITH_ERROR_CAPTURES, ENUMERATE_COMPLETE_WITH_ERROR_OWNED_CAPTURES, Completer completer) {
    Lambda_FutureCompleteThen self = DOWNCAST(this, Lambda_FutureCompleteThen);
    Completer completer = self.data->completer;
    Object value = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);

    Completer_complete(completer, value);
    return null;
}

#define ENUMERATE_COMPLETE_CATCH_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)                  \
    CAPTURE(EventLoop, loop)            \
    CAPTURE(Function, catch)


#define ENUMERATE_COMPLETE_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(completer)            \
    CAPTURE(catch)

IMPLEMENT_LAMBDA(FutureCompleteCatch, ENUMERATE_COMPLETE_CATCH_CAPTURES, ENUMERATE_COMPLETE_OWNED_CAPTURES, Completer completer, EventLoop loop, Function catch) {
    Lambda_FutureCompleteCatch self = DOWNCAST(this, Lambda_FutureCompleteCatch);
    Completer completer = self.data->completer;
    EventLoop loop = self.data->loop;
    Function catch = self.data->catch;
    Throwable exception = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);

    Lambda_Catch lambda = Lambda_Catch$make_new(exception, catch);
    Future result = Future_computationAt(lambda.asFunction, loop);
    Completer_follow(completer, result);

    return null;
}

IMPLEMENT_SELF_METHOD(void, _onComplete) {
    assert(!IS_OBJECT_ASSIGNABLE(this.data->value, Future));
    this.data->state = FutureState$complete;
    Function then = this.data->then;
    if (Object_isNull(then.asObject)) {
        // TODO: Delete the future?
        return;
    }
    Lambda_Then lambda = Lambda_Then$make_new(this.data->value, then);
    Task task = Task$make_new(lambda.asFunction);
    EventLoop loop = this.data->attachedLoop;
    assert(!Object_isNull(loop.asObject));
    EventLoop_pushTask(loop, task);
}

IMPLEMENT_SELF_METHOD(void, _onCompleteWithError) {
    assert(!IS_OBJECT_ASSIGNABLE(this.data->exception.asObject, Future));
    assert(!Object_isNull(this.data->exception.asObject));
    this.data->state = FutureState$completedWithError;
    Function catch = this.data->catch;
    if (Object_isNull(catch.asObject)) {
        Thread current = Thread_current();
        Thread_unhandledAsyncException(current, this.data->exception);
        // TODO: Delete the future?
        return;
    }
    Lambda_Catch lambda = Lambda_Catch$make_new(this.data->exception, catch);
    Task task = Task$make_new(lambda.asFunction);
    EventLoop loop = this.data->attachedLoop;
    assert(!Object_isNull(loop.asObject));
    EventLoop_pushTask(loop, task);
}

IMPLEMENT_SELF_METHOD(Future, catch, Function catch) {
    assert(Object_isNull(this.data->catch.asObject));
    assert(Object_isNull(this.data->then.asObject));
    assert(Object_isNull(this.data->attachedLoop.asObject));
    switch (this.data->state) {
        case FutureState$complete: {
            return Future$make_value(this.data->value);
        }
        case FutureState$pending: {
            Completer completer = Completer$make_new();
            EventLoop loop = EventLoop_current();

            Lambda_FutureCompleteCatch lambda = Lambda_FutureCompleteCatch$make_new(completer, loop, catch);
            Lambda_FutureCompleteWithDataCatch successLambda = Lambda_FutureCompleteWithDataCatch$make_new(completer);

            this.data->then = successLambda.asFunction;
            this.data->catch = lambda.asFunction;
            this.data->attachedLoop = loop;

            EventLoop_addFuture(loop, this);
            return Completer_future(completer);
        }
        case FutureState$completedWithError:{
            assert(!Object_isNull(this.data->exception.asObject));
            Lambda_Catch lambda = Lambda_Catch$make_new(this.data->exception, catch);
            return Future_computationAt(lambda.asFunction, EventLoop_current());
        }
    }
}
IMPLEMENT_SELF_METHOD(Future, then, Function then) {
    assert(Object_isNull(this.data->catch.asObject));
    assert(Object_isNull(this.data->then.asObject));
    assert(Object_isNull(this.data->attachedLoop.asObject));
    switch (this.data->state) {
        case FutureState$complete: {
            Lambda_Then lambda = Lambda_Then$make_new(this.data->value, then);
            return Future_computationAt(lambda.asFunction, EventLoop_current());
        }
        case FutureState$pending: {
            Completer completer = Completer$make_new();
            EventLoop loop = EventLoop_current();

            Lambda_FutureCompleteThen lambda = Lambda_FutureCompleteThen$make_new(completer, then, loop);
            Lambda_FutureCompleteWithErrorThen errorLambda = Lambda_FutureCompleteWithErrorThen$make_new(completer);

            this.data->then = lambda.asFunction;
            this.data->catch = errorLambda.asFunction;
            this.data->attachedLoop = loop;

            EventLoop_addFuture(loop, this);
            return Completer_future(completer);
        }
        case FutureState$completedWithError:{
            assert(!Object_isNull(this.data->exception.asObject));
            return Future$make_exception(this.data->exception);
        }
    }
}
IMPLEMENT_SELF_METHOD(void, setCallbacks, Function then, Function catch) {
    assert(Object_isNull(this.data->then.asObject));
    assert(Object_isNull(this.data->catch.asObject));
    assert(Object_isNull(this.data->attachedLoop.asObject));
    switch (this.data->state) {
        case FutureState$complete: {
            Lambda_Then lambda = Lambda_Then$make_new(this.data->value, then);
            Task task = Task$make_new(lambda.asFunction);
            EventLoop loop = EventLoop_current();
            EventLoop_pushTask(loop, task);
            return;
        }
        case FutureState$pending: {
            EventLoop loop = EventLoop_current();
            this.data->then = then;
            this.data->catch = catch;
            this.data->attachedLoop = loop;

            EventLoop_addFuture(loop, this);
            return;
        }
        case FutureState$completedWithError: {
            assert(!Object_isNull(this.data->exception.asObject));
            Lambda_Catch lambda = Lambda_Catch$make_new(this.data->exception, catch);
            Task task = Task$make_new(lambda.asFunction);
            EventLoop loop = EventLoop_current();
            EventLoop_pushTask(loop, task);
            return;
        }
    }
}
IMPLEMENT_SELF_METHOD(Future, handle, Function then, Function catch) {
    assert(Object_isNull(this.data->catch.asObject));
    assert(Object_isNull(this.data->then.asObject));
    assert(Object_isNull(this.data->attachedLoop.asObject));
    switch (this.data->state) {
        case FutureState$complete: {
            Lambda_Then lambda = Lambda_Then$make_new(this.data->value, then);
            return Future_computationAt(lambda.asFunction, EventLoop_current());
        }
        case FutureState$pending: {
            Completer completer = Completer$make_new();
            EventLoop loop = EventLoop_current();

            Lambda_FutureCompleteCatch catchLambda = Lambda_FutureCompleteCatch$make_new(completer, loop, catch);
            Lambda_FutureCompleteThen thenLambda = Lambda_FutureCompleteThen$make_new(completer, then, loop);

            this.data->then = catchLambda.asFunction;
            this.data->catch = thenLambda.asFunction;
            this.data->attachedLoop = loop;

            EventLoop_addFuture(loop, this);
            return Completer_future(completer);
        }
        case FutureState$completedWithError:{
            assert(!Object_isNull(this.data->exception.asObject));
            Lambda_Catch lambda = Lambda_Catch$make_new(this.data->exception, catch);
            return Future_computationAt(lambda.asFunction, EventLoop_current());
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
    // Future
    vtable->catch = _Future_catch_impl;
    vtable->then = _Future_then_impl;
    vtable->_onComplete = _Future__onComplete_impl;
    vtable->_onCompleteWithError = _Future__onCompleteWithError_impl;
    vtable->setCallbacks = _Future_setCallbacks_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _Throwable_delete_impl;
}


IMPLEMENT_STATIC_METHOD(Future, computation, Function function) {
    EventLoop current = EventLoop_current();
    Task task = Task$make_new(function);
    return EventLoop_invokeTask(current, task);
}
IMPLEMENT_STATIC_METHOD(Future, computationAt, Function function, EventLoop loop) {
    Task task = Task$make_new(function);
    return EventLoop_invokeTask(loop, task);
}

IMPLEMENT_CONSTRUCTOR(value, Object value) {
    this.data->exception = DOWNCAST(null, Throwable);
    this.data->value = value;
    this.data->state = FutureState$complete;
    this.data->then = DOWNCAST(null, Function);
    this.data->catch = DOWNCAST(null, Function);
}

IMPLEMENT_CONSTRUCTOR(exception, Throwable exception) {
    this.data->exception = exception;
    this.data->value = null;
    this.data->state = FutureState$completedWithError;
    this.data->then = DOWNCAST(null, Function);
    this.data->catch = DOWNCAST(null, Function);
}

IMPLEMENT_CONSTRUCTOR(_) {
    this.data->exception = DOWNCAST(null, Throwable);
    this.data->value = null;
    this.data->state = FutureState$pending;
    this.data->then = DOWNCAST(null, Function);
    this.data->catch = DOWNCAST(null, Function);
    this.data->attachedLoop = DOWNCAST(null, EventLoop);
}

#undef Super
#undef Self
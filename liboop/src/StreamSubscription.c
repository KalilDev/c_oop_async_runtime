#include <assert.h>
#include "liboop.h"

#define Super() Object_vtable()
#define Self StreamSubscription
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_STREAM_SUBSCRIPTION_PARENTS)

ENUMERATE_STREAM_SUBSCRIPTION_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

#define ENUMERATE_CODONM_CAPTURES(CAPTURE) \
    CAPTURE(Object, value)                 \
    CAPTURE(Function, onData)

IMPLEMENT_LAMBDA(CompleteOnDataOnNextMicrotask, ENUMERATE_CODONM_CAPTURES, NO_OWNED_CAPTURES, Object value, Function onData) {
    Lambda_CompleteOnDataOnNextMicrotask self = DOWNCAST(this, Lambda_CompleteOnDataOnNextMicrotask);
    Object value = self.data->value;
    Function onData = self.data->onData;
    THROWS = va_arg(args, Throwable*);
    Object res = Function_call(onData, 2, value, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    return res;
}


#define ENUMERATE_COEONM_CAPTURES(CAPTURE) \
    CAPTURE(Throwable, exception)                 \
    CAPTURE(Function, onError)

IMPLEMENT_LAMBDA(CompleteOnErrorOnNextMicrotask, ENUMERATE_COEONM_CAPTURES, NO_OWNED_CAPTURES, Throwable exception, Function onError) {
    Lambda_CompleteOnErrorOnNextMicrotask self = DOWNCAST(this, Lambda_CompleteOnErrorOnNextMicrotask);
    Throwable exception = self.data->exception;
    Function onError = self.data->onError;
    THROWS = va_arg(args, Throwable*);
    Object res = Function_call(onError, 2, exception, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    return res;
}


#define ENUMERATE_CONONM_CAPTURES(CAPTURE) \
    CAPTURE(Function, onDone)

IMPLEMENT_LAMBDA(CompleteOnDoneOnNextMicrotask, ENUMERATE_CONONM_CAPTURES, NO_OWNED_CAPTURES, Function onDone) {
    Lambda_CompleteOnDoneOnNextMicrotask self = DOWNCAST(this, Lambda_CompleteOnDoneOnNextMicrotask);
    Function onDone = self.data->onDone;
    THROWS = va_arg(args, Throwable*);
    Object res = Function_call(onDone, 1, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    return res;
}

#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(StreamSubscription, myself)

IMPLEMENT_LAMBDA(CancelSelfOnNextMicrotask, CAPTURE_MYSELF, NO_OWNED_CAPTURES, StreamSubscription myself) {
    Lambda_CancelSelfOnNextMicrotask self = DOWNCAST(this, Lambda_CancelSelfOnNextMicrotask);
    StreamSubscription myself = self.data->myself;
    THROWS = va_arg(args, Throwable*);
    return StreamSubscription_cancel(myself).asObject;
}

IMPLEMENT_SELF_METHOD(Future, handleData, Object data) {
    assert(!IS_OBJECT_ASSIGNABLE(data, Future));
    assert(!Object_isNull(this.data->attachedLoop.asObject));
    Function computation = Lambda_CompleteOnDataOnNextMicrotask$make_new(data, this.data->onData).asFunction;
    return Future_computationAt(computation, this.data->attachedLoop);
}

IMPLEMENT_SELF_METHOD(Future, handleError, Throwable error) {
    assert(!IS_OBJECT_ASSIGNABLE(error.asObject, Future));
    if (Object_isNull(this.data->attachedLoop.asObject)) {
        Thread thread = Thread_current();
        Thread_unhandledAsyncException(thread, error);
        return Future$make_value(null);
    }
    assert(!Object_isNull(this.data->attachedLoop.asObject));
    Function computation = Lambda_CompleteOnErrorOnNextMicrotask$make_new(error, this.data->onError).asFunction;
    Future res = Future_computationAt(computation, this.data->attachedLoop);
    if (this.data->cancelOnError && !this.data->cancelScheduled) {
        Future_computationAt(Lambda_CancelSelfOnNextMicrotask$make_new(this).asFunction, this.data->attachedLoop);
    }
    return res;
}

IMPLEMENT_SELF_METHOD(Future, handleDone) {
    assert(!Object_isNull(this.data->attachedLoop.asObject));
    Function computation = Lambda_CompleteOnDoneOnNextMicrotask$make_new(this.data->onDone).asFunction;
    return Future_computationAt(computation, this.data->attachedLoop);
}

IMPLEMENT_SELF_METHOD(Future, cancel) {
    if (this.data->cancelScheduled) {
        // TODO: are there cases when this is an error, or every case is expected?
        return Future$make_value(null);
    }
    this.data->cancelScheduled = true;
    assert(!Object_isNull(this.data->attachedLoop.asObject));
    Object res = Function_call(this.data->onCancel, 0);
    EventLoop loop = this.data->attachedLoop;
    this.data->attachedLoop = DOWNCAST(null, EventLoop);
    EventLoop_removeSubscription(loop, this);
    return Future$$fromObject(res);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            0
    );
    // StreamSubscription
    vtable->cancel = _StreamSubscription_cancel_impl;
    vtable->handleData = _StreamSubscription_handleData_impl;
    vtable->handleError = _StreamSubscription_handleError_impl;
    vtable->handleDone = _StreamSubscription_handleDone_impl;
}

IMPLEMENT_CONSTRUCTOR(new, Function onData, Function onError, Function onDone, Function onCancel, Bool cancelOnError) {
    this.data->onData = onData;
    this.data->onError = onError;
    this.data->onDone = onDone;
    this.data->onCancel = onCancel;
    this.data->cancelOnError = Object_isNull(cancelOnError.asObject) ? false : cancelOnError.unwrap;
    this.data->cancelScheduled = false;
    this.data->attachedLoop = EventLoop_current();
    assert(!Object_isNull(this.data->attachedLoop.asObject));
    assert(this.data->attachedLoop.data != NULL);
    EventLoop_addSubscription(this.data->attachedLoop, this);
}

IMPLEMENT_SELF_GETTER(bool, cancelOnError) {
    return this.data->cancelOnError;
}

#undef Self
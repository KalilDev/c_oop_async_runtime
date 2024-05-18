#include "StreamSubscription.h"
#include "Function.h"
#include "Future.h"
#include "Throwable.h"
#include "oop.h"
#include <assert.h>
#include "primitive/StringRef.h"
#include "primitive/Bool.h"
#include "Socket.h"

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
    printf("completeOnData\n");
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

IMPLEMENT_SELF_METHOD(Future, handleData, Object data) {
    if (this.data->cancelled) {
        // todo: free error
        return Future$make__();
    }
    printf("handleData\n");
    Function computation = Lambda_CompleteOnDataOnNextMicrotask$make_new(data, this.data->onData).asFunction;
    return Future_computationAt(computation, this.data->listenerLoop);
}

IMPLEMENT_SELF_METHOD(Future, handleError, Throwable error) {
    if (this.data->cancelled) {
        // todo: free error
        return Future$make__();
    }
    if (this.data->cancelOnError) {
        StreamSubscription_cancel(this);
    }
    Function computation = Lambda_CompleteOnErrorOnNextMicrotask$make_new(error, this.data->onError).asFunction;
    return Future_computationAt(computation, this.data->listenerLoop);
}

IMPLEMENT_SELF_METHOD(Future, handleDone) {
    if (this.data->cancelled) {
        // todo: free error
        return Future$make__();
    }
    Function computation = Lambda_CompleteOnDoneOnNextMicrotask$make_new(this.data->onDone).asFunction;
    return Future_computationAt(computation, this.data->listenerLoop);
}

IMPLEMENT_SELF_METHOD(Future, cancel) {
    if (this.data->cancelled) {
        // todo: free error
        return Future$make__();
    }
    this.data->cancelled = true;
    Object res = Function_call(this.data->onCancel, 0);
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
    this.data->cancelled = false;
    this.data->listenerLoop = EventLoop_current();
}

IMPLEMENT_SELF_GETTER(bool, cancelOnError) {
    return this.data->cancelOnError;
}

#undef Self
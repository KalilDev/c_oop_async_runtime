#include "Object.h"
#include "Completer.h"
#include "Future.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "oop.h"
#include "foreach.h"
#include <assert.h>

#define Super() Object_vtable()
#define Self Completer
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_COMPLETER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_THROWABLE_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    // TODO
}

IMPLEMENT_SELF_METHOD(void, complete, Object value) {
    // TODO: Remove
    if (IS_OBJECT_ASSIGNABLE(value, Future)) {
        return Completer_follow(this, Future$$fromObject(value));
    }
    Future fut = this.data->future;
    assert(fut.data->state == FutureState$pending);
    fut.data->value = value;
    Future__onComplete(fut);
}

IMPLEMENT_SELF_METHOD(void, completeException, Throwable exception) {
    assert(!IS_OBJECT_ASSIGNABLE(exception.asObject, Future));
    assert(!Object_isNull(exception.asObject));
    Future fut = this.data->future;
    assert(fut.data->state == FutureState$pending);
    fut.data->exception = exception;
    Future__onCompleteWithError(fut);
}

#define ENUMERATE_COMPLETE_THEN_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

#define ENUMERATE_COMPLETE_THEN_OWNED_CAPTURES(CAPTURE) \
    CAPTURE(completer)

IMPLEMENT_LAMBDA(CompleteThen, ENUMERATE_COMPLETE_THEN_CAPTURES, ENUMERATE_COMPLETE_THEN_OWNED_CAPTURES, Completer completer) {
    Lambda_CompleteThen self = DOWNCAST(this, Lambda_CompleteThen);
    Completer completer = self.data->completer;
    Object res = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);
    Completer_complete(completer, res);
    return null;
}


IMPLEMENT_LAMBDA(CompleteCatch, ENUMERATE_COMPLETE_THEN_CAPTURES, ENUMERATE_COMPLETE_THEN_OWNED_CAPTURES, Completer completer) {
    Lambda_CompleteCatch self = DOWNCAST(this, Lambda_CompleteCatch);
    Completer completer = self.data->completer;
    Throwable exc = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);
    Completer_completeException(completer, exc);
    return null;
}


IMPLEMENT_SELF_METHOD(void, follow, Future future) {
    Lambda_CompleteThen then = Lambda_CompleteThen$make_new(this);
    Lambda_CompleteCatch catch = Lambda_CompleteCatch$make_new(this);
    Future_setCallbacks(future, then.asFunction, catch.asFunction);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // Completer
    vtable->complete = _Completer_complete_impl;
    vtable->completeException = _Completer_completeException_impl;
    vtable->follow = _Completer_follow_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Completer_delete_impl;
}

IMPLEMENT_SELF_GETTER(Future, future) {
    return this.data->future;
}

IMPLEMENT_CONSTRUCTOR(new) {
    this.data->future = Future$make__();
}

#undef Super
#undef Self
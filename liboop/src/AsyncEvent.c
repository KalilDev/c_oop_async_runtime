#include "Object.h"
#include "oop.h"
#include <assert.h>
#include "String.h"
#include "primitive/StringRef.h"
#include "Throwable.h"
#include "Future.h"
#include "AsyncEvent.h"
#include "StreamController.h"
#include "primitive/Bool.h"


#define Super() Object_vtable()
#define Self AsyncEvent
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_ASYNC_EVENT_PARENTS)

ENUMERATE_ASYNC_EVENT_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    AsyncEvent self = DOWNCAST(this, AsyncEvent);
    Object_delete(self.data->value);
    Super()->delete(this);
}

IMPLEMENT_SELF_METHOD(void, addToStreamSubscription, StreamSubscription subscription) {
    AsyncEventType type = this.data->type;
    Object value = this.data->value;
    StreamSubscription subs = subscription;
    switch (type) {
        case AsyncEventType$done:
            StreamSubscription_handleDone(subs);
            break;
        case AsyncEventType$error:
            StreamSubscription_handleError(subs, Throwable$$fromObject(value));
            break;
        case AsyncEventType$value:
            StreamSubscription_handleData(subs, value);
            break;
    }
    this.data->value = null;
}


IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // AsyncEvent
    vtable->addToStreamSubscription = _AsyncEvent_addToStreamSubscription_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _AsyncEvent_delete_impl;
}

IMPLEMENT_CONSTRUCTOR(value, Object value) {
    this.data->type = AsyncEventType$value;
    this.data->value = value;
}


IMPLEMENT_CONSTRUCTOR(done) {
    this.data->type = AsyncEventType$done;
    this.data->value = null;
}

IMPLEMENT_CONSTRUCTOR(error, Throwable error) {
    this.data->type = AsyncEventType$error;
    this.data->value = error.asObject;
}

#undef Super
#undef Self
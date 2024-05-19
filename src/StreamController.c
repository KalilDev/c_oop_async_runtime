#include "Object.h"
#include "oop.h"
#include <assert.h>
#include "String.h"
#include "primitive/StringRef.h"
#include "Throwable.h"
#include "Future.h"
#include "Function.h"
#include "StreamSubscription.h"
#include "StreamController.h"
#include "primitive/Bool.h"
#include "AsyncEvent.h"
#include "foreach.h"


#define Super() Object_vtable()
#define Self StreamController
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_STREAM_CONTROLLER_PARENTS)

ENUMERATE_STREAM_CONTROLLER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    StreamController self = DOWNCAST(this, StreamController);
    Object_delete(self.data->onListen.asObject);
    Object_delete(self.data->onCancelSubscription.asObject);
    Object_delete(self.data->subs.asObject);
    Object_delete(self.data->bufferedEvents.asObject);
    Super()->delete(this);
}

IMPLEMENT_OVERRIDE_METHOD(void, Sink, add, Object value) {
    Self self = Sink_as_StreamController(this);
    StreamSubscription subs = self.data->subs;
    if (!Object_isNull(subs.asObject)) {
        StreamSubscription_handleData(subs, value);
        return;
    }
    List bufferedEvents = self.data->bufferedEvents;
    List_add(bufferedEvents, AsyncEvent$make_value(value).asObject, CRASH_ON_EXCEPTION);
}


IMPLEMENT_OVERRIDE_METHOD(void, Sink, close) {
    Self self = Sink_as_StreamController(this);
    StreamSubscription subs = self.data->subs;
    if (!Object_isNull(subs.asObject)) {
        StreamSubscription_handleDone(subs);
        return;
    }
    List bufferedEvents = self.data->bufferedEvents;
    List_add(bufferedEvents, AsyncEvent$make_done().asObject, CRASH_ON_EXCEPTION);
}

IMPLEMENT_SELF_METHOD(void, addError, Throwable error) {
    Self self = DOWNCAST(this, Self);
    StreamSubscription subs = self.data->subs;
    if (!Object_isNull(subs.asObject)) {
        StreamSubscription_handleError(subs, error);
        return;
    }
    List bufferedEvents = self.data->bufferedEvents;
    List_add(bufferedEvents, AsyncEvent$make_error(error).asObject, CRASH_ON_EXCEPTION);
}
#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(StreamController, myself)

IMPLEMENT_LAMBDA(OnCancel, CAPTURE_MYSELF, NO_OWNED_CAPTURES, StreamController myself) {
    Lambda_OnCancel self = DOWNCAST(this, Lambda_OnCancel);
    StreamController myself = self.data->myself;
    Function onCancel = myself.data->onCancelSubscription;

    return Function_call(onCancel, 1, myself);
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = Stream_as_StreamController(this);
    Function onCancel = Lambda_OnCancel$make_new(self).asFunction;
    StreamSubscription subs = StreamSubscription$make_new(onData, onError, onDone, onCancel, cancelOnError);
    self.data->subs = subs;
    List bufferedEvents = self.data->bufferedEvents;
    if (List_length(bufferedEvents) != 0) {
        foreach(AsyncEvent, event, List_as_Iterable(bufferedEvents), {
            // todo: cancel on error
            // todo: what if the listener cancels?
            AsyncEvent_addToStreamSubscription(event, subs);
        })
        List_setLength(bufferedEvents, 0, CRASH_ON_EXCEPTION);
    }
    // TODO: Exception: and if there were, close the controller
    // TODO: Should this be async?: Nope
    Function_call(self.data->onListen, 2, self, subs);
    return subs;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
2,
        "Stream", offsetof(StreamController_vtable_t , Stream_vtable),
            "Sink", offsetof(StreamController_vtable_t , Sink_vtable)
        );
    // StreamController
    vtable->addError = _StreamController_addError_impl;
    // Sink
    Sink_vtable_t *sink_vtable = &vtable->Sink_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)sink_vtable,
            (Interface_vtable_t*)Sink_vtable(),
            sizeof(*Sink_vtable()),
            offsetof(struct StreamController_vtable_t, Sink_vtable)
    );
    sink_vtable->add = _StreamController_add_impl;
    sink_vtable->close = _StreamController_close_impl;
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)stream_vtable,
            (Interface_vtable_t*)Stream_vtable(),
            sizeof(*Stream_vtable()),
            offsetof(struct StreamController_vtable_t , Stream_vtable)
    );
    stream_vtable->listen = _StreamController_listen_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _StreamController_delete_impl;
}

OBJECT_CAST_IMPL(Stream, StreamController)
INTERFACE_CAST_IMPL(StreamController, Stream, Object)


OBJECT_CAST_IMPL(Sink, StreamController)
INTERFACE_CAST_IMPL(StreamController, Sink, Object)

IMPLEMENT_CONSTRUCTOR(new, Function onListen, Function onCancelSubscription) {
    this.data->onListen = onListen;
    this.data->onCancelSubscription = onCancelSubscription;
    this.data->subs = DOWNCAST(null, StreamSubscription);
    this.data->bufferedEvents = List_new();
}

IMPLEMENT_STATIC_METHOD(void, add, StreamController this, Object value) {
    return Sink_add(StreamController_as_Sink(this), value);
}

IMPLEMENT_STATIC_METHOD(void, close, StreamController this) {
    return Sink_close(StreamController_as_Sink(this));
}

#undef Super
#undef Self
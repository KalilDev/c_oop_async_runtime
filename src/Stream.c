#include "Stream.h"
#include "StreamSubscription.h"
#include "Function.h"
#include "Future.h"
#include "primitive/Bool.h"
#include "oop.h"
#include "Completer.h"
#include <assert.h>


#define Self Stream

SUPER_INTERFACE_CAST_IMPL(Stream, Interface)

OBJECT_CAST_IMPL(Stream, Object)

ENUMERATE_STREAM_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

#define TO_LIST_CAPTURES(CAPTURE) \
    CAPTURE(List, out)                              \
    CAPTURE(Completer, completer) \
    CAPTURE(StreamSubscription*, subs)

IMPLEMENT_LAMBDA(ToListOnData, TO_LIST_CAPTURES, NO_OWNED_CAPTURES, List out, Completer completer, StreamSubscription* subs) {
    Lambda_ToListOnData self = DOWNCAST(this, Lambda_ToListOnData);
    List out = self.data->out;
    Completer completer = self.data->completer;
    StreamSubscription *subs = self.data->subs;
    Object data = va_arg(args, Object);

    List_add(out, data, CRASH_ON_EXCEPTION);
}

IMPLEMENT_LAMBDA(ToListOnError, TO_LIST_CAPTURES, NO_OWNED_CAPTURES, List out, Completer completer, StreamSubscription* subs) {
    Lambda_ToListOnError self = DOWNCAST(this, Lambda_ToListOnError);
    List out = self.data->out;
    Completer completer = self.data->completer;
    StreamSubscription *subs = self.data->subs;
    Throwable error = va_arg(args, Throwable);
    *subs = DOWNCAST(null, StreamSubscription);
    Object_delete(subs->asObject);
    Completer_completeException(completer, error);
}

IMPLEMENT_LAMBDA(ToListOnDone, TO_LIST_CAPTURES, NO_OWNED_CAPTURES, List out, Completer completer, StreamSubscription* subs) {
    Lambda_ToListOnDone self = DOWNCAST(this, Lambda_ToListOnDone);
    List out = self.data->out;
    Completer completer = self.data->completer;
    StreamSubscription *subs = self.data->subs;
    *subs = DOWNCAST(null, StreamSubscription);
    Object_delete(subs->asObject);
    Completer_complete(completer, out.asObject);
}

IMPLEMENT_SELF_METHOD(Future, toList) {
    List out = List_new();
    Completer completer = Completer$make_new();
    StreamSubscription *subs = malloc(sizeof(StreamSubscription));
    *subs = DOWNCAST(null, StreamSubscription);
    *subs = Stream_listen(
        this,
        Lambda_ToListOnData$make_new(out, completer, subs).asFunction,
        Lambda_ToListOnError$make_new(out, completer, subs).asFunction,
        Lambda_ToListOnDone$make_new(out, completer, subs).asFunction,
        True
    );
    return Completer_future(completer);
}

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
            (Interface_vtable_t *) vtable,
            Interface_vtable(),
            sizeof(*Interface_vtable())
    );
    // Stream
    vtable->toList = _Stream_toList_impl;
}

#undef Self